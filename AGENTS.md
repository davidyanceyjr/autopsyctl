
# AGENTS.md

## Purpose

This repository is a reference implementation and working scaffold for **Phase-Oriented C** on Linux systems, with an emphasis on:

- human-auditable control flow
- air-gapped operation
- low dependency footprint
- deterministic behavior
- security-conscious systems programming

Agents, contributors, and code-generation tools operating in this repository must preserve those goals.

---

## Repository Intent

This project is not a generic C sandbox.

It exists to explore and demonstrate a disciplined C17 style for Linux tooling where functions are organized into explicit operational phases, failure handling is monotonic, and cleanup is centralized.

Primary use cases include:

- Linux diagnostics
- troubleshooting
- debugging support
- forensic collection
- reliability and SRE tooling
- reviewable AI-assisted C code generation

---

## Engineering Workflow

This repository follows a structured solo-engineering workflow intended to keep AI-assisted development reviewable and deterministic.

Standard lifecycle:

issue → specification → design → implementation → tests → debug → commit → review → merge → documentation

Artifacts map to this lifecycle as follows:

| Stage | Artifact |
|------|---------|
| issue | GitHub issue |
| specification | `spec/<issue>.md` |
| design | `docs/design/<issue>.md` |
| implementation | `src/` |
| verification | `tests/` |
| documentation | `docs/` |

Agents should not skip workflow stages without explicit justification.

Examples:

- do not implement complex behavior without a specification
- do not merge behavior changes without tests
- do not change CLI behavior without updating documentation

Agents should begin new sessions by determining repository state using the `repo-state` skill.

---

## AI Skills System

This repository uses a `.skills/` directory containing procedural instructions for AI-assisted development tasks.

Skills define repeatable engineering operations such as:

- repository state inspection
- issue creation
- specification drafting
- design review
- implementation planning
- test generation
- patch review
- debugging triage
- documentation synchronization

Skills are grouped by category:

.skills/
  workflow/   lifecycle transitions
  github/     issue and PR management
  git/        local repository discipline
  debug/      failure reproduction and debugging
  quality/    architecture and interface review
  release/    changelog and milestone preparation
  testing/    test framework maintenance

Agents should prefer invoking an appropriate skill instead of improvising new workflows.

Skills represent engineering procedures, not merely suggestions.

---

## AI Response Contract

When guiding development work, agents should structure responses using the following format when appropriate:

Stage
-----
Current workflow stage.

Skill
-----
Skill selected to guide the task.

Action
------
Concrete next steps.

Why
---
Reasoning behind the step.

Next
----
What should happen after the step completes.

---

## Hard Constraints

### Platform
- Target platform is **Linux**.
- Language baseline is **C17**.
- Default assumption is **air-gapped deployment**.

### Air-Gap Rules
Agents must not introduce features that require:
- internet access
- cloud APIs
- remote telemetry
- online package fetching at runtime
- remote configuration dependency
- license checks that require network access

Optional networking features are discouraged. If added, they must be isolated, disabled by default, and removable without affecting core functionality.

### Dependency Rules
Prefer, in order:
1. libc and Linux/POSIX interfaces
2. small local source files
3. tiny vendored libraries with clear justification

Avoid:
- large dependency trees
- auto-generated build chains that fetch dependencies
- opaque build tooling
- runtime plugin systems unless strongly justified

---

## Coding Standard

### Phase-Oriented C
Non-trivial functions should be structured into named phases such as:

- `validate_input:`
- `initialize_state:`
- `acquire_resources:`
- `read_inputs:`
- `transform_state:`
- `check_invariants:`
- `emit_results:`
- `cleanup:`

Not every function needs every phase, but labels must describe intent.

### Control Flow
Use `goto` for:
- phase transitions where it improves reviewability
- centralized cleanup
- monotonic failure routing

Do not use `goto` to create spaghetti control flow.

### Cleanup
Resource cleanup should be centralized whenever practical.

Every owned resource should have:
- a clear owner
- a sentinel or initialized state
- a visible cleanup path

### Loops
Loops are allowed in:
- parsers
- iterators
- traversal helpers
- serialization helpers
- narrow scanning functions

Top-level orchestration functions should favor explicit phases over dense loop-heavy logic.

### Nesting
Avoid deep nesting. Prefer:
- early checks
- helper extraction
- explicit phase boundaries

### Comments
Comments should explain:
- why a phase exists
- invariants
- trust boundaries
- forensic or operational caveats
- non-obvious assumptions

Comments should not narrate obvious syntax.

---

## Build Expectations

Agents must preserve an offline-friendly build.

### Required properties
- `make` should work without network access
- tests should run locally
- the default build should not fetch dependencies
- helper scripts should remain POSIX-shell friendly where practical

### Preferred compiler posture
Keep compatibility with:
- `cc`
- `gcc`
- `clang`

Do not assume exotic toolchains unless explicitly documented.

---

## File and Directory Guidance

### `docs/`
Specifications, style guidance, air-gap notes, and other design documents.

### `include/`
Public headers and shared internal interfaces that need stable declarations.

### `src/`
Implementation files. Keep modules small and purpose-driven.

### `tests/`
Smoke tests, unit tests, fixture-based tests, and golden-output checks.

### `scripts/`
Offline-safe helper scripts for build, packaging, checks, and local workflows.

---

## Change Policy for Agents

When making changes, agents should prefer:

1. correctness
2. auditability
3. deterministic behavior
4. low operational surprise
5. performance, after the above are protected

Do not optimize readability away for cleverness.

Do not introduce abstractions that hide resource ownership or failure behavior unless they clearly reduce total complexity.

Do not silently change project scope.

If a requested change conflicts with the air-gap or auditability goals, preserve repository intent and make the tension explicit in comments or documentation.

---

## Output and Error Handling Expectations

New code should aim to:
- distinguish success, partial success, and failure
- preserve error context
- avoid silent degradation
- report unavailable evidence honestly
- keep output deterministic when feasible

For diagnostics or forensic collection code, distinguish between:
- observed fact
- inference
- unavailable data
- permission-denied data

---

## Testing Expectations

Agents adding code should also add or update tests when practical.

Preferred test categories:
- smoke tests
- parser unit tests
- deterministic output checks
- regression tests for cleanup and error paths

A small well-targeted test is better than a large fragile one.

---

## Security Posture

Avoid:
- unsafe string handling
- shell injection hazards
- hidden subprocess dependencies
- temporary-file race patterns
- unnecessary privilege requirements

Prefer:
- explicit bounds handling
- argument-vector subprocess execution when subprocesses are necessary
- least-privilege operation
- read-only collection behavior for forensic or diagnostic tooling

---

## Commit/Change Style Guidance

Keep changes narrow and reviewable.

Prefer:
- one logical change per edit
- matching doc updates when behavior changes
- clear naming
- small helper functions over dense monoliths

When generating code, produce code that a human reviewer can approve quickly.

That is the standard.
