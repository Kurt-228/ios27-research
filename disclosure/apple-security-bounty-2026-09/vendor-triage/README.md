# Vendor Triage Package

Status: neutral incident summaries prepared from the existing repository
artifacts. This package records observations only; it does not make
exploitability, reward, privilege-escalation, arbitrary-read, or
arbitrary-write claims.

## Scope

The package contains three incident summaries, one already-submitted case,
and an evidence/status index. Each summary separates what was observed from
what was not established by the recorded evidence.

The following material is intentionally outside this handoff package:

- source code, proof-of-concept material, payloads, and triggering sequences;
- reverse-engineering notes and prospective research directions;
- exploitability analysis beyond the observed impact boundary;
- artifacts retracted by later controls.

## Package layout

- `EVIDENCE_INVENTORY.md` lists the existing source artifacts and whether they
  are repository-resident or externally referenced by an existing note.
- `STATUS_AND_DEDUPLICATION.md` groups observations, duplicates, and
  retractions into one handling decision per item.
- `incidents/` contains one neutral README for each selected observation.
- `already-submitted/` separates the prior Apple case so it is not submitted
  again with this package.

## Attachment policy

Attach only the evidence enumerated for the specific incident under review.
Do not attach this repository wholesale, internal research journals beyond
the cited excerpts, or any source/reproduction material.
