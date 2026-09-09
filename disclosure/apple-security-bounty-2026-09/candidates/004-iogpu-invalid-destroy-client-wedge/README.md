# Candidate 004 — IOGPU invalid destroy accepted, subsequent client wedge

Status: candidate / lower confidence. Keep wording conservative.

## Summary

Recent commit notes and run references indicate that invalid destroy-style operations were accepted with a success return in an IOGPU user-client path, followed by a wedge of subsequent operations on the same connection. This is grouped as a lifecycle-validation / hang-DoS candidate.

Do not label this as confirmed UAF in Apple-facing text unless existing evidence is later added that directly proves use-after-free behavior. Current package wording intentionally avoids that claim.

## Affected component

- Platform: iOS
- Device used for observation: iPhone 15 Pro Max / A17 Pro
- Build context: same iOS 27 beta research series as the surrounding IOGPU notes
- Component area: IOGPU user-client object lifecycle / destroy handling

## Observed result

- Invalid destroy input was accepted with a success return in existing internal runs.
- Subsequent operations on the same connection wedged in a kernel call.
- Device remained alive in the noted run; this is not currently framed as a kernel panic.

## Existing evidence

Use only existing notes/logs:

- `docs/SPTM_research_journal_part19.md` — v143/v144 sections
- `results/run-iogpusweep-full.log`
- `results/run-iogpusweep-2.log`
- `results/run-destroyuaf1.log`

## Deduplication

This candidate deduplicates:

- v143 destroy-selector garbage-id acceptance observations;
- v144 invalid-destroy accepted plus client wedge observation;
- related wording that previously suggested a possible UAF class.

For triage, keep it as one candidate unless a separate already-existing panic/crash artifact proves an independent issue.

## Impact boundary

- Treat as hang-DoS / client wedge candidate.
- Do not claim kernel memory corruption.
- Do not claim confirmed UAF.
- Do not claim system-wide denial of service unless supported by additional already-existing evidence.
- Do not add new object-lifetime experiments or triggering sequences to this package.

## Minimal Apple-facing repro description

During internal IOGPU lifecycle testing, an invalid destroy-style operation was accepted as successful. After that, later operations on the same connection stopped making progress and the client became wedged in a kernel call. Existing logs should be provided as evidence; this package does not include standalone triggering instructions.

## Recommended submission treatment

Possible conservative title:

`IOGPU user-client invalid destroy handling can wedge subsequent client operations`

This should be treated as lower priority than candidate 001 unless the existing evidence bundle shows broader impact.