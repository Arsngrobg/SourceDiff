//     ,──.   ,──.,──.    ,─────. ,─────.
//     │  │   `──'│  │─. '  .──./'  .──./
//     │  │   ,──.│ .─. '│  │    │  │
//     │  '──.│  ││ `─' │'  '──'╲'  '──'╲
//     `─────'`──' `───'  `─────' `─────'
//
//     LibCC - lightweight C compiler invocation library

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CC_ARGLIST_CAPACITY0    2    // no need for checking "stuck-at-one"
#define CC_ARGLIST_LOADFACTOR   0.75 // based off Java's ArrayList
#define CC_ARGLIST_GROWTHFACTOR 1.50 // based off Java's ArrayList

typedef struct {
    size_t   capacity; // the true size of the supporting array
    size_t   size;     // the 'local' size of the list
    char   **args;     // the supporting array
} CC_ArgList;

CC_ArgList *cc_arglist_new   (void);
void        cc_arglist_delete(CC_ArgList *list);
bool        cc_arglist_push  (CC_ArgList *list, const char *arg);
bool        cc_arglist_pushf (CC_ArgList *list, const char *fmt, va_list vargs);
char       *cc_strdup        (const char *s);

CC_ArgList *cc_arglist_new(void) {
    CC_ArgList *list = malloc(sizeof(CC_ArgList));
    if (list == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    list->capacity = CC_ARGLIST_CAPACITY0;
    list->size     = 0;
    list->args     = malloc(sizeof(char*) * CC_ARGLIST_CAPACITY0);
    if (list->args == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    return list;
}

void cc_arglist_delete(CC_ArgList *list) {
    assert(list != NULL);

    for (size_t idx = 0; idx < list->size; idx++) {
        free(list->args[idx]);
    }

    free(list->args);
}

bool cc_arglist_push(CC_ArgList *list, const char *arg) {
    assert(list != NULL); assert(arg != NULL);

    float load = list->size / list->capacity;
    if (load >= CC_ARGLIST_LOADFACTOR) {
        size_t new_capacity = list->capacity * CC_ARGLIST_GROWTHFACTOR;
        list->args = realloc(list->args, sizeof(char*) * new_capacity);
        if (list->args == NULL) {
            fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
            return false;
        }
        list->capacity = new_capacity;
    }

    list->args[list->size++] = cc_strdup(arg);
    return true;
}

bool cc_arglist_pushf(CC_ArgList *list, const char *fmt, va_list vargs) {
    assert(list != NULL); assert(fmt != NULL);

    char buf[LIBCC_ARGMAX];
    if (vsnprintf(buf, LIBCC_ARGMAX, fmt, vargs) < 0) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m string formatting error\n");
        return false;
    }

    float load = list->size / list->capacity;
    if (load >= CC_ARGLIST_LOADFACTOR) {
        size_t new_capacity = list->capacity * CC_ARGLIST_GROWTHFACTOR;
        list->args = realloc(list->args, sizeof(char*) * new_capacity);
        if (list->args == NULL) {
            fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
            return false;
        }
        list->capacity = new_capacity;
    }

    list->args[list->size++] = cc_strdup(buf);

    return true;
}

char *cc_strdup(const char *s) {
    assert(s != NULL);

    size_t len = strlen(s) + 1;
    char  *cpy = malloc(sizeof(char) * len);
    if (cpy == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    memcpy(cpy, s, sizeof(char) * len);
    return cpy;
}
