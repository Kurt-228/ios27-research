# Candidate 001 — IOGPUCommandQueue init error-path NULL dereference

Status: active candidate for Apple triage.

## Summary

A deterministic kernel panic was observed during an internal IOGPU queue-creation fuzzing run on iOS 27.0 beta 4. Existing panic triage attributes the crash to an error-path NULL dereference in `IOGPUCommandQueue::init` when a diagnostic/logging path reads a member that is only initialized on the successful path.

This package intentionally does not include standalone repro code or step-by-step triggering instructions.

## Affected component

- Platform: iOS
- Device used for observation: iPhone 15 Pro Max / A17 Pro
- Build noted by existing report draft: iOS 27.0 beta 4 / 24A5390f
- Component: IOGPUFamily / IOGPU command queue initialization path

## Observed result

- Kernel panic / kernel data abort.
- Three observed panic events in the same triage set.
- Same fault location class and same function offset across different kernel slides, according to the existing triage note.
- The panicked task is the internal fuzzing app/process.

## Existing evidence

Use the existing files, not new triggering material:

- `docs/panic_triage_0909.md`
- `docs/report_draft.md` — Finding 4
- panic logs referenced by `docs/panic_triage_0909.md`:
  - `panic-full-2026-09-09-124459.0002.ips`
  - `panic-full-2026-09-09-135915.0002.ips`
  - `panic-full-2026-09-09-140637.0002.ips`

## Impact boundary

- Treat as kernel DoS.
- Do not claim code execution.
- Do not claim privilege escalation.
- Do not claim memory corruption beyond the observed NULL dereference.
- The root cause is currently framed as failure-path initialization / diagnostic logging misuse.

## Minimal Apple-facing repro description

During internal fuzzing of the IOGPU command queue creation interface, a request with an unsupported queue-version shape was rejected through an error path. Instead of returning a recoverable error, the kernel entered a diagnostic path that dereferenced an uninitialized member and panicked. The same crash was observed three times and triaged against the matching kernelcache.

No standalone trigger is included in this package. Existing panic logs and the symbolized triage note are the intended evidence.

## Recommended submission treatment

Submit separately from the previously submitted 2026-08-28 report. Keep the title conservative, for example:

`IOGPUCommandQueue::init error-path NULL dereference causes kernel panic on iOS 27 beta`

Attach the panic triage note and the three panic logs. Keep the write-up focused on deterministic DoS and failure-path root cause.