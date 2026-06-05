//     ,──.   ,──.,──.    ,─────. ,─────.
//     │  │   `──'│  │─. '  .──./'  .──./
//     │  │   ,──.│ .─. '│  │    │  │
//     │  '──.│  ││ `─' │'  '──'╲'  '──'╲
//     `─────'`──' `───'  `─────' `─────'
//
//     LibCC - lightweight C compiler invocation library

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libcc.h"

#define LIBCC_ARGMAX ((32 << 10) - 1) // 32KB - Windows compatible

#include "libcc_internal.c"

struct CC_Toolchain {
    // Local render cache for the command
    char render[LIBCC_ARGMAX];

    // Invocation
    char *ccid;

    // Arguments
    CC_ArgList *options;
    CC_ArgList *include_paths;
    CC_ArgList *lib_paths;
    CC_ArgList *libs;
    CC_ArgList *sources;
};

CC_Toolchain *cc_new(void) {
    CC_Toolchain *cc = malloc(sizeof(CC_Toolchain));
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    cc->ccid          = cc_strdup(CC_COMPILER_DEFAULT);
    cc->options       = cc_arglist_new();
    cc->include_paths = cc_arglist_new();
    cc->lib_paths     = cc_arglist_new();
    cc->libs          = cc_arglist_new();
    cc->sources       = cc_arglist_new();

    if (!(cc->ccid) || !(cc->options) || !(cc->include_paths) || !(cc->lib_paths) || !(cc->libs) || !(cc->sources)) {
        return NULL;
    }

    return cc;
}

void cc_delete(CC_Toolchain *cc) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return;
    }

    free(cc->ccid);
    cc_arglist_delete(cc->options);
    cc_arglist_delete(cc->include_paths);
    cc_arglist_delete(cc->lib_paths);
    cc_arglist_delete(cc->libs);
    cc_arglist_delete(cc->sources);

    free(cc);
}

bool cc_set_compiler(CC_Toolchain *cc, const char *ccid) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }

    free(cc->ccid);
    cc->ccid = cc_strdup(ccid);
    return true;
}

bool cc_add_option(CC_Toolchain *cc, const char *option) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }

    return cc_arglist_push(cc->options, option);
}

bool cc_add_include_path(CC_Toolchain *cc, const char *fmt, ...) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }
    if (fmt == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m fmt cannot be NULL\n");
        return false;
    }

    va_list vargs;
    va_start(vargs, fmt);
    bool encoded = cc_arglist_pushf(cc->include_paths, fmt, vargs);
    va_end(vargs);

    return encoded;
}

bool cc_add_library_path(CC_Toolchain *cc, const char *fmt, ...) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }
    if (fmt == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m fmt cannot be NULL\n");
        return false;
    }

    va_list vargs;
    va_start(vargs, fmt);
    bool encoded = cc_arglist_pushf(cc->lib_paths, fmt, vargs);
    va_end(vargs);

    return encoded;
}

bool cc_add_library(CC_Toolchain *cc, const char *fmt, ...) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }
    if (fmt == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m fmt cannot be NULL\n");
        return false;
    }

    va_list vargs;
    va_start(vargs, fmt);
    bool encoded = cc_arglist_pushf(cc->libs, fmt, vargs);
    va_end(vargs);

    return encoded;
}

bool cc_set_output(CC_Toolchain *cc, const char *fmt, ...) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }
    if (fmt == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m fmt cannot be NULL\n");
        return false;
    }

    va_list vargs;
    va_start(vargs, fmt);
    bool encoded = cc_add_option(cc, "-o") && cc_arglist_pushf(cc->options, fmt, vargs);
    va_end(vargs);

    return encoded;
}

bool cc_add_source(CC_Toolchain *cc, const char *fmt, ...) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }
    if (fmt == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m fmt cannot be NULL\n");
        return false;
    }

    va_list vargs;
    va_start(vargs, fmt);
    bool encoded = cc_arglist_pushf(cc->sources, fmt, vargs);
    va_end(vargs);

    return encoded;
}

const char *cc_render_command(CC_Toolchain *cc) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }

    // TODO: this can error if, for some reason, you have a command that is larger than 32KB
    size_t len = 0;

    // ccid
    size_t _len = strlen(cc->ccid);
    while (len < _len) {
        cc->render[len] = cc->ccid[len];
        len++;
    }

    // options
    for (size_t opt = 0; opt < cc->options->size; opt++) {
        cc->render[len++] = ' ';
        size_t _len = strlen(cc->options->args[opt]);
        for (size_t idx = 0; idx < _len; idx++) {
            cc->render[len++] = cc->options->args[opt][idx];
        }
    }

    // include paths
    for (size_t inc = 0; inc < cc->include_paths->size; inc++) {
        cc->render[len++] = ' ';
        cc->render[len++] = '-';
        cc->render[len++] = 'I';

        cc->render[len++] = '"';
        size_t _len = strlen(cc->include_paths->args[inc]);
        for (size_t idx = 0; idx < _len; idx++) {
            cc->render[len++] = cc->include_paths->args[inc][idx];
        }
        cc->render[len++] = '"';
    }

    // lib paths
    for (size_t libp = 0; libp < cc->lib_paths->size; libp++) {
        cc->render[len++] = ' ';
        cc->render[len++] = '-';
        cc->render[len++] = 'L';

        cc->render[len++] = '"';
        size_t _len = strlen(cc->lib_paths->args[libp]);
        for (size_t idx = 0; idx < _len; idx++) {
            cc->render[len++] = cc->lib_paths->args[libp][idx];
        }
        cc->render[len++] = '"';
    }

    // libs
    for (size_t lib = 0; lib < cc->libs->size; lib++) {
        cc->render[len++] = ' ';
        cc->render[len++] = '-';
        cc->render[len++] = 'l';

        cc->render[len++] = '"';
        size_t _len = strlen(cc->libs->args[lib]);
        for (size_t idx = 0; idx < _len; idx++) {
            cc->render[len++] = cc->libs->args[lib][idx];
        }
        cc->render[len++] = '"';
    }

    // sources
    for (size_t src = 0; src < cc->sources->size; src++) {
        cc->render[len++] = ' ';

        cc->render[len++] = '"';
        size_t _len = strlen(cc->sources->args[src]);
        for (size_t idx = 0; idx < _len; idx++) {
            cc->render[len++] = cc->sources->args[src][idx];
        }
        cc->render[len++] = '"';
    }

    cc->render[len++] = '\0';
    return cc->render;
}

int32_t cc_invoke(CC_Toolchain *cc) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }

    return system(cc_render_command(cc));
}

bool cc_set_output_type(CC_Toolchain *cc, CC_OutputType out_type) {
    if (cc == NULL) {
        fprintf(stderr, "libcc: \x1b[1;31merror:\x1b[0m CC_Toolchain cannot be NULL\n");
        return false;
    }

    switch (out_type) {
        case CC_OUTPUT_PREPROCESSED:
            return cc_add_option(cc, "-E");
        case CC_OUTPUT_ASSEMBLY:
            return cc_add_option(cc, "-S");
        case CC_OUTPUT_OBJECT:
            return cc_add_option(cc, "-c");
        case CC_OUTPUT_INTERMEDIATES:
            return cc_add_option(cc, "-save-temps");
        case CC_OUTPUT_SHARED:
            return cc_add_option(cc, CC_SHARED_FLAG);
        case CC_OUTPUT_EXE:
            return true;
        default:
            return false;
    }
}
