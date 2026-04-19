
//    ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
//   ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
//   ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀
//   ▐░▌          ▐░▌       ▐░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░▌          ▐░▌
//   ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄
//   ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
//    ▀▀▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀
//             ▐░▌▐░▌     ▐░▌  ▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░▌          ▐░▌
//    ▄▄▄▄▄▄▄▄▄█░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌ ▄▄▄▄█░█▄▄▄▄ ▐░▌          ▐░▌
//   ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░▌          ▐░▌
//   ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀            ▀
//
//   The main application

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "tree_sitter/api.h"

#include "sddebug.h"
#include "sdconfig.h"
#include "sdoptions.h"

#if !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_REPO)
#error SD_VERSION, SD_DESCRIPTION, or SD_REPO are not defined!
#endif // !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_REPO)

#define SD_HELP_STRING                                                                   \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                      \
    "Copyright (c) 2025 James Armstrong\n"                                               \
    "\n"                                                                                 \
    "Usage:\n"                                                                           \
    "  %.*s diff     <file1> <file2>   The difference between <file1> and <file2>\n"     \
    "  %.*s register <name>  <dir>     Registers a new language (C compiler required)\n" \
    "Options:\n"                                                                         \
    "  --help      Display this information\n"                                           \
    "  --version   Display version information for SourceDiff\n"                         \
    "  -v          Display diagnostics during analysis\n"                                \
    "  -o <file>   Output digestible, structured analysis results to the <file>\n"       \
    "\n"                                                                                 \
    "For more information: "SD_REPO"\n"

// helper type for safe string slicing
typedef struct {
    const char *str;
    uint64_t    start;
    uint64_t    length;
} SD_StringSlice;

#define SD_STRINGSLICE_FMT(s) (int) s.length, s.str + s.start

void sd_strip_ext(const char *file, SD_StringSlice *slice) {
    assert(file != NULL); assert(strlen(file) != 0);

    slice->str    = file;
    slice->start  = 0;
    slice->length = strlen(file);
    int64_t idx = strlen(file) - 1;
    while (idx >= 0) {
        if (slice->str[idx] == '.') {
            slice->length = idx;
            break;
        }

        idx--;
    }
}

int32_t main(int32_t argc, char *argv[]) {
    (void) argc;
    SD_StringSlice noext;
    sd_strip_ext(argv[0], &noext);
    printf(SD_HELP_STRING, SD_STRINGSLICE_FMT(noext), SD_STRINGSLICE_FMT(noext));
    return 0;
}
