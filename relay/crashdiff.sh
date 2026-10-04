#!/bin/bash
# crashdiff.sh — snapshot the device's crash log index, run something, diff.
#
# Usage: ./relay/crashdiff.sh <label> -- <command...>
#        ./relay/crashdiff.sh <label>          # diff only, no command
#
# Why this exists: every negative result in this project has to be backed by a
# detector that is known to fire. Five of them were not — the phase never ran,
# or the signal we were watching for was the wrong one, and the "no bug" was
# really "no evidence of a bug". A crash detector that has never been seen to
# detect a crash is indistinguishable from no detector at all. So this script
# makes the detector falsifiable: it snapshots the device crash corpus on both
# sides of a command and prints only what is new.
#
# The corpus is read through the systemCrashLogs domain, which is the one
# device-global filesystem a sandboxed app's host can enumerate. It already
# contains hundreds of unrelated third-party crashes, which is exactly why the
# diff has to be on names+dates rather than on counts.
set -u
DEV=8A8A1D3F-AF75-5493-9585-6374D1BB90D1
LABEL=${1:-diff}; shift || true
BEFORE=/tmp/crash-before.txt
AFTER=/tmp/crash-after.txt

snap() {
  xcrun devicectl device info files --device $DEV \
    --domain-type systemCrashLogs --subdirectory / 2>/dev/null \
  | awk 'NR>3 && NF {print $1, $NF}' | sort
}

snap > "$BEFORE"
BEFORE_N=$(wc -l < "$BEFORE")

if [ "${1:-}" = "--" ]; then
  shift
  echo "[crashdiff] running: $*"
  "$@"
  # crash reporting is asynchronous: the report is written by ReportCrash after
  # the corpse is reaped, so an immediate read races it. Give it room rather
  # than reporting a false negative.
  sleep "${CRASHDIFF_WAIT:-25}"
fi

snap > "$AFTER"
echo "[crashdiff] corpus $BEFORE_N -> $(wc -l < "$AFTER") entries"
NEW=$(comm -13 "$BEFORE" "$AFTER")
if [ -z "$NEW" ]; then
  echo "[crashdiff] $LABEL: no new crash reports"
else
  echo "[crashdiff] $LABEL: NEW CRASH REPORTS"
  echo "$NEW" | sed 's/^/    /'
fi
