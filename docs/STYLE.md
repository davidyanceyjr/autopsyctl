# STYLE.md
**Project:** autopsyctl  
**Language:** C17  
**Purpose:** Human-auditable systems programming style for Linux tooling

---

# 1. Why This Style Exists

This project is built on a simple premise:

AI-assisted code generation can increase throughput, but it also increases the burden on human review. In systems code, especially Linux, security, debugging, incident response, and forensic tooling, review quality matters more than novelty and more than raw output speed.

This style exists to make C easier to:

- read
- review
- reason about
- debug
- audit
- modify safely

The standard is not "most clever" or "fewest lines."  
The standard is: **Can a human reviewer quickly understand what this function does, what it depends on, how it fails, and how it cleans up?**

---

# 2. Core Principles

## 2.1 Human Review First

Code should be optimized for human inspection, not for cleverness, code golf, or stylistic performance theater.

A reviewer should be able to answer these questions quickly:

- What is this function trying to do?
- What are the phases of work?
- What resources are acquired?
- What can fail?
- What cleanup is required?
- What assumptions are being made?
- What outputs are trusted, partial, or best-effort?

---

## 2.2 Explicit Control Flow

Non-trivial functions should expose their control flow clearly.

Preferred characteristics:

- visible phases
- shallow nesting
- explicit failure paths
- centralized cleanup
- descriptive labels where labels improve comprehension

The purpose of `goto` in this codebase is not style signaling.  
The purpose is to support readable phase transitions and reliable cleanup.

---

## 2.3 Determinism Over Cleverness

Code should behave predictably.

Avoid:

- hidden state mutation
- surprising macros
- side-effect-heavy expressions
- ambiguous ownership
- dense control structures with multiple responsibilities

Prefer:

- simple structs
- direct conditions
- explicit resource ownership
- stable ordering
- obvious state transitions

---

## 2.4 Small, Focused Units

Functions should do one coherent job.

If a function is doing parsing, collection, correlation, formatting, and cleanup all at once, it is too large.

Split along meaningful boundaries.

---

# 3. Function Design Rules

## 3.1 Functions Must Have a Single Purpose

Each function should answer one question:

- collect proc metadata
- parse socket row
- resolve fd target
- emit JSON field
- hash executable
- correlate inode to socket

Bad functions mix unrelated work:

- parse proc state and emit final report
- resolve cgroup and print user-facing warnings
- read logs and build timeline and allocate output paths

---

## 3.2 Non-Trivial Functions Use Phases

Functions of meaningful complexity should be divided into explicit phases.

Typical phases include:

- `validate_input:`
- `acquire_resources:`
- `read_state:`
- `transform_state:`
- `emit_result:`
- `cleanup:`
- `fail:`

Not every function needs every phase.  
Do not add labels mechanically.  
Add them when they improve human understanding.

---

## 3.3 Labels Must Be Descriptive

Allowed:

```c
validate_input:
acquire_proc_fd:
read_proc_status:
correlate_socket_inodes:
emit_json:
cleanup:
fail_open_proc:
fail_parse_status: