# Status And Deduplication

| Group | Current handling | Reason |
| --- | --- | --- |
| 001 - IOGPU command-queue initialization panic | Separate vendor triage item | Three matching kernel panic records are documented. The observed impact is limited to kernel panic / denial of service. |
| 002 - AGX resource-release timing | Observed behavior, not an active security submission | Later recorded controls show the observed write remained in a buffer whose CPU mapping was still held by the test app; after that mapping was released, the tested recipient and readback paths were clean. The repository does not establish cross-process or reclaimed-page impact. |
| 003 - GPU VM residual service-page content | Historical observation, on hold | Existing artifacts record nonzero service-page content after a prior crash condition, but the repository does not contain an independent repeat of the same GPU-only condition with a clean source attribution. |
| S-001 - scaler/DART kernel panic | Already submitted; keep separate | Existing Apple case `OE1107312371417` covers this issue. Variants and later panic artifacts from the same scaler path must not become a second report. |
| Former IOGPU invalid-destroy item | Retracted | Existing controlled re-test attributes the apparent acceptance to an identifier collision and the apparent wedge to a harness-side fault. |
| Former scaler cross-request-write observation | Retracted / grouped with S-001 history | Existing control evidence attributes the apparent cross-request corruption to marker loss during a legitimate operation. It is not an independent issue. |
| IOGPU selector-49 worker hang | Deferred | The repository contains a single low-signal hang observation without a bounded affected state or independently established impact. |

## Duplicate Rule

One vendor report should cover one distinct, evidenced observed outcome. Do not
group the current IOGPU panic with the previously submitted scaler case, and
do not promote retracted or deferred entries by association with either one.

## Vendor-Facing Boundary

The package is deliberately neutral about severity, exploitability, and bounty
eligibility. Those determinations require vendor validation of the attached
artifacts.
