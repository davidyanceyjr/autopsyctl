# Skill: api-change-review

## Purpose

Detect changes that affect public APIs and assess compatibility risk.

## Inputs

- public headers
- exported functions
- git diff
- specifications

## Outputs

- detected API changes
- compatibility impact
- migration guidance

## Procedure

1. Identify Public Interfaces

List exported headers and functions.

2. Inspect Diff

Check for:

function signature changes
struct layout changes
removed symbols
new parameters

3. Evaluate Compatibility

Determine whether changes break:

source compatibility
binary compatibility

4. Identify Behavioral Changes

Check whether semantics of functions changed.

5. Suggest Migration Strategy

Recommend updates needed by users.

## Rules

- treat public interfaces as stable contracts
- clearly distinguish breaking vs non-breaking changes

## Output Format

API CHANGE REPORT

Detected Changes

Compatibility Impact

Breaking Changes

Migration Recommendations