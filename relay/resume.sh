#!/bin/bash
# resume.sh — post-reboot recovery: verify dev channel, install, run dsrecon recon.
# usage: relay/resume.sh
set -u
cd "$(dirname "$0")/.."
DEV="${DEVICE_ID:-8A8A1D3F-AF75-5493-9585-6374D1BB90D1}"

echo "[resume] waiting for device..."
for i in $(seq 1 90); do
    idevice_id -l 2>/dev/null | grep -q . && break
    sleep 20
done
xcrun devicectl list devices 2>/dev/null | grep -q 'available (paired)' || {
    echo "[resume] device not fully paired/unlocked yet — rerun after unlock"; exit 1; }

# dev channel sanity: plain launch of another app
if ! xcrun devicectl device process launch --device "$DEV" com.roooot.mond >/dev/null 2>&1; then
    echo "[resume] launch channel broken (10002) — needs device reboot"; exit 1
fi
sleep 2
PID=$(xcrun devicectl device info processes --device "$DEV" 2>/dev/null | awk '/mond/ {print $1; exit}')
[ -n "$PID" ] && xcrun devicectl device process terminate --device "$DEV" --pid "$PID" >/dev/null 2>&1

echo "[resume] channel OK, installing current build"
xcrun devicectl device install app --device "$DEV" build/fuzz27.app 2>&1 | grep -c 'App installed'
echo "[resume] running dsrecon"
relay/run_phase.sh results/run-dsrecon-postreboot.log 240 FUZZ_MODE=scaler FUZZ_DSRECON=1 FUZZ_LOGFILE=1
grep -cE 'dsr' results/run-dsrecon-postreboot.log 2>/dev/null || true
