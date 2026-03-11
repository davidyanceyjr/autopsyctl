#include "poc.h"

int collect_proc_status(struct collect_ctx *ctx, pid_t pid, struct proc_status *out)
{
    int rc = -1;

validate_input:
    if (!ctx || !out || pid <= 0) {
        goto cleanup;
    }

initialize_state:
    out->pid = 0;

read_inputs:
    out->pid = pid;

emit_results:
    rc = 0;

cleanup:
    return rc;
}
