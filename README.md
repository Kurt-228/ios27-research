# ios27-fuzz

Experimental **iOS 27 kernel / driver research harness** for **iPhone 15 Pro Max (A17 Pro)**, currently exercised on **iOS 27.0 beta 4**.

This repository is both a runnable on-device fuzzing harness and a research notebook. The current work is centered on reachable App-Sandbox attack surface, IOKit / IOSurface / GPU behavior, lifetime bugs, DART faults, panic reproduction and SPTM-adjacent memory-safety research.

> **Status:** private research code. Findings here are experimental until reproduced and controlled. This is not a one-click exploit and the README deliberately distinguishes confirmed behavior from hypotheses and unfinished exploitability work.

## Current research state

Current head: **v110**.

| Area | Current state |
|---|---|
| **AppleM2ScalerCSCDriver** | Extensive request-layout and geometry research. A repeatable DART write-fault kernel panic (`bug_type 210`) and driver hang / DoS points are documented. Several earlier memory-corruption hypotheses were later rejected by control experiments; the journal records those corrections. |
| **AGX / IOGPU resource lifetime** | GPU work can complete after the destination resource has been destroyed: the post-destroy write behavior is reproducible. Reclaim of the freed pages into tested userland allocations has **not** been demonstrated. MTLBuffer, raw GPU resource and IOSurface reclaim attempts remained clean through v110. A `bug_type 284` GPU/BIF page-fault event was also observed during race experiments. |
| **IOCoreSurfaceRoot / IOSurfaceRootUserClient** | Raw user-client basis works from the current sandbox (`sel13` init, `sel6` create, `sel2` lock, `sel3` unlock). The v109 campaign ran 197 mutation cases without a crash. v110 explored deeper `sel9` serialization and `sel27` attachment paths; the attempted frames were rejected, so their exact input formats still need reversing. |
| **VCPDRMServiceUserClient** | Retained as a research target, but the current App-Sandbox path is MACF-denied (`iokit-open-user-client VCPDRMUserClient`). It is not currently a live fuzz surface with the app's entitlements. |
| **MIG / SPRR / JITBox reconnaissance** | Exploratory bootstrap/MIG work remains in the harness. Relevant service names are visible, but the current app sandbox gates direct access to the SPRR/JITBox paths observed so far. |

The large `fuzzer/t_iosurface_scaler.m` file is historical by design: it contains sequential experimental phases, controls, reproductions and later follow-up campaigns, including work that expanded beyond the scaler itself into AGX/IOGPU and IOSurface internals.

## Harness entry points

The app is a small UIKit wrapper that launches fuzz workers in background pthreads. It reads `FUZZ_MODE` and defaults to `all`.

| `FUZZ_MODE` | Worker | Purpose |
|---|---|---|
| `vcpdrm` | `t_vcpdrm` | VCPDRM user-client reconnaissance / fuzzing |
| `scaler` | `t_iosurface_scaler` | Main research track: scaler, IOSurface, AGX/IOGPU and phase-specific experiments |
| `mig` | `t_migscan` | MIG / bootstrap service reconnaissance |
| `all` | all of the above | Default; starts all three workers |

Individual research phases inside the main track use additional `FUZZ_*` environment switches. Read the corresponding journal section and source before running an old phase: some experiments intentionally panic the device, and some earlier hypotheses were superseded by later controls.

## Repository layout

- `fuzzer/` — the signed iOS app, common helpers and target implementations.
- `fuzzer/main.m` — UIKit entry point and `FUZZ_MODE` dispatch.
- `fuzzer/t_iosurface_scaler.m` — the main accumulated experimental harness and most current GPU / IOSurface / scaler work.
- `relay/` — direct-clang build, signing, install, launch, log capture and panic collection tooling.
- `relay/iotrace.m` — tracing dylib built into the app bundle.
- `docs/` — research journals, static-analysis notes, experiments, controls and conclusions.
- `results/` — run logs and collected crash / panic artifacts.

## Requirements

- macOS with Xcode / iPhoneOS SDK available through `xcrun`.
- Xcode command-line tools.
- `libimobiledevice` tools (`idevice_id`, `idevicesyslog`, `idevicecrashreport`; `ideviceinstaller` is used as an install fallback).
- A paired iPhone with Developer Mode enabled.
- A valid iOS development signing identity and provisioning profile.

Install the host-side basics:

```sh
xcode-select --install
brew install libimobiledevice
```

The build script defaults to:

```text
sign/Development.mobileprovision
```

Override signing or device selection when needed:

```sh
PROVISIONING_PROFILE=/path/to/profile.mobileprovision \
CSC_NAME="Apple Development: ..." \
DEVICE_ID=<UDID> \
./relay/build.sh
```

`relay/build.sh` compiles directly with `clang` against the iPhoneOS SDK, builds `libiotrace.dylib`, signs the bundle and produces:

```text
build/fuzz27.app
```

There is intentionally no Xcode project in the normal build path.

## Run through the relay

```sh
python3 relay/relay.py 30
```

The numeric argument is the run duration in minutes. If omitted, the relay uses 30 minutes.

A relay cycle performs:

```text
git pull --rebase (only when the worktree is clean)
        ↓
build + sign
        ↓
install with devicectl (ideviceinstaller fallback)
        ↓
launch with console capture + filtered idevicesyslog
        ↓
watch for panic / watchdog / fatal events
        ↓
collect crash reports
        ↓
git add results/ → commit → push
```

**Important:** `relay.py` is intentionally stateful. At the end of a run it stages `results/`, creates a results commit and pushes it. Do not point it at a branch where that behavior would be surprising.

Useful relay overrides:

```sh
DEVICE_ID=<UDID>                  # choose a device explicitly
PROVISIONING_PROFILE=/path/...    # alternate provisioning profile
CSC_NAME="Apple Development: ..." # alternate signing identity
CRASHREPORT_TIMEOUT=300           # idevicecrashreport timeout in seconds
```

## Logs, panics and research notes

- Console output for each relay run is written to `results/run-*.log`.
- Pulled crash and panic reports are stored under `results/panics/`.
- The journals under `docs/` are the authoritative narrative for each experiment: setup, assumptions, observations, controls and revised conclusions.

Treat **newer controlled experiments as authoritative over older journal claims**. This matters in this repo: several promising early interpretations were explicitly withdrawn after better controls showed an artifact or a weaker primitive than first assumed.

When triaging a crash, preserve at minimum:

1. the exact phase / environment switches,
2. the run log immediately before failure,
3. the full panic or GPU event report,
4. whether the behavior reproduces after reboot,
5. a control run that removes the suspected trigger.

## Research scope

This harness is intended for authorized testing on owned research devices. Some phases deliberately exercise malformed driver inputs, resource-lifetime races and GPU/DART fault paths and can reboot the phone, hang a driver or invalidate the current run.

A panic, GPU restart or post-destroy write is **not automatically an exploitable vulnerability**. Exploitability is tracked separately from trigger reliability, address/value control and reclaim/cross-process reachability.
