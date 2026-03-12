# Skill: ci-failure-triage

## Purpose

Diagnose continuous integration failures quickly.

## Inputs

- CI logs
- failing tests
- build output
- recent commits

## Outputs

- likely failure causes
- reproduction instructions
- fix suggestions

## Procedure

1. Identify Failure Stage

Determine where pipeline failed:

build
test
deployment
lint

2. Extract Error Messages

Locate the earliest failure message.

3. Compare Local vs CI Environment

Check differences in:

operating system
dependency versions
environment variables

4. Inspect Recent Changes

Analyze commits that may affect the failing stage.

5. Attempt Local Reproduction

Provide steps to reproduce the failure locally.

6. Recommend Fix

Suggest minimal changes required.

## Rules

- prioritize earliest error
- avoid chasing secondary failures
- consider environment differences first

## Output Format

CI FAILURE REPORT

Failure Stage

Error Summary

Likely Causes

Reproduction Steps

Recommended Fix