# Skill: git-history-audit

## Purpose

Audit repository history for structural and quality problems.

This skill helps detect unhealthy patterns that accumulate in long-lived repositories.

## Inputs

- git log
- git shortlog
- repository tree
- commit statistics
- file size information

## Outputs

- history quality assessment
- detected problems
- cleanup recommendations

## Procedure

1. Inspect Commit History

Review:

git log --stat
git shortlog -sn

Check commit frequency and size.

2. Detect Oversized Commits

Flag commits that:

- modify many unrelated files
- mix refactors with features
- introduce large binary files

3. Identify Generated Files

Check for committed:

- build artifacts
- compiled binaries
- dependency directories
- temporary files

4. Detect Repeated Reverts

Look for patterns such as:

- revert commits
- bug fix followed by revert
- repeated fixes to same area

5. Check Test Presence

Determine whether major features were committed without corresponding tests.

6. Detect Repository Hygiene Issues

Examples:

node_modules committed
vendor directories committed
build output committed
large data files committed

7. Evaluate Commit Messages

Identify messages that lack clarity:

"fix"
"update"
"misc"

Flag weak commit descriptions.

## Rules

- focus on long-term maintainability
- prefer small meaningful commits
- highlight areas where history obscures intent

## Output Format

HISTORY AUDIT REPORT

History Summary

Detected Issues

Oversized Commits
Generated Files
Weak Commit Messages
Repeated Fix Patterns

Recommendations