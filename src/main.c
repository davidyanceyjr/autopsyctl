#include <stdio.h>

#include "poc.h"

int main(void)
{
    struct collect_ctx ctx = {0};
    struct proc_status st;
    enum collect_result result;

    result = collect_proc_status(&ctx, 1, &st);
    switch (result) {
    case COLLECT_OK:
        break;
    case COLLECT_INVALID:
        fprintf(stderr, "collection failed: invalid input\n");
        return 1;
    case COLLECT_UNAVAILABLE:
        fprintf(stderr, "collection failed: target unavailable\n");
        return 1;
    case COLLECT_PARSE_ERROR:
        fprintf(stderr, "collection failed: parse error\n");
        return 1;
    default:
        fprintf(stderr, "collection failed: unexpected result\n");
        return 1;
    }

    printf("Collected pid: %d\n", st.pid);
    return 0;
}
