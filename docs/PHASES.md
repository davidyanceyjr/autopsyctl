# PHASES.md

Canonical Function Patterns for Phase-Oriented C

This document defines the standard phase layout patterns used in
Phase-Oriented C projects.

These patterns are intended to make functions:

-   easy for humans to audit
-   predictable in behavior
-   safe in resource handling
-   compatible with air-gapped operational environments
-   consistent across contributors and AI-generated code

The goal is not rigid ceremony, but clear operational structure.

------------------------------------------------------------------------

# 1. Phase Philosophy

A phase is a named operational step in a function.

Phases serve as navigational anchors for reviewers.

A reviewer should be able to read only the phase labels and understand:

-   what the function does
-   what order operations occur
-   where failures may happen
-   where cleanup occurs

Example mental flow:

validate_input initialize_state acquire_resources read_inputs
transform_state check_invariants emit_results cleanup

Not every function needs every phase.

------------------------------------------------------------------------

# 2. Canonical Phase Order

Recommended phase order:

validate_input initialize_state acquire_resources read_inputs
transform_state check_invariants emit_results cleanup

Only include phases that are meaningful.

Example minimal function:

validate_input transform_state cleanup

------------------------------------------------------------------------

# 3. Required Structural Rules

## Rule 1 --- Single cleanup region

Resources should be released in one cleanup phase whenever possible.

Example:

``` c
cleanup:
    if (fd >= 0)
        close(fd);

    free(buffer);

    return rc;
```

------------------------------------------------------------------------

## Rule 2 --- Monotonic failure flow

Failures should only move forward toward cleanup.

Never jump backward after a failure.

Correct pattern:

phase phase failure -\> cleanup

Incorrect pattern:

failure -\> earlier phase

------------------------------------------------------------------------

## Rule 3 --- Initialize outputs early

Output structures should be initialized as soon as possible.

Example:

``` c
initialize_state:
    *out = (struct proc_status){0};
```

------------------------------------------------------------------------

## Rule 4 --- Explicit success

Success should be explicitly assigned before cleanup.

Example:

``` c
emit_results:
    rc = 0;
    goto cleanup;
```

Avoid implicit success from previous operations.

------------------------------------------------------------------------

## Rule 5 --- Sentinel resource values

All resources must use sentinel initialization.

Example:

``` c
int fd = -1;
char *buffer = NULL;
DIR *dir = NULL;
```

Cleanup can then safely check:

``` c
if (fd >= 0)
    close(fd);
```

------------------------------------------------------------------------

# 4. Minimal Function Pattern

Small helper functions may use a simplified structure.

Example:

``` c
int parse_pid(const char *str, pid_t *out)
{
    int rc = -1;

validate_input:
    if (!str || !out)
        goto cleanup;

transform_state:
    *out = atoi(str);

emit_results:
    rc = 0;

cleanup:
    return rc;
}
```

------------------------------------------------------------------------

# 5. Standard Resource Pattern

Functions that acquire resources should follow this pattern.

Example:

``` c
int read_file(const char *path, char **out)
{
    int rc = -1;
    int fd = -1;
    char *buffer = NULL;

validate_input:
    if (!path || !out)
        goto cleanup;

initialize_state:
    *out = NULL;

acquire_resources:
    fd = open(path, O_RDONLY);
    if (fd < 0)
        goto cleanup;

read_inputs:
    buffer = malloc(4096);
    if (!buffer)
        goto cleanup;

    read(fd, buffer, 4096);

emit_results:
    *out = buffer;
    buffer = NULL;
    rc = 0;

cleanup:
    if (fd >= 0)
        close(fd);

    free(buffer);

    return rc;
}
```

Key principles demonstrated:

-   sentinel resources
-   centralized cleanup
-   ownership transfer

------------------------------------------------------------------------

# 6. Multi-Resource Cleanup Pattern

When many resources exist, cleanup may be layered.

Example:

``` c
cleanup_files:
    if (fd >= 0)
        close(fd);

cleanup_memory:
    free(buffer);

cleanup:
    return rc;
```

Layered cleanup keeps code readable in complex functions.

------------------------------------------------------------------------

# 7. Loop Usage Policy

Loops are allowed, but should appear in narrow traversal functions.

Good loop locations:

-   parsers
-   directory traversal
-   buffer scanning
-   table iteration

Example:

``` c
while ((line = read_line(fd))) {
    parse_status_line(line, out);
}
```

Avoid placing large loops in top-level orchestration functions.

Instead extract helpers.

------------------------------------------------------------------------

# 8. Phase Naming Guidelines

Phase labels should describe intent.

Good labels:

validate_input read_proc_status decode_capabilities emit_json_summary
release_resources

Bad labels:

step1 l1 again work

A reviewer should understand the function by reading only the labels.

------------------------------------------------------------------------

# 9. Example Canonical Function

``` c
int collect_proc_status(struct collect_ctx *ctx, pid_t pid, struct proc_status *out)
{
    int rc = -1;
    int proc_fd = -1;

validate_input:
    if (!ctx || !out || pid <= 0)
        goto cleanup;

initialize_state:
    *out = (struct proc_status){0};

acquire_resources:
    proc_fd = open_pid_dir(pid);
    if (proc_fd < 0)
        goto cleanup;

read_inputs:
    rc = read_status_file(proc_fd, out);
    if (rc != 0)
        goto cleanup;

check_invariants:
    rc = validate_proc_status(out);
    if (rc != 0)
        goto cleanup;

emit_results:
    rc = 0;

cleanup:
    if (proc_fd >= 0)
        close(proc_fd);

    return rc;
}
```

------------------------------------------------------------------------

# 10. When NOT to Use Phases

Phases are unnecessary for extremely small functions.

Example:

``` c
static inline int is_valid_pid(pid_t pid)
{
    return pid > 0;
}
```

Over-structuring trivial functions reduces readability.

Use phases only when they improve reasoning.

------------------------------------------------------------------------

# 11. Review Checklist

When reviewing a Phase-Oriented function ask:

1.  Are phases clearly named?
2.  Is cleanup centralized?
3.  Are resources initialized with sentinel values?
4.  Is failure flow monotonic?
5.  Is success explicit?
6.  Are loops isolated to traversal helpers?
7.  Are invariants documented when needed?

If the answers are yes, the function is likely safe and readable.

------------------------------------------------------------------------

# 12. Summary

Phase-Oriented C aims to produce functions that:

-   tell a clear operational story
-   avoid hidden control flow
-   prevent resource leaks
-   remain easy for humans to audit
-   remain friendly to AI-assisted generation and review

The structure is simple:

declare phase phase phase cleanup return

That simplicity is intentional.
