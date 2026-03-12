# Skill: security-review

## Purpose

Perform a focused security pass on code changes.

## Inputs

- git diff
- input parsing code
- file system access
- network operations

## Outputs

- detected vulnerabilities
- insecure coding patterns
- recommended fixes

## Procedure

1. Identify Input Sources

Check code paths receiving:

user input
file input
network input
environment variables

2. Inspect Memory Handling

Check for:

buffer overflows
unchecked array bounds
unsafe string operations

3. Check Format Strings

Detect unsafe usage of printf-style functions.

4. Validate Path Handling

Check for:

path traversal
insecure temp file creation
unsafe file permissions

5. Inspect Integer Operations

Detect possible integer overflow.

6. Check Error Handling

Ensure failures do not expose sensitive data.

## Rules

- prioritize memory safety
- treat all input as untrusted
- flag any unsafe pattern

## Output Format

SECURITY REVIEW REPORT

Risk Areas

Detected Vulnerabilities

Evidence

Recommended Fixes