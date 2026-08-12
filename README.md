# ios27-fuzz

Kernel fuzz harness for **iOS 27.0 beta 4 (A17 / iPhone 15 Pro Max)**, plus a device relay that builds, deploys, runs and collects panics. Private research repo.

## Targets (from static analysis, see research notes)

| Target | Entry | Notes |
|---|---|---|
| **VCPDRMServiceUserClient** (new kext in 27.0) | `IOServiceOpen("VCPDRMService")` | 3 selectors, input = one u64 (id 1..0x20); **race phase**: two threads register/unregister same id — slot table has no visible locks |
| **AppleM2ScalerCSCDriver** (rewritten in 27.0) | IOSurface-attached | fuzzes crafted M2ScalerCSCRequest (0x1b0) — dims/floats/flags/surface-id pairs per mapped layout |
| **MIG ApplePrivate SPRR subsystem** | bootstrap ports | msgh_id sweep 0x3000_0000–0x3200_FFFF, watches for non-MIG_BAD_ID replies |

## Setup (one time)

1. macOS with Xcode (CLI tools: `xcode-select --install`).
2. `brew install libimobiledevice`
3. iPhone 15 Pro Max on 27.0b4, **developer mode on**, paired with the Mac.
4. Your purchased signing certificate in the keychain (auto-detected by `relay/build.sh`; override with `CSC_NAME="Your Cert Name"`).
5. `git clone git@github.com:Kurt-228/ios27-fuzz.git && cd ios27-fuzz && git config user.email/name`

## Run

```sh
python3 relay/relay.py 30     # 30 minutes of fuzz, then collect panics & push results/
```

Each cycle: `git pull → build (clang, no xcodeproj) → devicectl install → launch --console → watch for panics → idevicecrashreport → git push results/`.

## After a panic

- Full panic log lands in `results/panics/` (via idevicecrashreport).
- If the device boot-loops: hard-reset; panic log persists and is pulled next cycle.
- Check console output in `results/run-*.log` for the last fuzzer actions before death.

## Notes

- `fuzzer/ent.plist` asks for `get-task-allow` (lldb) and JIT entitlements (MAP_JIT experiments) — a free dev account cannot grant them, they are ignored harmlessly.
- MIG scan is exploratory: it needs the subsystem's bootstrap port name. Found names go into `t_mig.m:cand_names[]`.
- Edit `main.m` `FUZZ_MODE` env or call targets individually.
