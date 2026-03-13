#ifndef POC_H
#define POC_H

#include <sys/types.h>

enum collect_result {
    COLLECT_OK = 0,
    COLLECT_INVALID = 1,
    COLLECT_UNAVAILABLE = 2,
    COLLECT_PARSE_ERROR = 3,
};

struct proc_status {
    pid_t pid;
};

struct collect_ctx {
    int dummy;
};

enum collect_result collect_proc_status(
    struct collect_ctx *ctx,
    pid_t pid,
    struct proc_status *out
);

#endif
