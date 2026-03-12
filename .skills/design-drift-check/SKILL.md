# Skill: design-drift-check

## Purpose

Detect divergence between specification and implementation.

## Inputs

- specification files
- source code
- tests

## Outputs

- missing implementations
- undocumented behavior
- outdated specification sections

## Procedure

1. Parse Specification

Extract described behaviors and features.

2. Map Spec Sections to Code

Locate code implementing each behavior.

3. Identify Missing Implementations

Detect spec features with no implementation.

4. Detect Extra Behavior

Find code behavior not described in the spec.

5. Review Tests

Verify that tests align with specification behavior.

6. Flag Spec Drift

Highlight mismatches between design and implementation.

## Rules

- treat specification as authoritative
- flag undocumented behaviors
- encourage spec updates when implementation intentionally changes

## Output Format

DESIGN DRIFT REPORT

Specification Coverage

Missing Implementations

Undocumented Behavior

Outdated Spec Sections

Recommended Updates