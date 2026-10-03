# Evidence Inventory

This inventory names existing artifacts only. Paths are evidence locations,
not reproduction instructions.

| ID | Observation | Repository-resident evidence | External evidence referenced by existing notes | Handling |
| --- | --- | --- | --- | --- |
| 001 | IOGPU command-queue initialization kernel panic | `docs/panic_triage_0909.md`; the neutral summary in `incidents/001-iogpu-command-queue-panic/` | Three raw panic reports named in `docs/panic_triage_0909.md`; they are not copied into this repository | Attach the triage note and the three raw panic reports when available. |
| 002 | AGX resource-release timing observation | `docs/SPTM_research_journal_part19.md`; `results/run-uatrec1.log`; `results/run-uatrec-s3.log`; `results/run-oolrec1.log` through `results/run-oolrec6.log` | None | Keep as a bounded observation only. Later controls are part of the evidence and must accompany any review. |
| 003 | GPU VM residual service-page observation | `docs/SPTM_research_journal_part16.md`; `docs/SPTM_research_journal_part18.md`; `results/run-v90*.log`; `results/run-v91*.log`; `results/gpuvm-dump/page_10000098000.bin`; `results/gpuvm-dump/page_10000120000.bin` | None | Keep on hold pending vendor correlation of the historical condition. |
| S-001 | Previously submitted scaler/DART kernel panic | `docs/report_draft.md`; the historical panic/log references retained elsewhere in the repository | Apple case `OE1107312371417` | Do not include in a new submission. Use only the existing case thread for follow-up. |

## Evidence Integrity Notes

- The three raw panic reports for incident 001 are cited by an existing
  repository note but live outside this checkout. They have deliberately not
  been copied or modified by this package.
- Artifact names and locations are preserved as recorded. This package does
  not claim that a later operating-system build is affected.
- The evidence for incident 002 includes the later negative controls; they
  supersede the earlier, broader interpretation.
