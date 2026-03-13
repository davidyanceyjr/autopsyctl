#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "poc.h"

enum collect_result collect_proc_status(
    struct collect_ctx *ctx,
    pid_t pid,
    struct proc_status *out
)
{
    char proc_path[64];
    char status_buf[4096];
    char *cursor = NULL;
    char *line_end = NULL;
    char *value = NULL;
    char *parse_end = NULL;
    int fd = -1;
    enum collect_result rc = COLLECT_INVALID;
    long parsed_pid = 0;
    size_t total_read = 0;
    ssize_t chunk_size;
    int path_length;

    goto validate_input;

validate_input:
    if (!ctx || !out || pid <= 0) {
        goto cleanup;
    }

    goto initialize_state;

initialize_state:
    out->pid = 0;

    goto build_path;

build_path:
    path_length = snprintf(proc_path, sizeof(proc_path), "/proc/%ld/status", (long)pid);
    if (path_length < 0 || (size_t)path_length >= sizeof(proc_path)) {
        rc = COLLECT_PARSE_ERROR;
        goto cleanup;
    }

    goto acquire_resources;

acquire_resources:
    fd = open(proc_path, O_RDONLY);
    if (fd < 0) {
        rc = COLLECT_UNAVAILABLE;
        goto cleanup;
    }

    goto read_inputs;

read_inputs:
    while (total_read + 1 < sizeof(status_buf)) {
        chunk_size = read(fd, status_buf + total_read, (sizeof(status_buf) - 1) - total_read);
        if (chunk_size < 0) {
            rc = COLLECT_UNAVAILABLE;
            goto cleanup;
        }

        if (chunk_size == 0) {
            break;
        }

        total_read += (size_t)chunk_size;
    }

    if (total_read + 1 == sizeof(status_buf)) {
        chunk_size = read(fd, status_buf, 1);
        if (chunk_size < 0) {
            rc = COLLECT_UNAVAILABLE;
            goto cleanup;
        }

        if (chunk_size > 0) {
            rc = COLLECT_PARSE_ERROR;
            goto cleanup;
        }
    }

    status_buf[total_read] = '\0';

    goto parse_status;

parse_status:
    cursor = status_buf;
    while (*cursor != '\0') {
        line_end = strchr(cursor, '\n');
        if (line_end != NULL) {
            *line_end = '\0';
        }

        if (strncmp(cursor, "Pid:", 4) == 0) {
            value = cursor + 4;
            while (*value == ' ' || *value == '\t') {
                value++;
            }

            if (*value == '\0') {
                rc = COLLECT_PARSE_ERROR;
                goto cleanup;
            }

            errno = 0;
            parsed_pid = strtol(value, &parse_end, 10);
            if (parse_end == value || errno == ERANGE) {
                rc = COLLECT_PARSE_ERROR;
                goto cleanup;
            }

            while (*parse_end == ' ' || *parse_end == '\t') {
                parse_end++;
            }

            if (*parse_end != '\0') {
                rc = COLLECT_PARSE_ERROR;
                goto cleanup;
            }

            if (parsed_pid != (long)((pid_t)parsed_pid)) {
                rc = COLLECT_PARSE_ERROR;
                goto cleanup;
            }

            out->pid = (pid_t)parsed_pid;
            goto check_invariants;
        }

        if (line_end == NULL) {
            break;
        }

        cursor = line_end + 1;
    }

    rc = COLLECT_PARSE_ERROR;
    goto cleanup;

check_invariants:
    if (parsed_pid <= 0 || (pid_t)parsed_pid != pid) {
        rc = COLLECT_PARSE_ERROR;
        goto cleanup;
    }

    goto emit_results;

emit_results:
    rc = COLLECT_OK;

cleanup:
    if (fd >= 0) {
        (void)close(fd);
    }

    return rc;
}
