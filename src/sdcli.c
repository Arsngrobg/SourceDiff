#include "sdcommons.h"
#include <stdio.h>

int main(void) {
    // sdalloc.c
    sdmemory_t *zone;
    if (!(zone = sdalloc_create(64))) {
        return -1;
    }

    // void *ptr1 = sdalloc_malloc(zone, 8);
    // void *ptr2 = sdalloc_malloc(zone, 8);
    // sdalloc_free(zone, ptr1);
    // sdalloc_free(zone, ptr2);

    void *sptr = sdalloc_malloc(zone, 8);
    sdalloc_free(zone, sptr);
    void *bptr = sdalloc_malloc(zone, 16);
    sdalloc_free(zone, bptr);

    sdalloc_delete(zone);
    return 0;
}
