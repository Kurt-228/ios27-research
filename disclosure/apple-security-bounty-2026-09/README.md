# Apple Security Bounty triage package — 2026-09

Status: defensive disclosure preparation package.

This directory is a neutral triage layer over the existing research repository. It does not add weaponized proof-of-concept code, bypass instructions, exploitation steps, or new research directions. It only indexes issue candidates already supported by existing crash/panic logs, sysdiagnose excerpts, commit notes, and internal report notes.

## Scope

Included here:

- confirmed or candidate issues with existing evidence in the repository;
- deduplicated summaries written for Apple triage;
- evidence pointers to existing repository artifacts;
- impact boundaries and confidence notes;
- exclusions for already submitted or low-signal items.

Not included here:

- standalone repro code;
- step-by-step triggering instructions;
- exploit chains;
- bypass workflows;
- speculative next-step research plans;
- raw exploitability claims that are not supported by the existing evidence.

## Package layout

- `TRIAGE_MATRIX.md` — candidate status, confidence, evidence, and handling decision.
- `EVIDENCE_INDEX.md` — existing evidence paths only, with duplicate grouping.
- `candidates/001-iogpu-commandqueue-init-null-deref/README.md` — active candidate, deterministic kernel panic.
- `candidates/002-agx-resource-lifetime-write-after-destroy/README.md` — active candidate, resource lifetime mismatch / stale GPU translation behavior.
- `candidates/003-gpuvm-residual-service-page-infoleak/README.md` — active candidate, residual GPU service metadata exposure.
- `candidates/004-iogpu-invalid-destroy-client-wedge/README.md` — candidate / lower-confidence hang-DoS class, deduped from related destroy-selector notes.
- `excluded/README.md` — already submitted report, expected behavior, low-signal items, and R&D-only tracks.

## Submission handling

The previously submitted Apple report from 2026-08-28, case `OE1107312371417`, is treated as already submitted and excluded from the active candidate set in this package.

Finding labels in this directory are triage labels, not final vulnerability names. Apple-facing wording should stay conservative and evidence-bound.