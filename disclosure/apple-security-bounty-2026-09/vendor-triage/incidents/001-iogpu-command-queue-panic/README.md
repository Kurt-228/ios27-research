# Incident 001 - IOGPU Command-Queue Initialization Panic

Status: confirmed observed kernel panic; suitable for separate vendor triage.

## Observed Behavior

Three kernel panic reports from the same internal test path were triaged to
the IOGPU command-queue initialization failure path. The recorded crashes have
the same fault class and corresponding function location across different
kernel slides. The observed impact is a kernel panic / denial of service.

No memory-corruption, privilege-escalation, code-execution, or exploitability
claim is made by this summary.

## Affected Environment

- iPhone 15 Pro Max with A17 Pro.
- iOS 27.0 beta 4, build 24A5390f.
- The observed component area is IOGPUFamily command-queue initialization.

This package does not assert that other devices, operating-system builds, or
shipping releases are affected.

## Attached Evidence

- Repository-resident triage: `docs/panic_triage_0909.md`.
- Three raw panic reports cited by that triage note. They are retained outside
  the repository and should be attached to the vendor report as the primary
  crash evidence when available.

## Reproducibility Status

The existing record documents three matching panic events on 2026-09-09 from
the same test path. This package records that historical result only; it does
not add a new reproduction attempt or claim validation on a later build.

## Intentionally Omitted

This summary omits the triggering request details, source code, proof of
concept, payloads, bypasses, exploit logic, and further research directions.
