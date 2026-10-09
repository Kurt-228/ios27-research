#!/bin/bash
# runf.sh — launch one fuzz phase on the device and pull its log back.
#
# Usage: ./relay/runf.sh <tag> [ENV=VAL ...]
#
# Written because this cycle got repeated a dozen times and every repetition
# was a chance to misread a stale log. The invariant the project keeps
# tripping over (§104, and five negative results that turned out to be
# "the phase never ran"): a log is only evidence if it belongs to THIS run.
# So this script kills the previous instance, truncates the log file, and
# verifies the fresh log carries this tag before printing anything. If the
# marker is missing it says so loudly instead of letting a stale file speak.
set -u
DEV=8A8A1D3F-AF75-5493-9585-6374D1BB90D1
BID=cancer9725.turquoise1323
HERE=$(cd "$(dirname "$0")" && pwd)
TAG=$1; shift
LOG=/tmp/runf-$TAG.log
: > /tmp/empty.log

if [ ! -d "$HERE/../build/fuzz27.app" ]; then
  echo "[runf] build/fuzz27.app missing — build first"; exit 1
fi

PID=$(xcrun devicectl device info processes --device $DEV 2>/dev/null \
      | awk '/fuzz27/{print $1;exit}')
if [ -n "$PID" ]; then
  xcrun devicectl device process terminate --device $DEV --pid "$PID" >/dev/null 2>&1
  sleep 3
fi
xcrun devicectl device copy to --device $DEV --domain-type appDataContainer \
  --domain-identifier $BID --source /tmp/empty.log \
  --destination Documents/fuzz.log >/dev/null 2>&1

ENVS="FUZZ_LOGFILE=1"
for kv in "$@"; do ENVS="$ENVS $kv"; done

# Launch errors used to vanish into /dev/null (v181: two silent failures
# cost a cycle — the phase never ran while the script waited regardless).
LAUNCH_OUT=$(xcrun devicectl device process launch --device $DEV $BID $ENVS 2>&1)
if ! printf '%s' "$LAUNCH_OUT" | grep -qi 'Launched application'; then
  echo "[runf] launch FAILED — phase will not run:"
  printf '%s\n' "$LAUNCH_OUT"
  exit 3
fi

WAIT=${RUNF_WAIT:-30}
sleep "$WAIT"
xcrun devicectl device copy from --device $DEV --domain-type appDataContainer \
  --domain-identifier $BID --source Documents/fuzz.log \
  --destination "$LOG" >/dev/null 2>&1

if ! grep -q "\[$TAG\]" "$LOG" 2>/dev/null; then
  echo "[runf] WARNING: no [$TAG] marker in fresh log $LOG — phase may not have run"
  echo "[runf] log size $(wc -c < "$LOG" 2>/dev/null || echo missing)"
  exit 2
fi
echo "[runf] $LOG  ($(wc -l < "$LOG") lines)"
