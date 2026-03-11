#include <stdio.h>
#include "poc.h"

int main(void)
{
    struct collect_ctx ctx = {0};
    struct proc_status st;

    if (collect_proc_status(&ctx, 1, &st) != 0) {
        fprintf(stderr, "collection failed\n");
        return 1;
    }

    printf("Collected pid: %d\n", st.pid);
    return 0;
}
