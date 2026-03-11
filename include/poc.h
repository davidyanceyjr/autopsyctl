#ifndef POC_H
#define POC_H

#include <sys/types.h>

struct proc_status {
    pid_t pid;
};

struct collect_ctx {
    int dummy;
};

int collect_proc_status(struct collect_ctx *ctx, pid_t pid, struct proc_status *out);

#endif
