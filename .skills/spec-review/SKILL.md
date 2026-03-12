# Skill: spec-review

## Purpose

Analyze a software specification before implementation to identify ambiguity, missing constraints, hidden assumptions, and potential failure modes.

Use this skill when reviewing a specification located in `specs/*.md` or similar design documents.

The goal is to ensure the specification is precise, implementable, and testable.

## Inputs

One or more of the following:

- specification file (`specs/*.md`)
- design document
- CLI specification
- protocol description
- API contract

Optional:

- related issues
- implementation notes

## Outputs

1. ambiguity list
2. missing constraints
3. hidden assumptions
4. edge cases
5. failure modes
6. concurrency or ordering risks
7. extracted invariants
8. suggested acceptance tests
9. recommended spec improvements

## Procedure

### 1. Identify the System Model

Extract:

- system purpose
- inputs
- outputs
- state transitions
- external dependencies

Summarize the operational model.

### 2. Detect Undefined or Ambiguous Behavior

Search for imprecise language:

- "should"
- "may"
- "generally"
- "normally"
- "fast"
- "large"
- "valid"

Flag:

- unspecified error behavior
- unspecified return values
- unspecified ordering

Record every ambiguous statement.

### 3. Extract Explicit Constraints

Identify:

- input constraints
- size limits
- valid formats
- state requirements
- ordering requirements
- error conditions

Mark missing constraints explicitly.

### 4. Identify Hidden Assumptions

Look for implicit expectations such as:

- filesystem state
- environment variables
- concurrency assumptions
- single-user vs multi-user
- network availability
- architecture assumptions

List every assumption.

### 5. Extract Invariants

Determine properties that must always remain true.

Examples:

- counters never negative
- file handles always closed
- operations idempotent
- deterministic output

List inferred invariants.

### 6. Identify Edge Cases

Consider:

- empty input
- maximum size input
- invalid formats
- duplicate input
- partial state
- interrupted operations

Provide concrete examples.

### 7. Identify Failure Modes

Enumerate failures such as:

- resource exhaustion
- invalid input
- dependency failure
- race conditions
- I/O errors
- corrupted state

Define expected behavior for each.

### 8. Concurrency and Ordering Risks

Check for:

- operation ordering requirements
- concurrent execution issues
- shared resource access
- atomicity assumptions

Flag unclear ordering guarantees.

### 9. Generate Acceptance Tests

Produce executable tests validating:

- normal behavior
- edge cases
- failure handling
- invariants

Example:

Test: invalid flag  
Input: `cli --badflag`  
Expected: exit code 2 and usage message

### 10. Recommend Spec Improvements

Provide concrete improvements:

- clarify vague language
- define limits
- define error behavior
- document invariants

Avoid vague suggestions.

## Rules

- never assume missing behavior
- mark unclear behavior explicitly
- prefer concrete examples
- assume implementer does not know author intent
- avoid implementation suggestions unless necessary for clarity

## Output Format

SPEC REVIEW REPORT

1. System Model
2. Ambiguities
3. Missing Constraints
4. Hidden Assumptions
5. Invariants
6. Edge Cases
7. Failure Modes
8. Concurrency / Ordering Risks
9. Acceptance Test Suggestions
10. Recommended Spec Improvements