# Candidate 002 — AGX / IOGPU resource lifetime mismatch

Status: active candidate, but requires conservative impact wording.

## Summary

Existing internal notes describe a resource lifetime mismatch where GPU work may continue to affect pages after a GPU resource has been destroyed and the resource teardown path has progressed. The already-recorded evidence frames this as stale GPU translation behavior and delayed invalidation, with observable downstream effects in system GPU clients under the recorded test conditions.

This package does not include exploit steps, bypass instructions, or new directions for converting the behavior into a stronger primitive.

## Affected component

- Platform: iOS
- Device used for observation: iPhone 15 Pro Max / A17 Pro
- Build noted by existing report draft: iOS 27.0 beta 4 / 24A5390f
- Component area: AGX / IOGPU resource teardown and GPU translation invalidation behavior

## Observed result

- Existing notes report write-after-destroy behavior against freed resource pages in internal runs.
- Existing journal notes correlate the behavior with system GPU-client restarts and user-visible GPU/UI disruptions during recorded campaigns.
- The report draft explicitly bounds the impact and does not claim a complete privilege-escalation chain.

## Existing evidence

Use the existing files and logs:

- `docs/report_draft.md` — Finding 2
- `docs/SPTM_research_journal_part19.md` — AGXUAT retirement / stale translation sections
- `results/run-uatrec-s4*` references from the journal
- sysdiagnose / kernel-log excerpts referenced by the journal

## Impact boundary

- Treat as a memory-safety / resource-lifetime issue candidate.
- Existing evidence supports stale translation / write-after-destroy behavior and system GPU-client impact.
- Do not claim controlled kernel-object corruption.
- Do not claim privilege escalation.
- Do not add new grooming, targeting, bypass, or exploitability research steps to the disclosure package.

## Minimal Apple-facing repro description

During internal GPU resource-lifetime testing, already-submitted GPU work continued to interact with pages after resource teardown progressed. Existing logs and notes correlate this behavior with stale GPU translation effects and GPU restarts in system components under the recorded conditions.

No standalone trigger sequence is included in this package. Apple triage should be given the existing report note, relevant run logs, and sysdiagnose/kernel-log excerpts.

## Recommended submission treatment

Submit as a separate candidate only if the evidence bundle is attached cleanly. Suggested conservative title:

`AGX / IOGPU resource lifetime mismatch permits stale GPU translation effects after resource teardown`

The write-up should lead with the observed lifetime mismatch and impact boundary, not with exploitability speculation.