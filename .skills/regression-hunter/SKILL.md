# Skill: regression-hunter

## Purpose

Identify the commit that introduced a regression using systematic git bisect analysis.

## Inputs

- failing test
- known good commit
- known bad commit
- reproduction command

## Outputs

- git bisect plan
- automated bisect test command
- suspected commit
- root cause hypothesis

## Procedure

1. Confirm Failure

Verify the failing behavior can be reproduced consistently.

2. Identify Good Commit

Locate commit where behavior was correct.

3. Start Bisect

Run:

git bisect start
git bisect bad HEAD
git bisect good <good-commit>

4. Provide Bisect Test Script

Create command that returns:

0 for success
1 for failure

Example:

./run-tests.sh

5. Run Bisect

Continue until git identifies first bad commit.

6. Analyze Commit

Inspect:

git show <commit>

Determine likely cause.

7. Recommend Fix

Suggest minimal change to restore correct behavior.

## Rules

- ensure bisect test is deterministic
- avoid manual bisect decisions when possible
- confirm regression with final test

## Output Format

REGRESSION ANALYSIS

Good Commit

Bad Commit

Bisect Command

Suspected Commit

Root Cause Hypothesis

Recommended Fix