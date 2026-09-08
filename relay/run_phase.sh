#!/bin/bash
# run_phase.sh — robust one-shot phase runner (post-iOS27b4 devicectl quirks).
# usage: relay/run_phase.sh <logfile> <wait_secs> <KEY=VAL> [KEY=VAL...]
#
# Flow: launch (no --console, phase config as KEY=VAL argv parsed in main.m)
#       -> wait up to <wait_secs> (early exit when the process disappears,
#          e.g. victim SIGKILL or a crash)
#       -> terminate the app if still alive
#       -> pull Documents/fuzz.log from the app container into <logfile>
#          (requires FUZZ_LOGFILE=1 among the KEY=VAL args).
set -u
cd "$(dirname "$0")/.."
DEV="${DEVICE_ID:-8A8A1D3F-AF75-5493-9585-6374D1BB90D1}"
BID="cancer9725.turquoise1323"
LOG="$1"; WAIT="$2"; shift 2

find_pid() {
    xcrun devicectl device info processes --device "$DEV" 2>/dev/null \
        | awk '/fuzz27\.app\/fuzz27/ {print $1; exit}'
}

# a still-running previous instance makes the next launch fail with
# CoreDevice 10002 EINVAL — kill it first
old=$(find_pid)
[ -n "$old" ] && { echo "[run_phase] killing leftover pid $old"; \
    xcrun devicectl device process terminate --device "$DEV" --pid "$old" >/dev/null 2>&1; sleep 3; }

# pre-clear the container log so a stale one can never be mistaken for a
# fresh result (silent-miss failure mode seen repeatedly on 08.09)
: > /tmp/fuzz27-empty.log
xcrun devicectl device copy to --device "$DEV" \
    --domain-type appDataContainer --domain-identifier "$BID" \
    --source /tmp/fuzz27-empty.log --destination Documents/fuzz.log >/dev/null 2>&1

for attempt in 1 2 3; do
    xcrun devicectl device process launch --device "$DEV" "$BID" "$@"  >/dev/null 2>&1 \
     || xcrun devicectl device process launch --device "$DEV" "$BID" -- "$@" >/dev/null 2>&1 \
     || { echo "[run_phase] launch failed"; [ $attempt -lt 3 ] && { sleep 10; continue; }; exit 1; }
    echo "[run_phase] launched: $* (attempt $attempt)"

    # verify the process actually started (silent drop = wedged channel)
    sleep 4
    pid=$(find_pid)
    if [ -z "$pid" ]; then
        echo "[run_phase] launch dropped silently (channel wedged), retry in 20s"
        sleep 20
        continue
    fi

    t0=$(date +%s)
    while :; do
        sleep 5
        pid=$(find_pid)
        [ -z "$pid" ] && { echo "[run_phase] app exited on its own"; break; }
        [ $(( $(date +%s) - t0 )) -ge "$WAIT" ] && {
            echo "[run_phase] wait cap ${WAIT}s, terminating pid $pid"
            xcrun devicectl device process terminate --device "$DEV" --pid "$pid" >/dev/null 2>&1
            sleep 2
            break
        }
    done

    # container write may lag process death slightly
    sleep 3
    xcrun devicectl device copy from --device "$DEV" \
        --domain-type appDataContainer --domain-identifier "$BID" \
        --source Documents/fuzz.log --destination "$LOG" >/dev/null 2>&1
    if [ -s "$LOG" ] && grep -q 'fuzz harness up' "$LOG"; then
        echo "[run_phase] log pulled -> $LOG ($(wc -l < "$LOG" | tr -d ' ') lines)"
        exit 0
    fi
    echo "[run_phase] empty/stale log after run (attempt $attempt), relaunching"
done
echo "[run_phase] all attempts failed to produce a fresh log"
exit 1
