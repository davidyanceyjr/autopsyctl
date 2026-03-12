# Skill: release-prep

## Purpose

Prepare a clean GitHub release including structured release notes and change summaries.

## Inputs

- git log
- commits since last tag
- closed issues
- merged pull requests

## Outputs

- release notes
- change summary
- breaking changes list
- recommended tag

## Procedure

1. Identify Previous Release

Find last tag:

git describe --tags

2. Collect Commits Since Last Tag

List commits:

git log <last-tag>..HEAD

3. Categorize Changes

Group commits into:

features
bug fixes
refactors
tests
documentation

4. Detect Breaking Changes

Look for:

API signature changes
CLI interface changes
behavior changes

5. Summarize Major Improvements

Identify significant capabilities added since last release.

6. Generate Release Notes

Provide structured summary of all changes.

## Rules

- avoid listing internal refactors unless relevant
- highlight user-visible changes first
- clearly mark breaking changes

## Output Format

RELEASE REPORT

Version

Summary

Features
Bug Fixes
Refactors
Tests
Documentation

Breaking Changes

Upgrade Notes