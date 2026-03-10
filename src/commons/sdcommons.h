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

// types
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;
typedef double   f64;
typedef float    f32;

// sdalloc.c
#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

typedef struct {
    bool free;
    u64  size;
    u8   user[];
} sdallocated_t;

typedef struct {
    u64 size;
    u8  bytes[]; // mapped to sdallocated_t
} sdallocator_t;

extern sdallocator_t *sdalloc_create(uint64_t size);
extern void          *sdalloc_malloc(sdallocator_t *aloc, uint64_t size);
extern void           sdalloc_free  (sdallocator_t *aloc, void *ptr);
extern void           sdalloc_delete(sdallocator_t *aloc);

// sdfile.c
extern const char *sdfiles_fmt   (char *buf, const char *fmt, ...);
extern bool        sdfiles_exists(char *buf, const char *pahf, ...);

#endif // SRCDIFF_COMMONS_H_
