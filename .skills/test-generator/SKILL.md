# Skill: test-generator

## Purpose

Generate comprehensive tests from specifications or code emphasizing correctness and robustness.

Applicable to libraries, modules, and CLI tools.

## Inputs

One or more:

- specification file
- function signature
- module source code
- CLI documentation
- expected behavior description

## Outputs

- unit tests
- edge case tests
- failure path tests
- fuzz/property testing ideas
- bats tests for CLI tools
- regression test suggestions

## Procedure

### 1. Identify Test Targets

Extract:

- public functions
- CLI commands
- exported modules
- observable system behaviors

List each testable unit.

### 2. Determine Expected Behavior

Identify:

- return values
- error conditions
- side effects
- state transitions

Record expected outcomes.

### 3. Boundary Condition Analysis

Check boundaries such as:

- zero values
- minimum values
- maximum values
- buffer limits
- array sizes
- integer overflow limits

Generate tests for each boundary.

### 4. Malformed Input Tests

Generate tests for:

- null pointers
- invalid formats
- truncated input
- corrupted data
- unexpected flags

Verify failure behavior.

### 5. Failure Path Tests

Simulate failures such as:

- I/O errors
- allocation failure
- dependency failure
- invalid state

Ensure errors propagate correctly.

### 6. Edge Case Tests

Include cases such as:

- empty input
- duplicate input
- extremely large input
- repeated operations
- partially completed operations

### 7. Concurrency / Timing Tests

If concurrency exists:

Check for:

- race conditions
- ordering issues
- shared state corruption

Propose stress tests if appropriate.

### 8. CLI Tests (BATS)

Generate bats tests verifying:

- correct output
- exit codes
- stderr messages
- invalid flags
- help output

### 9. Fuzz / Property Testing Ideas

Targets:

- parsers
- decoders
- protocol handlers

Properties:

- deterministic output
- no crashes
- no memory corruption

### 10. Regression Test Suggestions

Identify fragile logic:

- boundary handling
- parsing logic
- error propagation

Recommend permanent tests.

## Rules

- prioritize robustness over coverage metrics
- every test must define expected results
- avoid redundant tests
- prefer minimal reproducible tests

## Output Format

TEST PLAN

1. Test Targets
2. Unit Tests
3. Boundary Tests
4. Malformed Input Tests
5. Failure Path Tests
6. Edge Case Tests
7. Concurrency Tests
8. CLI BATS Tests
9. Fuzz / Property Tests
10. Regression Tests