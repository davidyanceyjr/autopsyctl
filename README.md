````md
# autopsyctl

Read-only Linux incident triage, debugging, and forensic snapshotting in C17.

`autopsyctl` collects evidence about a process or service, correlates what it finds across Linux runtime interfaces, and emits deterministic reports for human review.

The project is built around one central idea:

**systems code should be easy for humans to audit.**

That matters even more in a world where AI-generated code can increase throughput while also increasing the volume of code humans must approve.

---

# Status

This project is in active design and bootstrap implementation.

Current design documents:

- `SPEC.md`
- `STYLE.md`
- `ARCHITECTURE.md`

Initial implementation target:

- `pid` mode
- read-only process snapshotting
- deterministic JSON and text output
- human-auditable control flow in C17

Current bootstrap implementation:

- a `collect_proc_status(...)` collector for `/proc/<pid>/status`
- explicit collector result codes for invalid input, unavailable targets, and parse failures
- offline `make test` coverage for collector semantics plus a smoke test

---

# Why This Exists

Most Linux tooling in this space falls into one of these buckets:

- excellent but operationally heavy
- shell-script glue with weak structure
- security products with opaque behavior
- one-off debugging tools
- toy projects that do not survive real review

`autopsyctl` is meant to occupy a different space:

- small enough to understand
- serious enough to matter
- read-only by design
- useful for debugging, incident triage, and forensic review
- implemented in a C style optimized for human reasoning

This is not an EDR, not a hardening agent, and not a repair tool.

It is a **snapshot and evidence tool**.

---

# Goals

`autopsyctl` is intended to:

- inspect a target process by PID
- inspect a systemd unit by service name
- collect evidence from `/proc`, `/sys`, cgroups, namespaces, sockets, executable metadata, and journal logs
- correlate runtime state into a coherent process or service view
- emit:
  - a human-readable report
  - structured JSON
  - an evidence bundle suitable for offline review
- preserve deterministic output ordering
- make failure, partial evidence, and uncertainty explicit

---

# Non-Goals

`autopsyctl` is not trying to be:

- an antivirus product
- a rootkit detector
- a vulnerability scanner
- a daemon
- a live monitoring platform
- an automated remediation tool
- a kernel agent
- a memory forensics framework

It performs **single-run, read-only collection**.

---

# Project Thesis

Modern code generation changes the economics of implementation, but it does not remove the need for human judgment.

In low-level Linux code, the hard problem is not just producing code that compiles. The hard problem is producing code that a human reviewer can trust.

This project treats readability as an engineering constraint, not a cosmetic preference.

That means:

- explicit phases
- centralized cleanup
- constrained use of `goto`
- shallow nesting
- deterministic behavior
- precise errors
- clear separation between observed facts and derived findings

The point is not novelty. The point is reviewability.

---

# What It Does

At a high level, `autopsyctl` does this:

1. resolve a target
2. collect raw evidence
3. parse and normalize evidence
4. correlate related runtime objects
5. generate findings and caveats
6. emit text, JSON, and optionally a bundle

Examples of evidence sources include:

- `/proc/<pid>/status`
- `/proc/<pid>/cmdline`
- `/proc/<pid>/exe`
- `/proc/<pid>/fd/`
- `/proc/<pid>/maps`
- `/proc/<pid>/ns/`
- `/proc/<pid>/cgroup`
- `/proc/net/tcp`
- `/proc/net/udp`
- `/sys/fs/cgroup`
- `journalctl` for service-aware collection when systemd is present

---

# Example Use Cases

## Debugging a crash-looping service

Inspect a systemd unit and correlate:

- recent restarts
- journal errors
- executable identity
- file descriptor usage
- mapped deleted libraries
- socket state
- cgroup placement

---

## Investigating suspicious process state

Inspect a PID and detect:

- deleted executable or mapped library references
- unusual namespace isolation
- capability or seccomp settings
- excessive open file descriptors
- network connections associated with the process

---

## Capturing evidence before a target disappears

Produce a stable bundle of:

- raw collection artifacts
- normalized state
- text report
- JSON summary

for later offline review.

---

# Core Design Principles

## Read-only operation

The tool must not modify target state.

Allowed actions are limited to collection-related operations such as:

- `open`
- `read`
- `readlink`
- `stat`
- directory traversal
- bounded helper command execution for read-only system data

The only writes allowed are to the configured output directory when emitting reports or evidence bundles.

---

## Deterministic output

The same target under the same observed conditions should produce stable ordering.

This applies to:

- file descriptor lists
- PID lists
- paths
- findings
- JSON emission
- on-disk bundle layout

This is required for both review quality and testing.

---

## Partial evidence is explicit

Linux runtime state is live and unstable. Targets may exit during collection. Permissions may be restricted. System interfaces may differ between distributions.

The tool must distinguish between:

- confirmed observations
- partial observations
- inferred findings
- unavailable evidence

It must never pretend certainty it does not have.

---

## Human-auditable code

The codebase uses a phase-oriented C style designed to make review easier.

This is documented in `STYLE.md`.

The short version:

- non-trivial functions should expose meaningful phases
- cleanup should be centralized
- labels must be descriptive
- loops are restricted to places where iteration is genuinely the job
- comments explain assumptions and reasoning, not obvious syntax

---

# Supported Modes

## `pid`

Inspect a running process by PID.

Example:

```sh
autopsyctl pid 1234
````

Intended coverage:

* identity
* executable
* cwd/root
* file descriptors
* maps
* sockets
* namespaces
* cgroups
* security context

---

## `unit`

Inspect a systemd unit by service name.

Example:

```sh
autopsyctl unit nginx.service
```

Intended coverage:

* service identity
* recent journal events
* resolved process targets
* runtime process state
* restart/crash hints

This mode depends on systemd integration and may be unavailable on some systems.

---

## `compare`

Planned later.

Compare two previously captured bundles.

Example:

```sh
autopsyctl compare snapshotA snapshotB
```

---

# Planned Output Modes

## Text report

A concise human-readable report intended for terminal use and incident notes.

Example sections:

* target summary
* process/service identity
* open file descriptors
* network summary
* memory mappings
* security context
* findings
* caveats

---

## JSON report

A machine-readable representation intended for:

* tooling
* automation
* testing
* snapshot diffing later

The JSON must preserve the difference between:

* observed facts
* derived findings
* caveats
* unavailable evidence

---

## Evidence bundle

A deterministic on-disk directory containing:

* metadata
* raw artifacts where useful
* structured outputs
* final text report
* final JSON report

This exists so an operator can review evidence later without rerunning against a live target.

---

# Intended Repository Layout

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

  tests/
  fixtures/
  docs/

  README.md
  SPEC.md
  STYLE.md
  ARCHITECTURE.md
```

---

# Implementation Roadmap

## Phase 1: PID core

Build the minimum serious pipeline:

* CLI parsing for `pid`
* PID target resolution
* proc metadata collection
* fd collection
* maps collection
* parser layer
* normalization layer
* text emission
* JSON emission

This phase proves the architecture.

---

## Phase 2: executable and socket correlation

Add:

* executable hashing
* build metadata where practical
* socket table parsing
* inode correlation
* deleted-file detection
* initial findings engine

This phase makes the tool meaningfully useful.

---

## Phase 3: systemd unit mode

Add:

* unit target resolution
* journal collection
* service-aware findings
* restart/crash narrative support

This phase expands the tool into SRE and incident triage territory.

---

## Phase 4: evidence bundles

Add:

* deterministic on-disk bundle layout
* raw artifact preservation
* metadata files
* stable bundle-writing tests

This phase makes the tool useful for offline review.

---

## Phase 5: comparison support

Add:

* bundle loading
* normalized diffing
* domain-specific comparison logic
* comparison text and JSON reports

Only do this once the snapshot format is stable.

---

# Coding Model

This project deliberately does not chase “idiomatic C” in the lazy sense of whatever is common.

It uses a stricter model aimed at reviewability.

A typical non-trivial function should read as a sequence of named phases, for example:

```c
validate_input:
acquire_resources:
read_state:
transform_state:
emit_result:
cleanup:
fail:
```

That does **not** mean every function needs labels. It means meaningful functions should make control flow easy to inspect.

This project allows `goto` where it improves:

* cleanup correctness
* phase clarity
* error-path readability

It does **not** allow `goto` as an excuse for spaghetti.

See `STYLE.md` for the actual standard.

---

# Testing Approach

This project should be testable at multiple levels.

## Unit tests

Focus on:

* parsers
* normalizers
* correlators
* emitters

These should run from fixtures without requiring a live target.

---

## Integration tests

Use controlled environments such as containers or disposable VMs to simulate:

* fd leaks
* deleted executables
* namespace isolation
* service restarts
* partial permission scenarios

---

## Golden tests

Validate deterministic JSON and text output.

This is only possible if ordering is stable and output semantics are disciplined.

---

# Security and Trust Model

This tool is security-relevant in the sense that it touches incident triage and runtime evidence, so it must behave conservatively.

Assumptions:

* targets may change while being inspected
* access may be partial
* some evidence sources may be absent
* helper command output may vary by distribution
* runtime state may disappear between steps

Therefore:

* the tool must prefer explicit caveats over false confidence
* parsing must be bounded and defensive
* output must distinguish observation from inference
* write operations must remain confined to the output directory

---

# Why C17

This project is intentionally written in C17 because it is close to the Linux runtime interfaces being inspected and because it forces discipline about:

* ownership
* error handling
* data layout
* control flow
* runtime cost

That discipline is part of the point.

The project is not trying to show off “unsafe C.” It is trying to demonstrate careful systems programming under review-oriented constraints.

---

# Why Not Just Shell or Python

Those are useful tools, but they trade away some of what this project is meant to demonstrate.

This project is explicitly about:

* systems-level Linux understanding
* structured error handling
* runtime data modeling
* deterministic output
* low-level evidence collection
* code auditability in a language where mistakes are expensive

That combination is part of the value.

---

# Build Philosophy

The build should stay simple.

Preferred qualities:

* minimal dependencies
* clear compiler flags
* reproducible builds
* warnings enabled aggressively
* straightforward local development

A likely baseline:

* `cc`
* `make`
* optional test harness tooling
* optional sanitizer builds during development

The project should be able to build cleanly with strong warnings enabled.

---

# Documentation Set

This project should maintain these core documents:

* `README.md` — public overview and project story
* `SPEC.md` — base implementation specification
* `STYLE.md` — coding and review standard
* `ARCHITECTURE.md` — module boundaries and data flow

Likely future additions:

* `FORENSICS.md`
* `TESTING.md`
* `CONTRIBUTING.md`

---

# Who This Is For

`autopsyctl` is meant for people who care about at least one of these:

* Linux systems programming
* debugging real runtime behavior
* service triage
* incident response
* forensic evidence collection
* engineering code that humans can actually review

It is also a statement project: a serious portfolio piece that demonstrates Linux, C, reasoning, and engineering discipline together.

---

# What Makes This Project Different

The differentiator is not merely that it is written in C.

The differentiator is the combination of:

* Linux runtime evidence collection
* security and incident-response usefulness
* deterministic artifact generation
* explicit confidence and caveat handling
* a review-first C style designed for human reasoning

That is the actual thesis.

---

# Current Priority

The immediate priority is to build a clean Phase 1 implementation for `pid` mode with:

* proc status collection
* fd collection
* maps collection
* deterministic internal representations
* text report
* JSON report

Everything else should stay subordinate to getting that base right.

---

# License

TBD.

Choose deliberately. Do not leave this as an afterthought.

---

# Contributing

Contributions should preserve the core project standards:

* read-only behavior
* deterministic output
* explicit caveats
* human-auditable code
* disciplined module boundaries

A future `CONTRIBUTING.md` should codify review expectations and coding rules for contributors.

---

# Development Notes

The design of this project rejects two common failures:

1. tools that collect a lot of evidence but are hard to trust
2. codebases that compile but are painful to review

If the implementation becomes clever at the expense of clarity, the project has missed its own point.
