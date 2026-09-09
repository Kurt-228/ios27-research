# Evidence index

This file lists existing evidence paths only. It does not add new repro instructions or new research tasks.

## Candidate 001 — IOGPUCommandQueue init error-path NULL dereference

Primary evidence:

- `docs/panic_triage_0909.md`
- `docs/report_draft.md` — Finding 4 section
- panic logs referenced by the triage note:
  - `/tmp/fzc-today/panic-full-2026-09-09-124459.0002.ips`
  - `/tmp/fzc-today/panic-full-2026-09-09-135915.0002.ips`
  - `/tmp/fzc-today/panic-full-2026-09-09-140637.0002.ips`

Evidence summary:

- three deterministic kernel panic events from the same internal fuzzing path;
- matching kernelcache UUID noted in triage;
- same fault address class and same function offset across different slides;
- impact bounded to kernel panic / DoS.

## Candidate 002 — AGX / IOGPU resource lifetime mismatch

Primary evidence:

- `docs/report_draft.md` — Finding 2 section
- `docs/SPTM_research_journal_part19.md` — AGXUAT retirement / stale translation notes
- `results/run-uatrec-s4*` references from the journal
- sysdiagnose / kernel-log excerpts referenced by the journal

Evidence summary:

- resource lifetime mismatch observed through already-recorded internal runs;
- stale GPU translation behavior described in the journal;
- cross-client system GPU restart correlations noted in existing logs;
- no direct privilege-escalation claim included in this package.

## Candidate 003 — GPU VM residual service-page metadata exposure

Primary evidence:

- `docs/report_draft.md` — Finding 3 section
- `results/gpuvm-dump/` references from report notes
- relevant journal sections referenced by the report draft

Evidence summary:

- residual GPU service metadata observed after crash/termination of another GPU process;
- material described as KASLR-relevant metadata rather than direct code execution;
- boundary kept explicit: neighboring GPU-process crash is part of the observed condition.

## Candidate 004 — RETRACTED (IOGPU invalid destroy accepted, client wedge)

Retracted on 2026-09-09 (see excluded/README.md EX-006). The "accepted
invalid destroy" was a live-id collision in the test harness, and the
"client wedge" was a harness self-SIGSEGV on a freed CPU mapping, not
kernel behavior. Controlled 53-case re-test confirms strict destroy
validation. Do not submit.

Retraction evidence:

- `docs/SPTM_research_journal_part19.md` — sections 131, 131-addendum, 132
- `results/run-destroyuaf3.log`

## Excluded / duplicate / low-signal evidence

- `docs/report_draft.md` — Finding 1: already-submitted scaler/DART kernel panic report, excluded from the active package.
- `docs/SPTM_research_journal_part19.md` — cbchain v142: negative result / expected queue behavior.
- `docs/SPTM_research_journal_part19.md` — replay2 / stream reconstruction notes: research-only, no standalone confirmed issue.
- IOGPU selector 49 worker hang from v143: keep as low-signal observation unless grouped later with stronger already-existing evidence.