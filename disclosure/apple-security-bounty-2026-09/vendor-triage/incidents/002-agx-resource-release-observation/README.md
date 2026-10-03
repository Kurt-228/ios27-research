# Incident 002 - AGX Resource-Release Timing Observation

Status: confirmed limited observation; not an active security submission.

## Observed Behavior

Recorded controls observed queued GPU work affecting the test app's buffer
after GPU-side resource release had progressed. The later controls establish
an important boundary: while the test app retained its CPU mapping, the
underlying pages remained pinned by that app. When the mapping was released,
the recorded recipient and readback checks were clean, and the tested paths
showed scrubbed or zeroed data.

The repository therefore supports a resource-release timing observation only.
It does not establish modification of another process's data, reclaimed kernel
memory, or an externally observable security boundary violation.

## Affected Environment

- iPhone 15 Pro Max with A17 Pro.
- iOS 27.0 beta 4, build 24A5390f.
- Observed component area: AGX / IOGPU resource release and GPU translation
  retirement.

No claim is made about other devices, builds, or releases.

## Attached Evidence

- `docs/SPTM_research_journal_part19.md`.
- `results/run-uatrec1.log` and `results/run-uatrec-s3.log`.
- `results/run-oolrec1.log` through `results/run-oolrec6.log`, which contain
  the later controls that bound the observation.

## Reproducibility Status

The limited self-owned mapping behavior was recorded repeatedly. The later
controls did not establish cross-domain impact after the mapping was released.
For that reason, this item is retained for factual vendor context rather than
as a standalone security finding.

## Intentionally Omitted

This summary omits request sequencing, source code, payloads, targeting,
grooming, proof-of-concept material, exploit analysis, and future research
directions.
