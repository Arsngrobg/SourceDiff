#include "sdcommons.h"

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define WORDSIZE    sizeof(void*)
#define ALIGN(size) (((size) + WORDSIZE - 1) & ~(WORDSIZE - 1))
#define MINSIZE     (sizeof(sdmemory_t) + sizeof(sdmemory_block_t))

sdmemory_t* sdalloc_create(uint64_t size) {
    assert(size != 0);

    sdmemory_t *zone;
    const uint64_t aligned = ALIGN(size);
    if (!(zone = malloc(MINSIZE + aligned))) {
        ERRLOG("Unable to allocate %lld bytes for the memory zone", MINSIZE + aligned);
        return NULL;
    }
    DLOG("Allocated %lld total bytes for sdmemory_t", MINSIZE + aligned);

    zone->size = sizeof(sdmemory_block_t) + aligned;

    sdmemory_block_t *block = (sdmemory_block_t*) zone->bytes;
    block->size = aligned;
    block->free = true;

    return zone;
}

void *sdalloc_malloc(sdmemory_t *mem, uint64_t size) {
    assert(mem != NULL); assert(size != 0);

    size = ALIGN(size);
    if (size > mem->size) {
        ERRLOG("Given size is greater than the allocated bytes of memory zone (%lld > %lld)", size, mem->size);
        return NULL;
    }

    uint64_t offset = 0;
    while (offset != mem->size) {
        sdmemory_block_t *block = (sdmemory_block_t*) (mem->bytes + offset);

        if (block->free && block->size >= size) {
            DLOG("Found available free block of %lld bytes", block->size);

            if (size < block->size) {
                DLOG("Requested size (%lld) is smaller than the block - dividing...", size);

                uint32_t new_block_pos = offset + (sizeof(sdmemory_block_t) + size);
                sdmemory_block_t *new_block = (sdmemory_block_t*) (mem->bytes + new_block_pos);
                new_block->size = block->size - (sizeof(sdmemory_block_t) + size);
                if (new_block->size != 0) {
                    new_block->free = true;
                }

                DLOG("New tail memory block of size: %lld", new_block->size);
                block->size = size;
            }

            block->free = false;
            return block->user;
        }

        offset += sizeof(sdmemory_block_t) + block->size;
    }

    ERRLOG("Unable to allocate %lld bytes of memory in the memory zone", size);
    return NULL;
}

void sdalloc_free(sdmemory_t *mem, void *ptr) {
    assert(mem != NULL); assert(ptr != NULL);

    DLOG("Attempting to free memory at address: %p", ptr);

    uint64_t offset = 0;
    while (offset != mem->size) {
        sdmemory_block_t *block = (sdmemory_block_t*) (mem->bytes + offset);
        if (ptr == block->user) {
            DLOG("Found address in memory zone - freeing...");
            block->free = true;

            while (offset != mem->size) {
                DLOG("Attempting to defragment local blocks...");
                offset += sizeof(sdmemory_block_t) + block->size;
                sdmemory_block_t *_block = (sdmemory_block_t*) (mem->bytes + offset);
                if (!_block->free) break;
                DLOG("Defragmented %lld bytes", sizeof(sdmemory_block_t) + _block->size);
                block->size += sizeof(sdmemory_block_t) + _block->size;
            }

            break;
        }

        offset += sizeof(sdmemory_block_t) + block->size;
    }
}

void sdalloc_delete(sdmemory_t *mem) {
    assert(mem != NULL);
    DLOG("Freed sdmemory_t occupying %lld bytes", sizeof(sdmemory_t) + mem->size);
    free(mem);
}
