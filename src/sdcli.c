#include "sdcommons.h"
#include <stdio.h>

int main(void) {
    // sdalloc.c
    sdmemory_t *zone;
    if (!(zone = sdalloc_create(MiB(1)))) {
        return -1;
    }

    void *ptr1 = sdalloc_malloc(zone, KiB(1));
    void *ptr2 = sdalloc_malloc(zone, KiB(1));
    sdalloc_free(zone, ptr1);
    sdalloc_free(zone, ptr2);

    return 0;
}
