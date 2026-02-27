#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "libtcc.h"

// TODO: make cross-platform
#ifdef _WIN32
#include "windows.h"
#define FSSTR "\\"
#define FSCHR '\\'
#endif

#ifndef NDEBUG
#define PROJECT_NAME "SourceDiff"
#define LOG(fmt, ...)                                                   \
    do {                                                                \
        printf("["PROJECT_NAME":%s] "fmt"\n", __func__, ##__VA_ARGS__); \
    } while (0)
#define ERRLOG(fmt, ...)                                                         \
    do {                                                                         \
        fprintf(stderr, "["PROJECT_NAME":%s] "fmt"\n", __func__, ##__VA_ARGS__); \
    } while (0);
#else
#define LOG(fmt, ...)
#define ERRLOG(fmt, ...)
#endif
#define path(pahf, ...) _path(pahf, ##__VA_ARGS__, NULL)


/* Gets the language parser directory */
const char *get_ldir(void) {
    static char dir[MAX_PATH];
    static bool cached = false;

    if (!cached) {
        char cwd[MAX_PATH];
#ifdef _WIN32
        int32_t bytes = GetCurrentDirectory(MAX_PATH, cwd);
#endif
        if (bytes == 0 || bytes >= MAX_PATH) {
            ERRLOG("The working directory path is longer than %d or an encoding error has occurred", MAX_PATH);
            return NULL;
        }

        bytes = snprintf(dir, MAX_PATH, "%s"FSSTR"languages", cwd);
        if (bytes == 0 || bytes >= MAX_PATH) {
            ERRLOG("The language directory path is longer than %d or an encoding error has occurred", MAX_PATH);
            return NULL;
        }

        cached = true;
    }
    return dir;
}

/* Checks the validity of the path - must be NULL-terminated (use path)
   any varargs are joined with the path param
   path is returned (non-NULL = true)
*/
const char *_path(const char *pahf, ...) {
    assert(pahf != NULL);
    static char buf[MAX_PATH];

    va_list varargs;
    va_start(varargs, pahf);

    int32_t s = 0;
    const char *pc = pahf;
    do {
        for (int32_t idx = 0; idx < strlen(pc) && s < MAX_PATH; idx++) {
            buf[s++] = pc[idx];
        }

        if (s >= MAX_PATH) break;
        buf[s++] = FSCHR;
    } while ((pc = va_arg(varargs, char*)));

    if (s >= MAX_PATH) {
        ERRLOG("Path is too long!");
        return NULL;
    }
    buf[--s] = '\0';

    va_end(varargs);

#ifdef _WIN32
    int32_t fattrs = GetFileAttributesA(buf);
    return (fattrs != INVALID_FILE_ATTRIBUTES) ? buf : NULL;
#endif
}

/* Compiles the *lid* tree-sitter language parser */
bool compile_parser(const char *lid) {
    assert(lid != NULL);

    LOG("Initialising compiler backend...");
    TCCState *cc = tcc_new();
    if (cc == NULL) {
        ERRLOG("Unable to initialise compiler backend");
        return false;
    }
    tcc_set_output_type(cc, TCC_OUTPUT_MEMORY);

    const char *pahf;
    if (!(pahf = path(get_ldir(),lid,"tree_sitter"))) {
        ERRLOG("Could not find required tree_sitter include directory in (%s)", path(get_ldir(),lid));
        return false;
    }
    LOG("Found tree_sitter include directory (%s)", pahf);
    tcc_add_include_path(cc, pahf);

    if (!(pahf = path(get_ldir(),lid,"parser.c"))) {
        ERRLOG("Could not find required parser.c source file in (%s)", path(get_ldir(),lid));
        return false;
    }
    LOG("Found parser.c source file (%s)", pahf);
    tcc_add_file(cc, pahf);

    if ((pahf = path(get_ldir(),lid,"scanner.c"))) {
        LOG("Found scanner.c source file (%s)", pahf);
        tcc_add_file(cc, pahf);
    }

    LOG("Compiling %s language parser...", lid);
    tcc_relocate(cc);
    LOG("Searching for tree_sitter_%s parser producer function...", lid);
    tcc_delete(cc);
    LOG("Finished!");

    return true;
}

int main(void) {
    compile_parser("c");
    compile_parser("python");
    return 0;
}
