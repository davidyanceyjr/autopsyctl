#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "poc.h"

static int find_unavailable_pid(pid_t *out)
{
    pid_t candidate = getpid() + 100000;
    int attempts = 0;

    if (out == NULL) {
        return -1;
    }

    while (attempts < 1024) {
        if (candidate <= 0) {
            candidate++;
            attempts++;
            continue;
        }

        errno = 0;
        if (kill(candidate, 0) < 0 && errno == ESRCH) {
            *out = candidate;
            return 0;
        }

        candidate++;
        attempts++;
    }

    return -1;
}

int main(void)
{
    struct collect_ctx ctx = {0};
    struct proc_status st = {0};
    pid_t unavailable_pid = 0;
    enum collect_result result;

    result = collect_proc_status(NULL, getpid(), &st);
    if (result != COLLECT_INVALID) {
        fprintf(stderr, "expected COLLECT_INVALID for null ctx, got %d\n", result);
        return 1;
    }

    result = collect_proc_status(&ctx, getpid(), NULL);
    if (result != COLLECT_INVALID) {
        fprintf(stderr, "expected COLLECT_INVALID for null out, got %d\n", result);
        return 1;
    }

    result = collect_proc_status(&ctx, 0, &st);
    if (result != COLLECT_INVALID) {
        fprintf(stderr, "expected COLLECT_INVALID for pid 0, got %d\n", result);
        return 1;
    }

    result = collect_proc_status(&ctx, getpid(), &st);
    if (result != COLLECT_OK) {
        fprintf(stderr, "expected COLLECT_OK for live pid, got %d\n", result);
        return 1;
    }

    if (st.pid != getpid()) {
        fprintf(stderr, "expected collected pid %ld, got %ld\n",
            (long)getpid(),
            (long)st.pid);
        return 1;
    }

    if (find_unavailable_pid(&unavailable_pid) != 0) {
        fprintf(stderr, "failed to find unavailable pid for test\n");
        return 1;
    }

    result = collect_proc_status(&ctx, unavailable_pid, &st);
    if (result != COLLECT_UNAVAILABLE) {
        fprintf(stderr, "expected COLLECT_UNAVAILABLE for pid %ld, got %d\n",
            (long)unavailable_pid,
            result);
        return 1;
    }

    return 0;
}
