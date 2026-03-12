# Skill: git-commit-split

## Purpose

Transform a messy working tree or large diff into a clean series of logical commits suitable for review and long-term maintenance.

Use this when changes include multiple types such as features, refactors, tests, and documentation in a single diff.

## Inputs

- git status
- git diff
- changed file list
- related specification or issue (optional)

## Outputs

- recommended commit grouping
- suggested staging strategy
- commit messages
- history cleanup recommendations

## Procedure

1. Inspect Working Tree

Run:

git status
git diff

List all modified files.

2. Classify Changes

Categorize each change as:

feature
bugfix
refactor
test
documentation
build

3. Identify Logical Commits

Group files or partial file changes into logical units that represent:

- one feature
- one bug fix
- one refactor
- one test addition
- one documentation change

4. Detect Mixed Files

If a file contains multiple logical changes:

recommend using:

git add -p

Split hunks into separate commits.

5. Verify Commit Integrity

Each commit must:

- compile successfully
- not break tests
- represent one logical change

6. Generate Commit Messages

Use format:

type(scope): short summary

Motivation:
...

Changes:
...

Types:

feat
fix
refactor
test
docs
build

## Rules

- avoid commits mixing refactors with features
- avoid committing formatting changes with logic changes
- ensure commits are independently understandable
- prefer small coherent commits over large ones

## Output Format

COMMIT SPLIT PLAN

Commit 1
Type:
Files:
Description:

Commit 2
Type:
Files:
Description:

Staging Strategy

git add -p instructions

Commit Messages

<commit message list>