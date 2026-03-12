# Skill: repo-structure-review

## Purpose

Evaluate repository layout for maintainability and clarity.

## Inputs

- repository directory tree
- build configuration
- test directories
- documentation directories

## Outputs

- structural issues
- missing directories
- recommended layout

## Procedure

1. Inspect Repository Tree

Review top-level structure.

2. Detect Mixed Responsibilities

Check for:

tests inside source directories
build artifacts committed
multiple languages mixed incorrectly

3. Check Required Directories

Verify presence of:

src/
tests/
docs/
scripts/
specs/

4. Evaluate Build Output

Ensure compiled artifacts are excluded.

5. Suggest Improved Layout

Recommend clear separation of:

source
tests
documentation
scripts

## Rules

- prioritize clarity
- avoid deeply nested directory trees
- separate generated files from source

## Output Format

REPOSITORY STRUCTURE REPORT

Current Layout Summary

Detected Problems

Missing Directories

Recommended Layout