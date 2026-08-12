#!/usr/bin/env python3
# relay.py — device loop: pull -> build -> install -> launch -> collect logs/panics -> push results
# deps: Xcode CLI, libimobiledevice (brew install libimobiledevice)
import os, re, sys, time, shutil, subprocess, datetime, pathlib, shlex

ROOT = pathlib.Path(__file__).resolve().parent.parent
RESULTS = ROOT / "results"
BUNDLE_ID = "cancer9725.turquoise1323"

def sh(cmd, check=False, timeout=None, capture=True):
    print("$", cmd, flush=True)
    r = subprocess.run(cmd, shell=True, capture_output=capture, text=True, timeout=timeout)
    if check and r.returncode != 0:
        print(r.stdout, r.stderr); sys.exit(1)
    return r

def pull():
    status = sh("git -C %s status --porcelain" % ROOT)
    if status.stdout.strip():
        print("[pull] skipped: local changes are present; preserving working tree")
        return
    sh("git -C %s pull --rebase" % ROOT, check=True)

def device_id():
    configured = os.environ.get("DEVICE_ID")
    if configured:
        return configured
    r = sh("idevice_id -l")
    ids = [line.strip() for line in r.stdout.splitlines() if line.strip()]
    if not ids:
        print("[device] no connected iPhone found; set DEVICE_ID explicitly")
        sys.exit(1)
    return ids[0]

def build():
    sh(str(ROOT / "relay/build.sh"), check=True)

def install():
    dev = shlex.quote(device_id())
    app = shlex.quote(str(ROOT / "build/fuzz27.app"))
    r = sh(f"xcrun devicectl device install app --device {dev} {app}")
    print(r.stdout, r.stderr, sep="", end="")
    if r.returncode != 0:
        if shutil.which("ideviceinstaller"):
            sh(f"ideviceinstaller -i {app}", check=True)
        else:
            print("[install] devicectl failed and ideviceinstaller is not installed")
            sys.exit(1)
    # verify the app is really installed — silent install failures otherwise
    r = sh(f"xcrun devicectl device info apps --device {dev} | grep -i turquoise || true")
    if "turquoise" not in (r.stdout or ""):
        print("[install][warn] app not listed after install — check provisioning/signing")
    else:
        print("[install] verified on device")

def launch_and_watch(minutes):
    ts = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    log = RESULTS / f"run-{ts}.log"
    RESULTS.mkdir(exist_ok=True)
    dev = shlex.quote(device_id())
    bid = shlex.quote(BUNDLE_ID)
    with open(log, "w") as lf:
        # 1) try devicectl console; 2) in parallel tail idevicesyslog (unified log)
        p1 = subprocess.Popen(
            f"xcrun devicectl device process launch --device {dev} --console {bid}",
            shell=True, stdout=lf, stderr=subprocess.STDOUT, text=True)
        p2 = subprocess.Popen(
            "idevicesyslog -m fuzz27 -m vcpdrm -m scaler -m mig 2>/dev/null || "
            "idevicesyslog 2>/dev/null | grep -E 'fuzz27|vcpdrm|scaler|VCPDRM' ",
            shell=True, stdout=lf, stderr=subprocess.DEVNULL, text=True)
        t0 = time.time()
        while time.time() - t0 < minutes * 60:
            time.sleep(20)
            r = sh("idevicesyslog -m 'panic' 2>/dev/null | tail -5")
            if r.stdout.strip():
                lf.write("\n=== PANIC DETECTED (syslog match) ===\n" + r.stdout)
                break
            # if devicectl died early, report its status into the log
            if p1.poll() is not None and (time.time() - t0) > 30:
                lf.write(f"\n=== devicectl launch exited rc={p1.returncode} ===\n")
                break
        for p in (p1, p2):
            try: p.terminate()
            except Exception: pass
    return log

def collect_panics():
    out = RESULTS / "panics"
    out.mkdir(parents=True, exist_ok=True)
    timeout = int(os.environ.get("CRASHREPORT_TIMEOUT", "300"))
    try:
        sh(f"idevicecrashreport -e -k {shlex.quote(str(out))}", timeout=timeout)
    except subprocess.TimeoutExpired:
        print(f"[crashreport] timed out after {timeout}s; keeping partial results")

def push(log):
    sh("git -C %s add results/" % ROOT)
    sh("git -C %s commit -m 'results %s' || true" % (ROOT, datetime.datetime.now()))
    sh("git -C %s push" % ROOT)

def main():
    minutes = int(sys.argv[1]) if len(sys.argv) > 1 else 30
    pull()
    build()
    install()
    log = launch_and_watch(minutes)
    collect_panics()
    push(log)
    print("done ->", log)

if __name__ == "__main__":
    main()
