````md
# ARCHITECTURE.md
**Project:** autopsyctl  
**Language:** C17  
**Platform:** Linux  
**Purpose:** Read-only process, service, and host triage with human-auditable implementation

---

# 1. Purpose

This document defines the intended architecture of `autopsyctl`.

The architecture has two primary goals:

1. Produce reliable, deterministic evidence snapshots for Linux troubleshooting, incident triage, debugging, and forensic review.
2. Keep the implementation understandable to human reviewers by enforcing clear module boundaries, explicit data flow, and phase-oriented control flow.

This document is a base architecture specification, not a frozen implementation contract.

---

# 2. Architectural Goals

## 2.1 Functional Goals

`autopsyctl` must be able to:

- inspect a running process by PID
- inspect a systemd unit by service name
- collect structured evidence from Linux runtime interfaces
- correlate process, file descriptor, socket, namespace, cgroup, and executable state
- emit human-readable and machine-readable reports
- preserve evidence in a deterministic on-disk bundle
- compare two snapshots in a later phase

---

## 2.2 Structural Goals

The architecture must:

- separate collection from parsing
- separate parsing from correlation
- separate correlation from emission
- keep optional integrations isolated
- support partial evidence without collapsing the entire run
- keep high-level execution linear and easy to reason about

---

## 2.3 Reviewability Goals

A reviewer should be able to answer:

- what each module owns
- what enters and leaves each stage
- what failures are fatal
- what evidence is optional
- what data is raw versus normalized versus derived

If those answers are not clear, the architecture is too muddy.

---

# 3. System Model

At a high level, `autopsyctl` follows a staged pipeline:

```text
CLI
  -> target resolution
  -> collection
  -> parsing
  -> normalization
  -> correlation
  -> analysis
  -> emission
  -> bundle writing
````

Each stage has a distinct job.

The pipeline is intentionally explicit. This reduces hidden coupling and makes failures easier to localize.

---

# 4. Top-Level Execution Flow

## 4.1 Modes

The tool is expected to support these execution modes:

* `pid`
* `unit`
* `compare` later
* `container` later or optional

The initial implementation should focus on `pid` and `unit`.

---

## 4.2 Primary Flow

A typical `pid` run looks like this:

```text
parse CLI
  -> build run config
  -> resolve target
  -> initialize snapshot context
  -> collect raw evidence
  -> parse evidence
  -> normalize evidence
  -> correlate related objects
  -> generate findings
  -> emit text/json/bundle outputs
  -> finalize and clean up
```

A typical `unit` run looks like this:

```text
parse CLI
  -> build run config
  -> resolve unit metadata
  -> resolve associated PID or process tree
  -> initialize snapshot context
  -> collect service-aware evidence
  -> parse, normalize, correlate
  -> generate findings
  -> emit outputs
  -> finalize and clean up
```

---

# 5. Architectural Layers

`autopsyctl` is organized into layers. Each layer must have a narrow purpose.

## 5.1 CLI Layer

Responsibilities:

* parse command-line arguments
* validate top-level mode and flags
* construct `run_config`
* select output modes
* select target mode

Must not:

* read `/proc`
* parse evidence files
* perform correlation
* format detailed reports directly

---

## 5.2 Target Resolution Layer

Responsibilities:

* resolve PID targets
* resolve systemd unit to service metadata and process targets
* later resolve container identifiers

Outputs:

* a canonical target descriptor

Must not:

* collect broad evidence
* emit reports
* perform heuristic analysis

---

## 5.3 Collection Layer

Responsibilities:

* read raw data from Linux interfaces
* capture evidence from `/proc`, `/sys`, journal, and related sources
* preserve raw or semi-raw artifacts
* record partial failures

Outputs:

* raw evidence objects
* optional raw text artifacts
* collection status metadata

Must not:

* make high-level conclusions
* print final reports
* bury failures silently

---

## 5.4 Parsing Layer

Responsibilities:

* parse raw text or kernel-facing data into typed structures
* validate field boundaries
* preserve raw source where useful
* tolerate partial or unstable live-system input

Outputs:

* typed parsed objects

Must not:

* infer suspiciousness
* merge unrelated domains
* emit final user-facing text

---

## 5.5 Normalization Layer

Responsibilities:

* transform parsed data into internal canonical forms
* normalize path, timestamp, inode, and identifier representation
* sort collections into deterministic order
* resolve representation inconsistencies

Outputs:

* normalized evidence tables

Must not:

* perform domain-wide reasoning beyond canonicalization
* emit text for users

---

## 5.6 Correlation Layer

Responsibilities:

* connect related evidence across domains

Examples:

* fd entry ↔ socket inode
* PID ↔ namespace set
* PID ↔ cgroup membership
* service ↔ process tree
* mapped file ↔ deleted state
* executable ↔ hash/build ID

Outputs:

* correlated views
* enriched evidence graph

Must not:

* overstate confidence
* generate narrative language directly

---

## 5.7 Analysis Layer

Responsibilities:

* compute findings and evidence-quality signals
* derive operational or forensic observations
* assign severity and confidence
* distinguish observed facts from heuristics

Examples:

* deleted executable still mapped
* fd usage near resource limit
* crash-loop hints from service state
* journal access unavailable
* socket mapping partial

Outputs:

* findings list
* evidence caveats
* confidence markers

Must not:

* mutate raw evidence
* invent facts unsupported by data

---

## 5.8 Emission Layer

Responsibilities:

* convert internal snapshot state into:

  * text report
  * JSON
  * bundle directory contents

Must not:

* recollect evidence
* perform heavy correlation
* reinterpret raw data differently than the analysis layer already defined

---

# 6. Core Data Flow

The main data flow should look like this:

```text
run_config
  -> target_descriptor
  -> collection_result
  -> parsed_snapshot
  -> normalized_snapshot
  -> correlated_snapshot
  -> analyzed_snapshot
  -> emitted_outputs
```

Each stage should produce an explicit artifact. Avoid hidden mutation across unrelated modules.

---

# 7. Primary Runtime Objects

The names here are placeholders and may change, but the object model should remain roughly stable.

## 7.1 `run_config`

Holds top-level execution settings.

Expected fields:

* execution mode
* target mode
* output mode flags
* output directory
* verbosity
* timeout
* evidence limits if any
* deterministic mode options

Purpose:

* immutable configuration for the run

---

## 7.2 `target_descriptor`

Canonical description of what is being inspected.

Possible fields:

* target kind: pid, unit, container
* primary PID
* service name
* process tree roots
* cgroup hints
* namespace hints

Purpose:

* unify target resolution so later modules do not care how the target was selected

---

## 7.3 `snapshot_context`

Shared execution context for one run.

Possible fields:

* `run_config *`
* error recorder
* output paths
* feature availability flags
* timestamps for collection start/end
* collection metadata
* allocator-related handles if needed

Purpose:

* central run-scoped operational context

This is not permission to create a god object. Keep it tight.

---

## 7.4 `collection_result`

Raw or near-raw evidence gathered from the host.

Possible contents:

* proc files captured
* fd listings
* maps content
* namespace link targets
* cgroup files
* socket table raw content
* journal excerpts
* executable file metadata
* collection warnings

Purpose:

* preserve what was actually collected before interpretation

---

## 7.5 `parsed_snapshot`

Typed representations of raw evidence.

Possible contents:

* parsed proc status
* parsed maps rows
* parsed fd entries
* parsed socket rows
* parsed namespace identities
* parsed cgroup entries

Purpose:

* structured internal form for later processing

---

## 7.6 `normalized_snapshot`

Canonicalized structured state.

Possible contents:

* sorted vectors
* normalized identifiers
* canonical timestamps
* stable path representation
* normalized permission/capability forms

Purpose:

* remove format noise before correlation

---

## 7.7 `correlated_snapshot`

Enriched relationships across evidence domains.

Possible contents:

* socket ownership mapping
* deleted-file references in mappings
* service/process relationships
* executable metadata attached to process identity
* namespace and cgroup summary attached to process record

Purpose:

* create coherent views for analysis and reporting

---

## 7.8 `analyzed_snapshot`

Final internal result before output.

Possible contents:

* findings
* caveats
* confidence indicators
* summary stats
* human-report sections
* JSON-ready top-level object graph

Purpose:

* emission should be mostly rendering at this point

---

# 8. Module Layout

A sane repository/module layout should look like this:

```text
autopsyctl/
  include/
    autopsyctl/

  src/
    main.c

    cli/
    target/
    collect/
    parse/
    normalize/
    correlate/
    analyze/
    emit/
    bundle/
    util/
    style/

  tests/
  fixtures/
  docs/
```

Below is the intended responsibility of each area.

---

# 9. Module Responsibilities

## 9.1 `src/main.c`

Responsibilities:

* program entry point
* initialize top-level context
* invoke CLI parsing
* dispatch mode runner
* return process exit code

Must remain small.

`main.c` should not become a second orchestrator full of Linux logic.

---

## 9.2 `src/cli/`

Responsibilities:

* argument parsing
* usage/help rendering
* mode and flag validation
* construction of `run_config`

Possible files:

```text
cli_parse.c
cli_usage.c
cli_validate.c
```

Outputs:

* `run_config`

Must not know about detailed `/proc` internals.

---

## 9.3 `src/target/`

Responsibilities:

* resolve a target into a canonical descriptor

Subdomains:

* PID target resolution
* systemd unit resolution
* later container resolution

Possible files:

```text
target_pid.c
target_unit.c
target_common.c
```

Outputs:

* `target_descriptor`

Notes:

* systemd integration should be isolated here or in a nearby dedicated module
* if systemd is unavailable, the failure should be explicit and bounded

---

## 9.4 `src/collect/`

Responsibilities:

* raw evidence acquisition from Linux interfaces

Subdomains may include:

```text
collect_proc.c
collect_fd.c
collect_maps.c
collect_ns.c
collect_cgroup.c
collect_socket_tables.c
collect_exe.c
collect_journal.c
```

Outputs:

* raw evidence structs
* collection warnings/errors

Rules:

* keep collection separate from parsing where practical
* when full separation is too expensive, keep boundaries clear inside the module

---

## 9.5 `src/parse/`

Responsibilities:

* parse raw text and semi-structured kernel-facing sources

Possible files:

```text
parse_proc_status.c
parse_proc_maps.c
parse_fd_targets.c
parse_socket_table.c
parse_cgroup.c
parse_journal.c
```

Outputs:

* typed parsed objects

Rules:

* parsers must be testable from fixtures
* parsers should not require live `/proc` reads directly when fixture-based testing is possible

---

## 9.6 `src/normalize/`

Responsibilities:

* stable ordering
* canonical field conversion
* cleanup of representation differences

Possible files:

```text
normalize_paths.c
normalize_timestamps.c
normalize_sockets.c
normalize_ids.c
```

Outputs:

* deterministic structured objects

Rules:

* no heuristic analysis here
* keep this layer boring

---

## 9.7 `src/correlate/`

Responsibilities:

* relate evidence objects across domains

Possible files:

```text
correlate_sockets.c
correlate_process.c
correlate_service.c
correlate_deleted_files.c
correlate_limits.c
```

Outputs:

* correlated views
* relationship tables
* enriched summary objects

Rules:

* any correlation with uncertainty must preserve caveats

---

## 9.8 `src/analyze/`

Responsibilities:

* turn correlated evidence into findings

Possible files:

```text
analyze_findings.c
analyze_confidence.c
analyze_summary.c
analyze_timeline.c
```

Outputs:

* findings list
* warnings/caveats
* operational summary
* forensic summary

Rules:

* separate observed facts from derived judgments
* do not let narrative language get ahead of evidence

---

## 9.9 `src/emit/`

Responsibilities:

* render text output
* render JSON output
* support stable ordering and formatting

Possible files:

```text
emit_text.c
emit_json.c
emit_common.c
```

Outputs:

* user-visible output streams or in-memory buffers

Rules:

* emitters should not perform collection or analysis
* formatting must be deterministic

---

## 9.10 `src/bundle/`

Responsibilities:

* create the evidence bundle directory
* write raw artifacts, structured artifacts, metadata, and reports
* preserve deterministic layout

Possible files:

```text
bundle_layout.c
bundle_write.c
bundle_meta.c
```

Outputs:

* on-disk evidence bundle

Rules:

* write only to the configured output directory
* do not silently overwrite without an explicit policy
* bundle structure must be documented and stable

---

## 9.11 `src/util/`

Responsibilities:

* narrow utility helpers

Examples:

* string handling
* file helpers
* sort wrappers
* errno formatting
* time formatting
* bounded buffers
* path helpers

Rules:

* do not dump unrelated logic here
* avoid turning `util` into a junk drawer

---

## 9.12 `src/style/`

Optional but useful.

Responsibilities:

* helpers that support project-wide style conventions

Examples:

* cleanup helpers
* resource-close helpers
* error-recording helpers
* deterministic ordering helpers

This directory exists only if it helps consistency. It should not become a dumping ground for code that lacks a home.

---

# 10. Interface Boundaries

Each module must expose only what other modules actually need.

## 10.1 Header Strategy

Use public headers under:

```text
include/autopsyctl/
```

Each header should expose:

* types
* enums
* function declarations
* interface-level ownership notes when needed

Avoid large comment dumps in headers.

---

## 10.2 Internal Functions

Functions not intended for cross-module use should be `static`.

Do not export broad surfaces “just in case.”

---

## 10.3 Dependency Direction

Preferred dependency flow:

```text
cli -> target -> collect -> parse -> normalize -> correlate -> analyze -> emit -> bundle
```

Allowed utilities may be shared downward or laterally where sane, but avoid back-coupling.

Bad examples:

* collector calling emitter
* parser directly writing bundle files
* emitter reopening `/proc`
* analyzer depending on CLI parsing details

---

# 11. Error Architecture

## 11.1 Error Categories

The system should distinguish at least these categories:

* argument/configuration error
* target resolution error
* collection error
* parsing error
* normalization error
* correlation error
* emission error
* bundle writing error

Not all errors are fatal to the run.

---

## 11.2 Severity

Recommended severity classes:

* `INFO`
* `WARN`
* `ERROR`
* `FATAL`

Meaning:

* `INFO`: context, no degradation
* `WARN`: partial evidence, still useful
* `ERROR`: meaningful feature loss, run may continue
* `FATAL`: cannot produce a valid snapshot of the requested target

---

## 11.3 Fatality Rules

Examples of likely fatal conditions:

* invalid CLI target
* cannot resolve target identity
* cannot initialize output mode required by the run
* cannot build minimal snapshot object

Examples of likely non-fatal conditions:

* one proc file unavailable
* one namespace unreadable
* journal unavailable
* one malformed socket row
* target exits after partial collection began

These distinctions must be encoded deliberately, not improvised everywhere.

---

## 11.4 Error Recording

The architecture should support structured error recording.

An error record should ideally include:

* stage
* operation
* resource
* errno if relevant
* severity
* message
* whether collection continued

This supports both debugging and final reporting.

---

# 12. Evidence Confidence Model

The tool will often operate on live, changing state. Confidence must be explicit.

## 12.1 Confidence Levels

Suggested confidence classes:

* `CONFIRMED`
* `OBSERVED_PARTIAL`
* `INFERRED`
* `UNAVAILABLE`

Examples:

* process exe link resolved successfully → `CONFIRMED`
* socket mapping incomplete because one fd disappeared → `OBSERVED_PARTIAL`
* likely crash loop inferred from repeated service restarts → `INFERRED`
* journal unavailable due to permission restrictions → `UNAVAILABLE`

---

## 12.2 Why This Matters

This tool must not pretend certainty where Linux runtime evidence is unstable or partial.

That is especially important for:

* live `/proc`
* deleted files
* transient sockets
* systemd runtime state
* containerized namespace visibility

---

# 13. Determinism Strategy

Deterministic output is a first-class architectural requirement.

## 13.1 Stable Sorting

Collections must be sorted explicitly.

Examples:

* PID lists numeric ascending
* file descriptor entries by fd number
* socket rows by inode then address
* paths lexicographically
* findings by severity then stable key

---

## 13.2 Timestamp Handling

Timestamps should be normalized into a documented format.

Prefer one canonical internal representation and one consistent emitted representation.

---

## 13.3 Bundle Layout Stability

The on-disk snapshot should have a fixed layout so:

* tests can compare runs
* humans can navigate bundles easily
* downstream tools can parse bundles reliably

---

# 14. Output Architecture

The system should support three top-level output products.

## 14.1 Text Report

Purpose:

* operator-readable summary
* concise but precise
* suitable for terminal or incident notes

Suggested sections:

* target summary
* process/service identity
* file descriptor summary
* network summary
* memory mapping summary
* security context
* findings
* caveats

---

## 14.2 JSON

Purpose:

* machine-readable export
* downstream tooling
* snapshot comparison groundwork
* testability

Design requirements:

* deterministic key ordering where feasible in emitter logic
* clear distinction between observed facts and findings
* explicit null or unavailable markers

---

## 14.3 Evidence Bundle

Purpose:

* preserve raw and normalized artifacts for offline review

Suggested contents:

```text
snapshot/
  meta.json
  target.json
  collection/
  parsed/
  normalized/
  report.txt
  report.json
  warnings.json
```

This may evolve, but the principle is stable:
preserve enough evidence to support later review without rerunning against the live target.

---

# 15. Systemd Integration Architecture

Systemd support should be optional and isolated.

## 15.1 Placement

Systemd-specific target resolution and journal-aware collection should live in dedicated modules, not scattered everywhere.

Possible modules:

```text
target_unit.c
collect_journal.c
collect_systemd.c
```

---

## 15.2 Failure Posture

If systemd is unavailable:

* `pid` mode should still work
* `unit` mode should fail cleanly
* systemd-specific sections should report unavailability explicitly

Do not contaminate generic PID inspection with systemd assumptions.

---

# 16. Test Architecture

The architecture must support testing without requiring every test to touch live kernel state.

## 16.1 Testable Boundaries

Best candidates for fixture-driven tests:

* parsers
* normalizers
* correlators
* emitters
* bundle writers

More limited integration tests:

* live collection
* systemd resolution
* `/proc` snapshots in containers

---

## 16.2 Fixtures

The repository should maintain fixture sets for:

* proc status
* maps
* socket tables
* cgroup entries
* journal excerpts
* deleted executable scenarios
* fd leak scenarios

---

## 16.3 Golden Output Tests

JSON and text outputs should support stable snapshot tests.

That is only possible if the architecture keeps emission deterministic.

---

# 17. Security Architecture Considerations

This is not a hardening agent. It is a read-only inspection tool. Still, the architecture should defend against obvious mistakes.

## 17.1 Read-Only Discipline

Modules should not require write privileges except bundle output.

Do not introduce “helpful” write-side behavior.

---

## 17.2 Path Safety

Bundle-writing code must:

* confine writes to configured output root
* avoid path traversal issues
* avoid accidental overwrite unless policy explicitly allows it

---

## 17.3 Bounded Parsing

Parsing modules must assume untrusted or unstable input even when reading kernel-facing files.

Why:

* truncated reads happen
* live process state changes
* helper command output can vary
* corrupted inputs may appear in tests

---

# 18. Future Extension Points

The architecture should leave room for expansion without forcing a redesign.

## 18.1 Compare Mode

Likely future pipeline:

```text
load snapshot A
  -> load snapshot B
  -> parse stored artifacts
  -> normalize
  -> diff evidence domains
  -> emit comparison report
```

This becomes much easier if the live-run architecture already separates raw, normalized, and analyzed forms.

---

## 18.2 Container Support

Container support should likely extend target resolution and collection, not rewrite the entire pipeline.

Possible additions:

* container PID root mapping
* namespace-based collection context
* runtime-specific hints

---

## 18.3 Additional Evidence Sources

Possible later sources:

* ELF richer metadata
* core dump presence/configuration
* OOM signals
* unit restart counters
* capability decoding improvements
* eBPF-derived signals if ever added

The architecture should allow these to slot into collection, parsing, correlation, and analysis without cross-cutting chaos.

---

# 19. Anti-Patterns to Avoid

## 19.1 God Context Objects

Do not hide all state inside one giant mutable structure passed everywhere.

Context should carry run-scoped coordination, not every domain object.

---

## 19.2 Smart Collectors

Collectors should not be doing half the parser’s and analyzer’s job.

If a collector “reads and interprets and decides severity,” the layering is broken.

---

## 19.3 Emitters Doing Analysis

If text output code starts inventing findings or resolving relationships on the fly, the architecture has failed.

---

## 19.4 Utility Junk Drawer

Do not dump unrelated logic into `util/` because no one wanted to decide where it belongs.

That is how codebases rot.

---

## 19.5 Excessive Optionality Early

Do not architect for every future plugin fantasy before the base system works.

Get the straight pipeline working first.

---

# 20. Recommended Initial Build Sequence

The first implementation should follow this order.

## Phase 1: PID Core

Build:

* CLI for `pid`
* target resolution for PID
* proc status collection
* fd collection
* maps collection
* parsing
* normalization
* JSON emission
* text summary

This proves the base pipeline.

---

## Phase 2: Socket and Executable Correlation

Add:

* socket table collection
* inode correlation
* exe hashing/build metadata
* deleted file detection
* findings generation

This makes the tool actually interesting.

---

## Phase 3: Unit Mode

Add:

* systemd unit resolution
* journal collection
* service-oriented findings
* process tree support if needed

This expands into SRE and incident response territory.

---

## Phase 4: Evidence Bundles

Add:

* bundle layout
* raw artifact writing
* metadata files
* deterministic bundle tests

This makes the tool valuable for offline review.

---

## Phase 5: Compare

Add:

* snapshot loading
* normalization from bundle artifacts
* domain-specific diffing
* comparison report

Only do this after the live snapshot architecture is stable.

---

# 21. Final Architectural Standard

The architecture is considered healthy if it preserves these properties:

* target resolution is separate from evidence collection
* collection is separate from parsing
* parsing is separate from correlation
* correlation is separate from analysis
* analysis is separate from emission
* output is deterministic
* partial evidence is explicit
* confidence is explicit
* module boundaries are readable
* implementation can evolve without collapsing into cross-module spaghetti

If a new feature cannot be added without violating those rules, the design should be corrected before the feature is merged.

```

Next should be `README.md`, because that will force the project story, scope, and first milestone into one coherent public-facing document.
```
