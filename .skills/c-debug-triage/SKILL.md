# Skill: c-debug-triage

## Purpose

Diagnose failures in C programs using systematic debugging.

Focus areas:

- crashes
- undefined behavior
- sanitizer reports
- incorrect output
- failing tests

## Inputs

Any combination of:

- compiler warnings
- crash logs
- sanitizer output
- stack traces
- failing tests
- minimal code snippets
- reproduction steps

## Outputs

- ranked root-cause hypotheses
- debugging steps
- code inspection targets
- minimal patch suggestions

## Procedure

### 1. Categorize the Failure

Classify:

- compile error
- runtime crash
- memory error
- logic error
- race condition

### 2. Analyze Compiler Warnings

Review warnings such as:

- unused variables
- implicit conversions
- uninitialized variables
- format string mismatches

Treat warnings as potential root causes.

### 3. Check for Undefined Behavior

Look for:

- signed overflow
- out-of-bounds access
- use-after-free
- double free
- uninitialized memory
- strict aliasing violations

Mark suspicious code.

### 4. Memory Lifetime Analysis

Inspect:

- malloc/free pairing
- stack vs heap pointers
- ownership expectations
- pointer escaping scope

Identify lifetime violations.

### 5. Pointer Safety Checks

Verify:

- null dereference
- incorrect pointer arithmetic
- incorrect struct casting
- invalid pointer reuse

### 6. Return Value Verification

Check calls to:

- malloc
- fopen
- read/write
- system calls

Ensure return values are validated.

### 7. Error Propagation

Determine whether:

- errors are ignored
- error codes overwritten
- error messages suppressed

### 8. Concurrency Risks

If multithreaded check:

- unsynchronized shared state
- missing locks
- incorrect atomic usage
- ordering bugs

### 9. Stack Trace Analysis

Trace crash path:

- locate failing frame
- inspect function inputs
- locate corrupted data

### 10. Generate Hypotheses

Produce ranked causes:

1. most likely
2. possible
3. less likely

Each must include supporting evidence.

### 11. Suggest Debugging Steps

Examples:

- enable AddressSanitizer
- run under Valgrind
- add assertions
- isolate minimal reproducer

### 12. Suggest Minimal Fixes

Provide targeted patches when cause is clear.

Avoid speculative rewrites.

## Rules

- do not guess without evidence
- prefer log or code evidence
- assume undefined behavior until ruled out
- avoid large refactor suggestions

## Output Format

DEBUG TRIAGE REPORT

Failure Type

Evidence

Ranked Hypotheses

Code Areas to Inspect

Recommended Debugging Steps

Minimal Patch Suggestions