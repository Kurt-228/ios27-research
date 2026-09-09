# Candidate 003 — GPU VM residual service-page metadata exposure

Status: active candidate, medium confidence.

## Summary

Existing notes describe residual GPU service metadata becoming observable after crash/termination of another GPU process. The report draft frames the material as KASLR-relevant metadata rather than direct code execution or a standalone privilege-escalation primitive.

This package does not include steps for inducing neighboring-process crashes or expanding the observation into exploitation.

## Affected component

- Platform: iOS
- Device used for observation: iPhone 15 Pro Max / A17 Pro
- Build noted by existing report draft: iOS 27.0 beta 4 / 24A5390f
- Component area: GPU VM / GPU service-page allocation and cleanup behavior

## Observed result

- Residual GPU service-layer data was observed in existing dump notes.
- Existing notes describe CPU-pointer-like metadata / KASLR-relevant material.
- The condition is bounded: observation depends on a neighboring GPU process crash/termination condition described in the existing notes.

## Existing evidence

Use the existing evidence paths:

- `docs/report_draft.md` — Finding 3
- `results/gpuvm-dump/` references from report notes
- related journal sections referenced by the report draft

## Impact boundary

- Treat as information disclosure / residual metadata exposure.
- Do not claim arbitrary read.
- Do not claim code execution.
- Do not claim privilege escalation.
- State clearly that the observed condition depends on a neighboring GPU-process crash/termination scenario.

## Minimal Apple-facing repro description

Existing internal observations indicate that after a neighboring GPU process crash/termination condition, residual GPU service metadata became observable in GPU VM related dumps. The material is described as KASLR-relevant metadata. The package provides only existing dump references and notes, not instructions for inducing the condition.

## Recommended submission treatment

Submit separately only if the evidence dump can be attached with enough context for Apple to confirm the residual-data boundary. Suggested conservative title:

`GPU VM residual service metadata exposure after GPU process crash on iOS 27 beta`

Keep the write-up focused on cleanup/scrubbing behavior and observed metadata exposure.