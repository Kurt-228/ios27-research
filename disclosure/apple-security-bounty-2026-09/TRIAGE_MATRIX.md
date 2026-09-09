# Triage matrix

This matrix is intentionally conservative. It separates active disclosure candidates from duplicates, expected behavior, previously submitted material, and research-only notes.

| ID | Candidate | Status | Confidence | Disclosure handling | Evidence class | Notes |
|---|---|---:|---:|---|---|---|
| 001 | IOGPUCommandQueue init error-path NULL dereference | Active candidate | High | Prepare as separate Apple triage submission | Three deterministic kernel panic logs, symbolized panic triage, report draft note | Kernel DoS only. No corruption claim. No standalone repro code in this package. |
| 002 | AGX / IOGPU resource lifetime mismatch with stale GPU translation effects | Active candidate | Medium-High | Prepare as separate candidate with strict impact boundary | Internal run logs, sysdiagnose/kernel-log correlations, report draft note | Memory-safety/lifetime issue candidate. Do not claim privilege escalation. |
| 003 | GPU VM residual service-page metadata exposure after neighboring GPU process crash | Active candidate | Medium | Prepare as separate candidate or attach as supporting infoleak issue | GPU dump notes, report draft note | KASLR-relevant metadata exposure. Boundary: requires crash/termination of a neighboring GPU process. |
| 004 | IOGPU invalid destroy accepted, subsequent client-side kernel-call wedge | Candidate / needs conservative wording | Medium-Low | Keep as separate low-severity hang-DoS candidate or defer | Commit notes and internal run log references | Deduplicates destroy-selector garbage-id acceptance from v143/v144. Not enough evidence here to call it a confirmed UAF. |
| EX-001 | AppleM2ScalerCSCDriver DART write-fault kernel panic | Excluded from active package | High | Already submitted separately | Existing report draft and prior submission record | Previously submitted to Apple on 2026-08-28 as case `OE1107312371417`; do not duplicate in a new submission. |
| EX-002 | Compound command-buffer mutation path | Excluded | High | Do not submit | Negative run notes | Closed as clean / expected behavior. |
| EX-003 | Shared-event wait greater than signal hang | Excluded | High | Do not submit | cbchain notes | Treated as expected queue behavior, not a vulnerability. |
| EX-004 | Replay2 raw-stream reconstruction blockers | Excluded | High | Do not submit | R&D notes | Research-only. No confirmed issue by itself. |
| EX-005 | Selector 49 worker hang during sweep | Deferred / duplicate bucket | Low-Medium | Do not submit alone yet | Sweep notes | Keep as low-signal hang observation unless further already-existing evidence proves independent impact. |

## Deduplication decisions

- v143 destroy-selector garbage-id acceptance and v144 invalid-destroy wedge are grouped under candidate 004.
- Command-buffer chain negative cases are grouped under exclusions rather than separate reports.
- Replay2 / stream reconstruction work is kept out of disclosure because it is an R&D track, not a confirmed vulnerability candidate.
- The previously submitted scaler/DART panic is explicitly excluded from the active candidate list to avoid duplicate submission noise.

## Apple-facing style rule

For every active candidate, use neutral wording:

- say "observed", "reproduced", "correlated", or "consistent with" where appropriate;
- avoid final exploitability claims unless directly supported by existing evidence;
- include impact boundaries early;
- attach logs and triage notes rather than adding new triggering instructions.