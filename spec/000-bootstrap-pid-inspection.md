# Bootstrap PID Inspection Specification

## Problem

`autopsyctl` has started implementation work for PID-oriented inspection, but the
repository does not yet have a task-scoped specification artifact under
`spec/`. That breaks the expected workflow in `AGENTS.md` and leaves the
current implementation without a behavioral contract.

This specification defines the initial bootstrap behavior for PID inspection so
that follow-on design and implementation work can proceed with a reviewable
scope.

## Context

The repository intent is a read-only Linux inspection tool that emits
deterministic evidence for debugging, triage, and forensic review. The current
codebase already contains:

- a minimal collector entry point
- a `collect_proc_status(...)` interface
- a smoke test that exercises the binary

The first bounded increment should establish one concrete evidence path and one
stable success/failure contract before broader PID, unit, or bundle collection
is added.

## Requirements

### Functional Requirements

1. The project shall support a bootstrap PID inspection path that targets one
   Linux process identified by PID.
2. The bootstrap collector shall attempt to read process status information from
   `/proc/<pid>/status`.
3. On success, the collector shall return structured process status data
   containing at minimum the inspected PID.
4. The bootstrap path shall reject invalid input explicitly, including null
   output/context pointers and non-positive PIDs.
5. The collector shall distinguish:
   - successful collection
   - invalid caller input
   - target unavailable or unreadable process state
6. The bootstrap implementation shall remain read-only with respect to the
   inspected target.
7. Output derived from the bootstrap collector shall be deterministic for the
   same observed process state.

### Interface Requirements

1. A public interface for bootstrap status collection shall remain available in
   headers under `include/`.
2. The collector interface shall accept:
   - collection context
   - target PID
   - destination structure for collected status data
3. The collector shall initialize output state before populating fields so
   cleanup and partial-failure behavior remain auditable.

### Operational Requirements

1. The bootstrap path shall build with the repository's offline `make` flow.
2. The bootstrap path shall run on Linux using C17-compatible toolchains.
3. The bootstrap path shall not require network access, background services, or
   dynamically fetched dependencies.

## Non-Goals

The bootstrap PID inspection spec does not require:

- systemd unit inspection
- container inspection
- evidence bundle generation
- JSON output
- text report formatting beyond minimal smoke-test visibility
- socket, namespace, cgroup, map, or file-descriptor correlation
- derived findings beyond directly observed process status facts

## Edge Cases

- PID does not exist at collection time.
- PID exits between validation and `/proc` read.
- `/proc/<pid>/status` exists but is unreadable due to permissions.
- `/proc` is unavailable or mounted with unexpected restrictions.
- The collector receives a valid PID but incomplete status content.
- The caller reuses an output structure from a prior collection attempt.

## Acceptance Criteria

1. A build from a clean tree succeeds with `make`.
2. A local test path exists that verifies the binary runs successfully in the
   bootstrap configuration.
3. `collect_proc_status(...)` rejects invalid inputs deterministically.
4. Successful bootstrap collection records the inspected PID in the output
   structure.
5. Failures to inspect a target process are surfaced explicitly rather than
   silently treated as success.
6. New code follows repository control-flow rules:
   - phase-oriented labels for non-trivial functions
   - centralized cleanup where resource ownership exists
   - shallow, reviewable failure handling

## Open Questions

1. Which GitHub issue number should replace the placeholder `000` in this spec
   filename?
2. Should the bootstrap collector return a richer status code enum instead of a
   generic integer?
3. What minimum field set beyond PID is required for the first meaningful
   `/proc/<pid>/status` milestone?
4. Should the initial CLI expose `autopsyctl pid <pid>` now, or should the
   current binary remain an internal scaffold until design work is completed?
