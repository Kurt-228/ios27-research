# Excluded, duplicate, and deferred items

This file keeps the active Apple triage package clean. Items listed here are not active candidates for a new submission in this package.

## EX-001 — Already submitted Apple report

The scaler / DART write-fault kernel panic issue is excluded from this active package because it was already submitted separately to Apple.

Known submission record:

- Submitted: 2026-08-28 at 22:03
- Apple case: `OE1107312371417`
- Status known from prior tracking: initial review

Related repository note:

- `docs/report_draft.md` — Finding 1: AppleM2ScalerCSCDriver / DART write-fault kernel panic

Handling decision:

- Do not resubmit as a new report.
- If Apple requests follow-up, use the original case thread rather than mixing it into this new package.
- Keep any additional evidence under the original case context only.

## EX-002 — Compound command-buffer chain mutation path

Related repository note:

- `docs/SPTM_research_journal_part19.md` — v142 / cbchain

Handling decision:

- Excluded as a negative result.
- Existing notes describe clean behavior, silent drops, invalid-resource handling, or expected queue behavior.
- Do not submit as a vulnerability.

## EX-003 — Shared-event wait greater than signal hang

Related repository note:

- `docs/SPTM_research_journal_part19.md` — v142 / cbchain event case

Handling decision:

- Excluded as expected behavior.
- Do not submit as a vulnerability.

## EX-004 — Replay2 / stream reconstruction blockers

Related repository notes:

- `docs/device_stream_builder.md`
- `docs/SPTM_research_journal_part19.md` — replay2 / stream page hunt / dsrecon notes

Handling decision:

- Excluded from Apple disclosure package.
- This is research state, not a confirmed vulnerability candidate.
- Do not include speculative firmware-stream reconstruction, bypass ideas, or future research steps in Apple-facing triage material.

## EX-005 — IOGPU selector 49 worker hang observation

Related repository notes:

- `docs/SPTM_research_journal_part19.md` — v143 / iogpusweep
- `results/run-iogpusweep-full.log`
- `results/run-iogpusweep-2.log`

Handling decision:

- Deferred / low-signal item.
- Do not submit alone unless stronger already-existing evidence shows independent impact.
- Do not merge into candidate 004 unless the evidence clearly shares root cause with lifecycle/destroy validation behavior.

## General exclusion rule

An item should remain excluded unless it has all of the following:

- existing crash/panic/sysdiagnose/log evidence;
- a bounded affected component;
- a reproducible observed outcome;
- clear deduplication against already submitted material;
- neutral wording that does not depend on speculative exploitability.
## EX-006 — Candidate 004 retracted (IOGPU invalid destroy / client wedge)

Retracted on 2026-09-09 after controlled re-testing. Do not submit.

Root-cause of the original observation (was: v143/v144 destroy notes):

- The "accepted garbage id 0x1" was a destroy of our own LIVE object — the
  garbage-id list collided with the real id space (ids start at 1).
  Genuinely invalid ids (0, +1000, 0xffffffff) are rejected with
  kIOReturnBadArgument.
- The "client wedge in a kernel call" was a self-inflicted SIGSEGV in the
  test harness: after a successful destroy the kernel tears down the
  resource's CPU mapping, and the harness's marker scan read that freed
  mapping. No kernel-side wedge existed.

Controlled confirmation (run-destroyuaf3.log, 53 cases, 4 object classes):

- destroy validation is strict: live id OK, dead/never-existed/foreign-class
  id rejected; use-after-destroy attempts all rejected; no neighbor
  corruption; no panic.

Related repository notes:

- `docs/SPTM_research_journal_part19.md` — sections 131 (retraction),
  131-addendum (sweep-log re-check), 132 (full controlled run)
- `results/run-destroyuaf3.log`
