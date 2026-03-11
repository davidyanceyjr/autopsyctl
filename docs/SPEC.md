Below is a **solid base SPEC.md** suitable for a serious engineering repository. It is written like a real systems project specification: scope, architecture, constraints, interfaces, style rules, and phased implementation plan. It is intentionally implementation-lean so it can evolve during development.

---

# SPEC.md

**Project:** autopsyctl
**Language:** C17
**Domain:** Linux incident triage, debugging, and forensic snapshotting
**Primary Goal:** Human-auditable systems code

---

# 1. Overview

`autopsyctl` is a **read-only Linux system inspection tool** that collects evidence about a process, service, or host state and produces a deterministic snapshot suitable for debugging, incident response, or forensic analysis.

The tool gathers information from:

* `/proc`
* `/sys`
* `cgroups`
* `systemd`
* Linux namespaces
* open file descriptors
* sockets
* executable metadata
* journal logs

The collected evidence is transformed into:

1. **Human-readable report**
2. **Structured JSON output**
3. **Evidence bundle (filesystem snapshot)**

The project also demonstrates a **human-auditable C programming style** optimized for code review and reasoning.

---

# 2. Non-Goals

The project intentionally does **not** aim to be:

* a full EDR
* a rootkit detector
* a vulnerability scanner
* a system repair tool
* a continuous monitoring daemon

The tool performs **single-shot inspection only**.

---

# 3. Core Design Principles

## 3.1 Read-Only Operation

The tool must **never modify system state**.

Allowed actions:

* open
* read
* stat
* readlink
* directory traversal
* spawning read-only helper commands

Forbidden actions:

* writing outside output directory
* modifying kernel state
* killing processes
* editing configuration
* loading kernel modules

---

## 3.2 Deterministic Output

Repeated runs against the same target should produce **stable and predictable output ordering**.

Sorting rules:

* file lists sorted lexicographically
* PID lists sorted numerically
* socket lists sorted by inode
* timeline entries sorted by timestamp

---

## 3.3 Evidence Preservation

All collected artifacts must preserve:

* timestamps
* permissions
* inode identifiers
* path resolution

Output should be suitable for **offline review**.

---

## 3.4 Human-Auditable Code

The codebase uses a structured style designed for human reasoning:

* explicit function phases
* minimal nesting
* centralized cleanup
* descriptive control-flow labels

The goal is **review clarity**, not minimal lines of code.

---

# 4. Target Platforms

Primary target:

* Linux kernel 5.x+

Tested distributions:

* Debian
* Ubuntu
* Fedora
* Arch
* Alpine

Architecture:

* x86_64
* ARM64

---

# 5. Program Architecture

```
+------------------+
| CLI Interface    |
+---------+--------+
          |
          v
+------------------+
| Target Resolver  |
| pid/unit/host    |
+---------+--------+
          |
          v
+------------------+
| Evidence Collect |
| proc/sys/cgroup  |
+---------+--------+
          |
          v
+------------------+
| Data Correlation |
+---------+--------+
          |
          v
+------------------+
| Report Builder   |
+---------+--------+
          |
          v
+------------------+
| Output Emitters  |
| text/json/bundle |
+------------------+
```

---

# 6. CLI Interface

### Basic command

```
autopsyctl <mode> [options]
```

### Modes

#### PID inspection

```
autopsyctl pid <pid>
```

Inspect a running process.

---

#### Systemd unit inspection

```
autopsyctl unit <service>
```

Resolve service → process tree.

---

#### Container inspection

```
autopsyctl container <id>
```

Future extension.

---

#### Snapshot comparison

```
autopsyctl compare <snapshotA> <snapshotB>
```

Compare two collected evidence bundles.

---

### Global flags

```
--json
--text
--bundle
--output <directory>
--verbose
--quiet
--timeout <seconds>
```

---

# 7. Evidence Collection

## 7.1 Process Metadata

Sources:

```
/proc/<pid>/status
/proc/<pid>/cmdline
/proc/<pid>/exe
/proc/<pid>/cwd
/proc/<pid>/root
```

Collected fields:

* PID
* PPID
* UID/GID
* capabilities
* seccomp mode
* namespace IDs

---

## 7.2 File Descriptors

Source:

```
/proc/<pid>/fd/
```

Collected:

* fd number
* target path
* inode
* file type

Detection targets:

* deleted files
* sockets
* pipes

---

## 7.3 Memory Maps

Source:

```
/proc/<pid>/maps
```

Collected:

* mapped files
* deleted libraries
* executable regions

---

## 7.4 Sockets

Sources:

```
/proc/net/tcp
/proc/net/udp
/proc/<pid>/fd
```

Correlation:

```
socket inode → fd → process
```

Collected:

* local address
* remote address
* connection state

---

## 7.5 Cgroups

Source:

```
/proc/<pid>/cgroup
/sys/fs/cgroup
```

Collected:

* cgroup path
* limits
* container hints

---

## 7.6 Namespaces

Source:

```
/proc/<pid>/ns/
```

Collected:

* pid
* net
* mnt
* uts
* ipc
* user

---

## 7.7 Executable Metadata

Resolved via:

```
/proc/<pid>/exe
```

Collected:

* absolute path
* SHA256 hash
* ELF build ID
* file owner
* permissions

---

## 7.8 Journal Logs

If systemd is present:

```
journalctl -u <service>
```

Collected:

* recent restart events
* crash indicators
* warnings/errors

---

# 8. Data Correlation

Collected data is merged into structured views.

Example relationships:

```
Process
 ├─ file descriptors
 ├─ memory maps
 ├─ sockets
 ├─ namespaces
 └─ cgroup membership
```

Correlations include:

* socket ↔ process mapping
* mapped library ↔ file existence
* service ↔ process tree

---

# 9. Output Formats

## 9.1 Human Report

Plain text summary.

Example sections:

```
Process Summary
Open File Descriptors
Network Connections
Memory Maps
Security Context
Suspicious Indicators
```

---

## 9.2 JSON

Machine readable output.

Example structure:

```
{
  "process": {},
  "fds": [],
  "sockets": [],
  "namespaces": {},
  "cgroups": {},
  "evidence": []
}
```

---

## 9.3 Evidence Bundle

Directory structure:

```
snapshot/
  meta.json
  proc/
  sockets/
  files/
  journal/
  report.txt
```

---

# 10. Error Handling

Errors must include:

* operation
* resource
* errno
* severity

Example:

```
read_proc_status: permission denied (EPERM)
```

Failures should degrade gracefully.

Example:

```
journal access unavailable
continuing with partial evidence
```

---

# 11. Coding Standard

## 11.1 Function Structure

Functions are divided into explicit phases.

Typical phases:

```
validate_input
acquire_resources
read_state
transform_state
emit_result
cleanup
fail
```

---

## 11.2 Control Flow

Rules:

* single function entry
* centralized cleanup
* minimal nesting
* explicit error paths

Example pattern:

```
validate:
acquire:
read:
transform:
emit:
cleanup:
fail:
```

---

## 11.3 State Handling

Use small state structs.

Example:

```
struct proc_view
{
    pid_t pid;
    char exe_path[PATH_MAX];
    size_t fd_count;
};
```

---

## 11.4 Iteration Policy

Loops are allowed only in:

* parsers
* iterators
* emitters

High-level orchestration functions should avoid loops.

---

## 11.5 Error Discipline

All syscalls must be wrapped.

Example wrapper:

```
safe_open()
safe_read()
safe_stat()
```

Wrappers attach context to failures.

---

# 12. Repository Layout

```
autopsyctl/
 ├─ src/
 │  ├─ main.c
 │  ├─ cli/
 │  ├─ collect/
 │  ├─ correlate/
 │  ├─ emit/
 │  ├─ util/
 │  └─ style/
 │
 ├─ include/
 │
 ├─ tests/
 │
 ├─ fixtures/
 │
 ├─ docs/
 │
 ├─ SPEC.md
 └─ README.md
```

---

# 13. Testing Strategy

### Unit Tests

Test parsers:

* proc status
* maps
* socket tables

---

### Integration Tests

Run tool inside containers with known conditions.

Examples:

* fd leak
* crash loop
* deleted executable
* namespace isolation

---

### Snapshot Tests

Validate stable output.

```
snapshot A
snapshot B
diff
```

---

# 14. Performance Expectations

Expected runtime:

```
single PID inspection < 200ms
```

Constraints:

* avoid expensive recursion
* avoid unnecessary allocations

---

# 15. Security Considerations

The tool must assume:

* partial permissions
* containerized environments
* missing kernel interfaces

Evidence must include **confidence indicators**.

Example:

```
socket mapping incomplete
journal access restricted
```

---

# 16. Implementation Phases

## Phase 1 — Minimal PID Snapshot

Implement:

* PID metadata
* fd listing
* maps parsing
* JSON output

---

## Phase 2 — Network Correlation

Add:

* socket parsing
* inode correlation

---

## Phase 3 — Service Awareness

Add:

* systemd unit resolution
* journal integration

---

## Phase 4 — Evidence Bundles

Add:

* snapshot directory output
* artifact preservation

---

## Phase 5 — Comparison Engine

Implement:

```
autopsyctl compare
```

Show differences between snapshots.

---

# 17. Future Extensions

Possible additions:

* container runtimes
* namespace topology visualization
* eBPF signal correlation
* timeline reconstruction
* memory pressure detection
* crash loop analysis

---

# 18. Documentation

Required documents:

```
SPEC.md
README.md
STYLE.md
ARCHITECTURE.md
FORENSICS.md
```

---

# 19. Success Criteria

The project is considered successful if:

* it produces reliable system snapshots
* code is easy for humans to review
* behavior is deterministic
* it works across major Linux distributions
* it assists debugging and incident triage

---

If desired, the next step can be to produce a **STYLE.md that formalizes the “phase-oriented C” coding method**, which would reinforce the project's core differentiator.
