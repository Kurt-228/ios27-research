# Incident 003 - GPU VM Residual Service-Page Observation

Status: historical observation; retained for vendor correlation, not a
submission-ready security assertion.

## Observed Behavior

Existing GPU VM scan artifacts recorded nonzero structured content in service
pages in a process following a prior crash condition. Other recorded scans,
including CPU-visible allocation controls, observed zeroed data. The preserved
evidence does not independently repeat the original GPU-only condition with a
clean attribution of the data source.

This summary therefore records residual-content observation only. It does not
claim exposure of another process's data, arbitrary read, KASLR bypass,
privilege escalation, or code execution.

## Affected Environment

- iPhone 15 Pro Max with A17 Pro.
- iOS 27.0 beta 4, build 24A5390f.
- Observed component area: GPU VM service-page allocation and cleanup.

This package does not assert that other devices, operating-system builds, or
shipping releases are affected.

## Attached Evidence

- `docs/SPTM_research_journal_part16.md`.
- `docs/SPTM_research_journal_part18.md`.
- `results/run-v90*.log` and `results/run-v91*.log`.
- `results/gpuvm-dump/page_10000098000.bin` and
  `results/gpuvm-dump/page_10000120000.bin`.

## Reproducibility Status

The nonzero-page observation is preserved in the historical record. Later
controls establish zeroed CPU-visible allocations, but they are not a repeat
of the same GPU-only observation. The item remains on hold pending vendor
correlation of the recorded artifacts; no stronger reproducibility claim is
made here.

## Intentionally Omitted

This summary omits scan mechanics, source code, proof-of-concept material,
payloads, targeting details, exploit logic, and future research directions.
