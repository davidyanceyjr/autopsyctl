# Skill: github-issue-triage

## Purpose

Convert vague or incomplete GitHub issues into actionable engineering tasks.

## Inputs

- issue text
- logs
- stack traces
- screenshots
- user reproduction steps

## Outputs

- clarified problem description
- missing information list
- reproduction steps
- issue classification
- recommended next action

## Procedure

1. Extract Core Problem

Identify:

- what failed
- when it failed
- expected behavior
- observed behavior

2. Identify Missing Information

Check whether issue includes:

- version
- operating system
- reproduction steps
- input data

List missing items.

3. Attempt Reproduction

Determine minimal steps that would reproduce the issue.

Produce a minimal reproduction scenario.

4. Classify Issue

Assign category:

bug
feature request
design flaw
documentation issue
user error

5. Determine Severity

Classify priority:

low
medium
high
critical

6. Recommend Action

Possible actions:

add test
fix implementation
update documentation
update specification
request more information

## Rules

- avoid assumptions
- prioritize reproducible issues
- reduce issue scope when possible

## Output Format

ISSUE TRIAGE REPORT

Problem Summary

Observed Behavior
Expected Behavior

Missing Information

Reproduction Steps

Issue Classification

Recommended Next Action