//
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
//   temporary unity build for building the initial product
//   will seperate later

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "tree_sitter/api.h"

#define SD_HELP_STRING                                                                 \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                    \
    "Copyright (c) 2025 James Armstrong\n"                                             \
    "\n"                                                                               \
    "Usage:\n"                                                                         \
    "  %s diff     <file1> <file2>   The difference between <file1> and <file2>\n"     \
    "  %s register <name>  <dir>     Registers a new language (C compiler required)\n" \
    "Options:\n"                                                                       \
    "  --help      Display this information\n"                                         \
    "  --version   Display version information for SourceDiff\n"                       \
    "  -o <file>   Output digestible, structured analysis results to the <file>\n"     \
    "  -v          Display diagnostics during analysis\n"                              \
    "\n"                                                                               \
    "For more information: "SD_REPO"\n"

char *sd_strip_ext(char *file) {
    assert(file != NULL); assert(strlen(file) != 0);

    int32_t idx = strlen(file);
    while (idx >= 0) {
        if (file[idx] == '.') {
            file[idx] = '\0';
            break;
        }

        idx--;
    }

    return file;
}

int32_t main(int32_t argc, char *argv[]) {
    (void)argc;
    printf(SD_HELP_STRING, sd_strip_ext(argv[0]), argv[0]);
    printf("%s\n", argv[1]);
    return 0;
}
