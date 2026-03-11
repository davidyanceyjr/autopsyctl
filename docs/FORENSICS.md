# FORENSICS.md
**Project:** autopsyctl  
**Language:** C17  
**Purpose:** Evidence handling, observation discipline, confidence rules, and forensic posture for read-only Linux snapshotting

---

# 1. Purpose

This document defines the forensic posture of `autopsyctl`.

`autopsyctl` is a **read-only evidence collection and triage tool** for Linux runtime state. It is intended to support debugging, incident response, service troubleshooting, and forensic review by collecting and preserving system observations in a deterministic form.

This document exists to define:

- what counts as evidence
- how evidence is collected
- how evidence is preserved
- how confidence is represented
- what inferences are allowed
- what the tool must not claim
- how to reason about live-system instability and partial evidence

This is not a legal chain-of-custody document. It is an engineering and operational evidence-handling document.

---

# 2. Forensic Posture

## 2.1 Read-Only by Design

`autopsyctl` must never alter the target system state as part of evidence collection.

The tool may:

- open files read-only
- read from `/proc`, `/sys`, and other runtime interfaces
- call `stat`, `readlink`, and similar metadata syscalls
- traverse directories needed for evidence collection
- execute bounded helper commands that retrieve read-only state
- write reports and artifacts only into the configured output directory

The tool must not:

- kill, stop, pause, or otherwise control target processes
- edit configuration
- change service state
- write to target-owned files
- load kernel modules
- modify cgroups, namespaces, or limits
- remediate, patch, or “fix” the system
- perform hidden cleanup of target artifacts

The first forensic claim of the project is therefore limited and precise:

**the tool is intended to observe and preserve, not to intervene.**

---

## 2.2 Snapshot, Not Memory Image

`autopsyctl` collects a **runtime snapshot of observable Linux state**. It is not:

- a physical disk imager
- a memory acquisition framework
- a kernel tracing platform
- a historical event recorder
- a complete ground truth system representation

It captures what can be read at collection time from the available interfaces and permissions.

That means all evidence must be interpreted with this constraint:

**the snapshot is an observation of a live and changing system, not a frozen universe.**

---

## 2.3 Evidence-Oriented, Not Verdict-Oriented

`autopsyctl` must prefer evidence over conclusions.

The tool may say:

- a deleted executable was still mapped
- a process had 1024 open file descriptors
- a service restarted 5 times in 3 minutes
- journal access was unavailable
- one namespace identifier differed from the host namespace

The tool must not claim:

- compromise is confirmed
- malware is present
- root cause is proven
- a user intended a given action
- an event happened when only one interpretation is possible unless the evidence actually supports that level of certainty

This distinction is non-negotiable.

---

# 3. Evidence Model

## 3.1 Categories of Evidence

Evidence in `autopsyctl` is divided into categories so the tool can distinguish raw observations from processed and derived results.

### 3.1.1 Raw Evidence

Raw evidence is data collected directly from source interfaces with minimal or no transformation.

Examples:

- contents of `/proc/<pid>/status`
- contents of `/proc/<pid>/maps`
- resolved text of `/proc/<pid>/fd/<n>`
- contents of `/proc/net/tcp`
- `stat` metadata
- `readlink` results
- journal output captured for a target unit

Raw evidence is closest to the source and should be preserved where useful.

---

### 3.1.2 Parsed Evidence

Parsed evidence is structured data extracted from raw evidence.

Examples:

- parsed process UID/GID
- parsed memory map entries
- parsed socket rows
- parsed cgroup paths
- parsed namespace identifiers

Parsed evidence is useful for reasoning, but it is already one step removed from the original raw form.

---

### 3.1.3 Normalized Evidence

Normalized evidence is parsed evidence transformed into canonical internal forms.

Examples:

- stable timestamps
- sorted fd tables
- canonical path forms where appropriate
- normalized inode representation
- stable ordering of findings inputs

Normalization exists to reduce noise and support deterministic output.

---

### 3.1.4 Correlated Evidence

Correlated evidence is produced by relating multiple observed domains.

Examples:

- fd entry linked to a socket inode
- socket linked to a process
- service linked to a process tree
- mapped file linked to deleted-on-disk status
- executable path linked to hash and build ID
- process linked to namespace and cgroup placement

Correlation is useful and often necessary, but it increases interpretive distance from raw evidence.

---

### 3.1.5 Findings

Findings are derived statements based on one or more correlated or normalized evidence sources.

Examples:

- process is near file descriptor exhaustion
- deleted library remained mapped during collection
- service showed signs consistent with crash-loop behavior
- network ownership mapping was partial because one or more descriptors disappeared during collection

Findings are not the same as raw evidence and must be labeled accordingly.

---

# 4. Observation Constraints

## 4.1 Live System Instability

Linux runtime state changes continuously. A process may exit between two reads. A file descriptor may close during enumeration. A journal window may rotate. A symlink target may disappear after resolution.

The tool must assume:

- evidence may change during collection
- evidence may disappear mid-run
- not all interfaces reflect one perfectly synchronized instant
- different subsystems expose different time windows

Because of this, the tool must treat collection as **best-effort observation under time drift**.

---

## 4.2 Permissions and Visibility

Different execution contexts will have different visibility.

Examples:

- an unprivileged user may be unable to inspect some `/proc` entries
- journal access may be restricted
- containerized environments may expose only partial host state
- namespace views may differ from host expectations
- security modules may hide or restrict information

This must not be flattened into generic failure.

The tool must report restricted visibility explicitly.

---

## 4.3 Distribution and Kernel Variation

Linux interfaces are not perfectly uniform across all distributions and kernel versions.

Differences may include:

- field presence in `/proc`
- helper command availability
- cgroup layout variation
- systemd presence or absence
- procfs formatting differences
- namespace or capability reporting differences

The tool must tolerate supported variation without pretending a single layout is universal.

---

# 5. Chain of Observation

## 5.1 What This Means

`autopsyctl` does not implement legal chain of custody. It implements a stricter engineering concept:

**chain of observation**.

This means the tool should preserve enough context that a reviewer can tell:

- what source produced a given artifact
- when it was collected
- how it was transformed
- whether it was partial
- what confidence applies to later statements derived from it

The aim is traceability of reasoning, not courtroom certification.

---

## 5.2 Required Traceability

For every meaningful artifact or derived conclusion, a reviewer should be able to determine:

- source interface or origin
- target identity
- collection phase or stage
- collection timestamp or run timestamp context
- whether the data was raw, parsed, normalized, correlated, or inferred
- whether the observation was degraded or partial

If a derived statement cannot be traced back to source observations, it should not be emitted.

---

## 5.3 Evidence Lineage

The intended lineage model is:

```text
source interface
  -> raw evidence
  -> parsed evidence
  -> normalized evidence
  -> correlated evidence
  -> finding
````

Every stage should preserve enough metadata that later stages do not become ungrounded.

Examples:

* “socket belongs to pid 1234” should be backed by socket-table parsing plus fd-to-inode correlation
* “mapped library deleted” should be backed by parsed maps plus path/deletion state
* “journal unavailable” should be backed by an explicit failed collection attempt or unavailable integration path

---

# 6. Confidence Model

## 6.1 Why Confidence Exists

Confidence exists because Linux runtime evidence is often incomplete, unstable, or inferential.

The tool must represent uncertainty directly instead of hiding it behind authoritative-sounding text.

---

## 6.2 Confidence Classes

The project should use confidence classes that are simple, explicit, and operationally meaningful.

### `CONFIRMED`

Used when the statement is directly supported by a successful observation.

Examples:

* `/proc/<pid>/exe` resolved successfully
* file descriptor `7` pointed to a specific inode at collection time
* namespace link target was read successfully
* executable hash was computed successfully

This does not mean metaphysical certainty. It means the observation was successfully obtained and is directly grounded.

---

### `OBSERVED_PARTIAL`

Used when an observation was made, but the evidence is incomplete or degraded.

Examples:

* process fd enumeration succeeded, but some targets disappeared during traversal
* socket correlation succeeded for some but not all descriptors
* journal window was truncated or partially inaccessible
* maps file was parsed, but one malformed line was skipped

This should be common and normal in live collection.

---

### `INFERRED`

Used when the tool derives a statement from multiple observations and the statement is reasonable but not directly read from one source.

Examples:

* behavior consistent with crash-looping
* process likely operating inside a distinct namespace context
* file descriptor pressure likely contributing to runtime instability
* network ownership mapping likely incomplete due to target churn

Inferred findings must identify their basis where practical.

---

### `UNAVAILABLE`

Used when evidence could not be collected or a statement cannot be made.

Examples:

* journal access denied
* systemd not available
* capability data unreadable
* target exited before a required source could be read

`UNAVAILABLE` is not the same as false.

---

## 6.3 Confidence Applies to Statements, Not Just Sources

Confidence should attach to reported claims, not only raw sources.

For example:

* raw maps read: `CONFIRMED`
* one deleted library entry observed: `CONFIRMED`
* “deleted library may explain crash behavior”: `INFERRED`

That distinction matters.

---

# 7. Severity Model

## 7.1 Purpose

Severity is separate from confidence.

Confidence answers: **how strongly is this statement supported?**

Severity answers: **how operationally important is this issue or caveat?**

---

## 7.2 Suggested Severity Classes

### `INFO`

Context only. No meaningful degradation or immediate concern.

Examples:

* process belongs to cgroup path X
* executable build ID present
* systemd unavailable in this environment

---

### `WARN`

Evidence or runtime state suggests a potentially important issue, or evidence quality is meaningfully degraded.

Examples:

* partial socket mapping
* deleted mapped library observed
* journal access restricted
* one optional proc source unreadable

---

### `ERROR`

Substantial feature loss or substantial runtime concern.

Examples:

* target identity incomplete
* bundle output could not preserve required artifact
* required collection source unavailable for requested mode
* severe file descriptor pressure observed

---

### `FATAL`

The requested snapshot or mode cannot be completed in a valid form.

Examples:

* invalid PID target
* target vanished before minimal identity could be established
* required output path unusable
* core collection pipeline failed before any valid snapshot could exist

---

# 8. Allowed Inference Rules

## 8.1 Inference Must Be Conservative

Inferences must be phrased conservatively and tied to observed evidence.

Allowed phrasing:

* “consistent with”
* “suggests”
* “appears”
* “likely”
* “correlation incomplete”
* “best-effort mapping indicates”

Disallowed phrasing unless directly proven:

* “caused by”
* “definitely compromised”
* “attacker used”
* “malware executed”
* “root cause identified”

---

## 8.2 Findings Must Not Outrun Data

A finding is valid only if it can be explained by a bounded set of observations.

Bad:

* “host is compromised”

Good:

* “process executable path resolved to a deleted file while active mappings remained present”
* “service restarted repeatedly during the observed journal window”
* “socket ownership could not be completed for all entries because some descriptors disappeared during collection”

---

## 8.3 Human Intent Is Out of Scope

The tool must not claim user motive, attacker motive, or intent.

It can report observed artifacts and correlations. It cannot know why a person did something.

---

## 8.4 Time Causality Must Be Treated Carefully

The tool can report ordering when a source supports ordering. It must not overclaim causal relationships merely from proximity.

Allowed:

* “journal shows repeated restarts before the current snapshot”
* “mapped deleted file was observed during collection”

Not allowed:

* “the deleted library caused the service restart” unless the evidence truly supports that level of conclusion, which is unlikely for this tool’s scope

---

# 9. Raw Artifact Preservation

## 9.1 Why Preserve Raw Artifacts

Preserving raw artifacts improves:

* reviewer trust
* debugging of parser behavior
* offline reanalysis
* auditability of findings
* comparison support later

Where feasible and justified, raw artifacts should be stored in the evidence bundle.

---

## 9.2 What Should Be Preserved

Examples of useful preservable artifacts:

* proc status text
* maps text
* resolved fd target table
* raw socket table excerpts
* journal text captured for the run
* metadata records for stat/readlink results
* collection warnings/errors

Not every kernel-facing source needs blind byte-for-byte dumping, but the tool should preserve enough raw material to support traceability.

---

## 9.3 Preservation Limits

The tool is not required to preserve every possible raw source if doing so would be:

* excessively redundant
* prohibitively noisy
* unstable without adding value
* impossible due to permissions or interface constraints

The preservation policy should be practical, not maximalist.

---

# 10. Target Identity Rules

## 10.1 Minimum Identity Requirement

The tool must establish a minimal target identity before claiming a valid snapshot exists.

For a PID target, this generally includes enough of the following to identify the process meaningfully:

* PID
* process existence at collection start
* executable link or command line where available
* basic proc status identity

If minimal identity cannot be established, the run should be considered invalid or fatal for that target.

---

## 10.2 Identity Drift

A PID may exit and a new process may later reuse the PID. Live collection must account for this possibility.

Where practical, the tool should preserve identity-relevant details that reduce ambiguity, such as:

* start-time-related identity data if collected
* executable path
* cmdline
* status metadata

If identity continuity becomes uncertain during collection, that uncertainty should be recorded.

---

# 11. Time Handling

## 11.1 Collection Timing Matters

The snapshot is not guaranteed to represent one perfect instant. It represents a bounded collection interval.

The tool should preserve:

* collection start time
* collection end time
* relevant artifact timestamps where available

This provides temporal framing for the evidence.

---

## 11.2 Timeline Discipline

When emitting time-based statements, the tool must distinguish between:

* collection time
* source event time
* file metadata time
* inferred ordering

These are not interchangeable.

Examples:

* journal line time is not the same as snapshot start time
* file mtime is not execution time
* collection order is not necessarily causal order

---

# 12. Source-Specific Rules

## 12.1 `/proc`

`/proc` is live and unstable.

Rules:

* partial reads are possible
* a target may disappear during collection
* one proc entry may succeed while another fails
* missing entries should not be assumed malicious by default

The tool should record `/proc` anomalies as evidence quality issues unless there is independent reason to escalate interpretation.

---

## 12.2 File Descriptor Links

`/proc/<pid>/fd/<n>` targets may change while enumerating. Some targets may be deleted, inaccessible, or transient.

Rules:

* preserve observed target text where possible
* distinguish deleted targets from unreadable targets
* do not assume missing targets imply concealment

---

## 12.3 Memory Maps

Maps entries are powerful but easy to over-interpret.

Rules:

* report observed mapped paths and properties
* report deleted mappings explicitly
* avoid implying malicious injection solely from unfamiliar mappings
* preserve raw lines or enough raw representation for later review where practical

---

## 12.4 Socket Tables

Socket correlation is inherently cross-domain and can be partial.

Rules:

* distinguish between parsed socket presence and resolved process ownership
* report incomplete mapping explicitly
* do not invent ownership for unmatched inodes

---

## 12.5 Journal Data

Journal evidence may be restricted, absent, rotated, or incomplete.

Rules:

* preserve collection window assumptions
* distinguish no entries from inaccessible entries
* do not claim the journal is authoritative for all events on the system

---

## 12.6 Cgroups and Namespaces

These interfaces can reveal isolation and runtime context, but not intent.

Rules:

* report what namespace or cgroup identifiers were observed
* note when those differ from baseline host expectations if the comparison exists
* do not equate isolation with maliciousness

---

# 13. What the Tool May Claim

The tool may claim only what its evidence supports.

Examples of acceptable claims:

* “The process had 328 open file descriptors at collection time.”
* “One mapped file entry referenced a deleted path.”
* “The service journal window contained 5 restart-related entries in the collected range.”
* “Socket ownership mapping was partial.”
* “Namespace identifiers differed from the host baseline.”
* “Executable hashing was unavailable due to permission restrictions.”
* “Findings below are derived from a live snapshot and may not represent all prior or subsequent state.”

These statements are bounded, source-aware, and defensible.

---

# 14. What the Tool Must Not Claim

The tool must not claim:

* compromise is proven
* malware is proven
* attacker identity is known
* user intent is known
* one artifact caused a later event unless strongly and explicitly supported
* absence of evidence equals absence of activity
* unavailable evidence equals benign state
* partial evidence equals deception

It must also not imply that one snapshot is a complete historical record.

---

# 15. Caveat Reporting

## 15.1 Caveats Are First-Class Output

Caveats are not embarrassing exceptions. They are required forensic metadata.

Examples:

* target exited during collection
* journal access restricted
* socket correlation incomplete
* one parser skipped malformed lines
* cgroup layout partially unsupported on this host
* some evidence unavailable due to permissions

These caveats help a reviewer decide how much weight to place on findings.

---

## 15.2 Caveats Must Be Specific

Bad caveat:

* “some stuff may be missing”

Good caveat:

* “two fd targets disappeared while `/proc/<pid>/fd` was being enumerated”
* “journal access failed with permission denied”
* “socket row parse skipped 1 malformed line in `/proc/net/tcp`”

Specific caveats are useful; vague caveats are laziness.

---

# 16. Determinism and Evidence Integrity

## 16.1 Why Determinism Matters

Deterministic output improves forensic review because it reduces accidental noise.

That matters for:

* comparing repeated runs
* reviewing changes over time
* validating parser behavior
* spotting real differences rather than formatting churn

---

## 16.2 Stable Ordering

The tool should enforce stable ordering for:

* PIDs
* file descriptors
* paths
* socket entries
* findings
* output sections
* bundle layout

Non-deterministic ordering weakens review quality.

---

## 16.3 Stable Representation

Equivalent observations should be represented consistently.

Examples:

* timestamps in one canonical format
* absent values represented consistently
* confidence and severity serialized consistently
* path and identifier formatting rules documented and stable

---

# 17. Bundle Integrity Expectations

## 17.1 Bundle Scope

An evidence bundle should preserve enough material for later review, but it is still only a product of this tool’s observable scope.

A bundle is:

* a deterministic record of collected evidence and outputs
* useful for offline analysis
* not a full disk or memory acquisition
* not a cryptographic proof of total system state

---

## 17.2 Metadata

The bundle should include metadata sufficient to interpret the run.

Useful metadata includes:

* tool version
* run mode
* collection start/end timestamps
* target descriptor
* host/environment hints where appropriate
* warnings/errors encountered
* feature availability state

---

## 17.3 Hashing of Bundle Contents

Hashing bundle artifacts may be useful later, but the project must not pretend that hashing alone creates legal chain of custody.

If artifact hashing is added, it should be described accurately:

* integrity aid
* comparison aid
* not legal certification

---

# 18. Helper Command Discipline

## 18.1 Why This Needs Rules

Some evidence sources may be easier or more portable to access through bounded helper commands, such as `journalctl`.

That convenience must not damage forensic clarity.

---

## 18.2 Requirements for Helper Commands

When helper commands are used:

* the command path or invocation basis should be known
* the command must be read-only in effect
* failure must be recorded explicitly
* collected output must be attributed to that helper
* parsing must preserve the distinction between helper output and kernel-native raw interfaces

---

## 18.3 Helper Output Is Still Evidence, Not Gospel

Helper output can vary by distribution, version, locale, and formatting assumptions.

Treat it as a source with boundaries, not as infallible truth.

---

# 19. Reporting Rules

## 19.1 Separate Facts from Findings

Reports should have a clear distinction between:

* observed facts
* evidence caveats
* derived findings

Bad reporting mixes them into one undifferentiated paragraph.

Good reporting presents them as different semantic layers.

---

## 19.2 Phrase Derived Statements Carefully

Preferred language includes:

* “observed”
* “resolved”
* “captured”
* “could not be collected”
* “consistent with”
* “suggests”
* “partial”

Avoid inflated or cinematic language.

---

## 19.3 Precision Over Drama

The tool should sound precise, not theatrical.

Bad:

* “ominous deleted binary lurking in memory”

Good:

* “one executable mapping referenced a deleted file during collection”

The second phrasing is useful. The first is noise.

---

# 20. Review Rules for New Evidence Sources

Any new evidence source added to the project should be evaluated against these questions:

1. What exactly is the source?
2. Is it raw, helper-mediated, or already transformed?
3. How stable is it during live collection?
4. What permissions affect it?
5. What caveats are common?
6. What claims can reasonably be derived from it?
7. What claims would overreach?
8. Should raw artifacts be preserved?
9. How should confidence be assigned?
10. How should absence or restriction be represented?

If these questions are not answered, the integration is not ready.

---

# 21. Anti-Patterns

The following are specifically disallowed in the project’s forensic posture.

## 21.1 Overclaiming

Bad:

* “system compromised”
* “attacker persistence detected”
* “malicious file injection confirmed”

unless the evidence truly and directly supports such claims, which is outside the normal scope of this tool.

---

## 21.2 Hiding Missing Data

Bad:

* silently omitting unavailable sources
* pretending a partial parse was complete
* presenting unmatched socket data as if ownership were established

---

## 21.3 Treating Live-State Inconsistency as Proof

Bad:

* assuming a disappeared proc entry implies tampering
* assuming incomplete fd enumeration implies stealth behavior
* assuming namespace divergence implies compromise

There may be reasons to investigate further, but the tool itself must stay disciplined.

---

## 21.4 Blending Observation and Interpretation

Bad:

* “suspicious process path” without criteria
* “weird network behavior” without observed specifics
* “likely attacker toolkit” without strong evidence

State the observation first. Then state any constrained interpretation separately.

---

# 22. Minimum Forensic Standard

A valid `autopsyctl` result should meet this minimum standard:

* target identity was established sufficiently for the requested mode
* evidence sources are attributed
* partial evidence is marked
* unavailable evidence is marked
* findings are distinguished from observations
* confidence is explicit or derivable
* severity is explicit where relevant
* output is deterministic
* the tool’s claims remain within its actual scope

If these conditions are not met, the output is not trustworthy enough.

---

# 23. Final Position

`autopsyctl` is not a truth machine. It is a disciplined observer.

Its job is to:

* collect what can be observed
* preserve what matters
* state what was actually seen
* distinguish what was inferred
* expose what was missing
* avoid claims it cannot support

