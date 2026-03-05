#ifndef SRCDIFF_COMMONS_H_
#define SRCDIFF_COMMONS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// logging
#ifndef NDEBUG
#define DLOG(fmt, ...) \
    printf("["__FILE__":%s] "fmt"\n", __func__, ##__VA_ARGS__)
#else
#define DLOG(fmt, ...)
#endif // NDEBUG
#define LOG(fmt, ...) \
    printf("[SourceDiff] "fmt"\n", ##__VA_ARGS__)
#define ERRLOG(fmt, ...) \
        fprintf(stderr, "[SourceDiff] "fmt"\n", ##__VA_ARGS__)

// sdalloc.c
#define KiB(n) ((uint64_t)(n) << 10)
#define MiB(n) ((uint64_t)(n) << 20)
#define GiB(n) ((uint64_t)(n) << 30)

typedef struct {
    bool     free;
    uint64_t size;
    uint8_t  user[];
} sdmemory_block_t;

typedef struct {
    uint64_t size;
    uint8_t  bytes[]; // mapped to sdmemory_block_t
} sdmemory_t;

extern sdmemory_t *sdalloc_create(uint64_t size);
extern void       *sdalloc_malloc(sdmemory_t *mem, uint64_t size);
extern void        sdalloc_free  (sdmemory_t *mem, void *ptr);
extern void        sdalloc_delete(sdmemory_t *mem);

// sdfiles.c
extern const char *sdfiles_lpath(void);
extern const char *sdfiles_pathj(sdmemory_t *mem, const char *pahf, ...);

#endif // SRCDIFF_COMMONS_H_
