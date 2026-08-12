#!/usr/bin/env python3
# relay.py — device loop: pull -> build -> install -> launch -> collect logs/panics -> push results
# deps: Xcode CLI, libimobiledevice (brew install libimobiledevice)
import os, re, sys, time, shutil, subprocess, datetime, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
RESULTS = ROOT / "results"
BUNDLE_ID = "local.fuzz27.harness"

def sh(cmd, check=False, timeout=None, capture=True):
    print("$", cmd, flush=True)
    r = subprocess.run(cmd, shell=True, capture_output=capture, text=True, timeout=timeout)
    if check and r.returncode != 0:
        print(r.stdout, r.stderr); sys.exit(1)
    return r

def pull():
    sh("git -C %s pull --rebase" % ROOT, check=True)

def build():
    sh(str(ROOT / "relay/build.sh"), check=True)

def install():
    # modern path: devicectl; fallback: ideviceinstaller
    r = sh(f"xcrun devicectl device install app {ROOT}/build/fuzz27.app")
    if r.returncode != 0:
        sh(f"ideviceinstaller -i {ROOT}/build/fuzz27.app", check=True)

def launch_and_watch(minutes):
    ts = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    log = RESULTS / f"run-{ts}.log"
    RESULTS.mkdir(exist_ok=True)
    with open(log, "w") as lf:
        # launch with console streaming via devicectl if possible
        p = subprocess.Popen(
            f"xcrun devicectl device process launch --console {BUNDLE_ID}",
            shell=True, stdout=lf, stderr=subprocess.STDOUT, text=True)
        t0 = time.time()
        while time.time() - t0 < minutes * 60:
            time.sleep(20)
            # detect device reboot (panic) — syslog dies
            r = sh("idevicesyslog -m 'panic' 2>/dev/null | tail -5")
            if r.stdout.strip():
                lf.write("\n=== PANIC DETECTED (syslog match) ===\n" + r.stdout)
                break
        p.terminate()
    return log

def collect_panics():
    # idevicecrashreport copies crash logs off the device
    out = RESULTS / "panics"
    out.mkdir(parents=True, exist_ok=True)
    sh(f"idevicecrashreport -e -k {out}")


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
