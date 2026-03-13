# Bootstrap PID Inspection Design

## Design Summary

This design covers the smallest useful PID inspection increment: a read-only
collector for `/proc/<pid>/status` that produces a stable in-memory result and a
clear success/failure contract.

The recommended approach is:

- keep one small public interface for status collection
- keep parsing and `/proc` access in a dedicated collector module
- use explicit phase labels for validation, path construction, file read,
  parsing, and cleanup
- return a bounded status code that separates invalid caller input from target
  read failure
- expand collected fields incrementally after the bootstrap path is stable

## Scope

Included in this design:

- `collect_proc_status(...)` as the bootstrap collection entry point
- reading `/proc/<pid>/status`
- parsing the minimum viable status payload
- deterministic initialization of output state
- test coverage for success and failure paths

Excluded from this design:

- service or systemd target resolution
- JSON or text report formatting
- bundle generation
- cross-source correlation
- broader `/proc` collection such as fd, maps, cgroup, or namespace traversal

## Module Boundaries

### Public Interface

The public header in [include/poc.h](/home/opsman/project_git/autopsyctl/include/poc.h)
should expose:

- `struct collect_ctx`
- `struct proc_status`
- a result code type for collection outcomes
- `collect_proc_status(...)`

This keeps the initial API surface small and reviewable.

### Collector Implementation

The implementation in
[src/collect_proc_status.c](/home/opsman/project_git/autopsyctl/src/collect_proc_status.c)
should own:

- target validation
- `/proc/<pid>/status` path construction
- file open/read handling
- line-oriented parsing
- output population
- centralized cleanup

If parsing complexity grows, the next extraction should be a narrow internal
helper in the same translation unit or a dedicated private parser module.

## Data Flow

Recommended control flow for `collect_proc_status(...)`:

1. `validate_input:`
   Reject null pointers and non-positive PIDs.
2. `initialize_state:`
   Reset all output fields to sentinel values.
3. `build_path:`
   Construct `/proc/<pid>/status` into a fixed-size local buffer with explicit
   bounds checks.
4. `acquire_resources:`
   Open the status file read-only.
5. `read_inputs:`
   Read file contents into a bounded buffer.
6. `parse_status:`
   Scan relevant lines and populate `struct proc_status`.
7. `check_invariants:`
   Confirm the output still represents the requested PID.
8. `emit_results:`
   Mark success.
9. `cleanup:`
   Close owned resources and return the result code.

This preserves the repository's phase-oriented style without adding abstraction
layers too early.

## Interface Design

### Result Codes

The current `int` return value is too weak for the spec's stated distinction
between invalid input and target access failure. The recommended change is a
small enum such as:

```c
enum collect_result {
    COLLECT_OK = 0,
    COLLECT_INVALID = 1,
    COLLECT_UNAVAILABLE = 2,
    COLLECT_PARSE_ERROR = 3,
};
```

This remains simple, deterministic, and auditable.

### Output Structure

The bootstrap structure should keep only fields justified by
`/proc/<pid>/status`. Minimum required field:

- `pid`

Reasonable next fields if implementation expands within the same scope:

- `ppid`
- `uid`
- `gid`
- `state`
- `threads`

Do not add fields sourced from other files in this bootstrap design.

### Collection Context

`struct collect_ctx` can remain minimal for now. It should not accumulate hidden
ownership or implicit global state. If later extended, it should hold only
collection-scoped policy such as buffer limits, verbosity, or evidence-quality
recording.

## Failure Handling

Failure handling should be monotonic:

- invalid caller input returns `COLLECT_INVALID`
- open/read failures return `COLLECT_UNAVAILABLE`
- malformed but readable status content returns `COLLECT_PARSE_ERROR`

The collector should never report success with an uninitialized or stale output
structure. Output initialization must happen before any read attempt so the
cleanup path is deterministic even after partial progress.

## Parsing Strategy

Use a bounded line-oriented scan of `/proc/<pid>/status`.

Recommended parser properties:

- no heap allocation
- no locale-sensitive parsing
- explicit key matching against known prefixes
- ignore unknown lines
- preserve deterministic handling order

A simple approach is sufficient:

- read the file into one fixed-size buffer
- iterate over newline-delimited records
- parse only the keys currently represented in `struct proc_status`

This avoids premature parser frameworks and stays aligned with the repository's
auditability goals.

## Risks

- `/proc/<pid>/status` can change between reads if the target exits mid-collect.
- Permission errors may differ across systems and test environments.
- A too-small buffer could truncate status content and create false parse
  failures.
- Adding many fields too early will blur the bootstrap boundary and increase API
  churn.

## Alternatives

### Keep Returning Raw `int`

Rejected for the next implementation step because it weakens the behavioral
contract and makes tests less specific.

### Parse Directly in `main`

Rejected because it collapses orchestration and collection into one place and
reduces reviewability.

### Add a Generic `/proc` Reader Framework Now

Rejected as unnecessary complexity. The bootstrap scope does not justify shared
abstractions yet.

## Testing Implications

The current smoke test should remain, but the next test additions should cover:

- invalid input rejection
- success for a known-live PID such as the test process itself
- failure for a clearly invalid or absent PID

Preferred testing shape:

- keep shell-based smoke coverage for offline portability
- add a narrow C test binary or targeted shell harness for collector semantics

Tests should verify observable behavior, not internal phase labels.

## Assumptions

- Linux `/proc` is available.
- The bootstrap collector remains single-threaded.
- The current placeholder issue number `000` will be replaced later.
- The binary may remain a scaffold until CLI design is specified separately.

## Recommended Approach

1. Introduce an explicit result enum in the public header.
2. Implement bounded `/proc/<pid>/status` reading in
   [src/collect_proc_status.c](/home/opsman/project_git/autopsyctl/src/collect_proc_status.c).
3. Keep `struct proc_status` intentionally small.
4. Add focused tests for invalid input, success, and unavailable target cases.
5. Defer richer CLI and multi-source correlation until this collector contract
   is stable.
