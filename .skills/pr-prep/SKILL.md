# Skill: pr-prep

## Purpose

Prepare a high-quality GitHub pull request from a git diff.

The goal is to produce a PR that is easy to review, logically organized, and properly documented.

## Inputs

- git diff
- commit history
- related issue
- specification (optional)

## Outputs

- recommended commit split
- commit messages
- PR title
- PR description
- risk assessment
- documentation updates required
- test coverage evaluation

## Procedure

### 1. Analyze the Diff

Identify:

- files modified
- functional changes
- refactors
- documentation updates
- test changes

Classify each modification.

### 2. Identify Logical Change Groups

Group changes into commits based on:

- feature additions
- bug fixes
- refactoring
- tests
- documentation

Each commit must represent one logical change.

### 3. Detect Behavioral Changes

Determine whether the diff changes:

- program output
- API behavior
- CLI interface
- performance characteristics

Record each change.

### 4. Identify Missing Tests

Check whether new behavior has:

- unit tests
- CLI tests
- regression tests

Flag missing coverage.

### 5. Identify Documentation Impact

Check whether updates are needed for:

- README
- CLI help
- API documentation
- specification files

List required updates.

### 6. Generate Commit Messages

Use structure:

<type>: <short summary>

Motivation:
...

Changes:
...

Types:

- feat
- fix
- refactor
- test
- docs

### 7. Generate PR Title

Short and descriptive.

Example:

feat(cli): add recursive delete support

### 8. Write PR Description

Use this structure:

Summary  
Problem  
Solution  
Testing  
Risks  
Follow-ups

### 9. Risk Assessment

Evaluate:

- breaking changes
- migration requirements
- performance impact
- security implications

### 10. Final Review Checklist

Verify:

- code compiles
- tests pass
- documentation updated
- commit history clean

## Rules

- avoid overly large commits
- each commit must be independently understandable
- optimize for reviewer clarity

## Output Format

PR PREPARATION REPORT

Commit Plan

Commit Messages

PR Title

PR Description

Summary  
Problem  
Solution  
Testing  
Risks  
Follow-ups

Test Coverage Evaluation

Documentation Updates Needed