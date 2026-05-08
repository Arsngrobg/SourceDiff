//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   The register mode

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>

#include "libcc.h"

#include "srcdiff.h"

#define SD_LANGDUMP "languages"

int32_t SD_ExecRegister(void) {
    assert(SD_CLArgsParsed());
    assert(SD_GetMode() == SD_MODE_REGISTER);

    const char *dirstr = SD_GetModeArgs()[1];

    struct dirent *entry;
    DIR *dir = opendir(dirstr);
    if (dir == NULL) {
        fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m directory does not exist\n", SD_GetExecName());
        return EXIT_FAILURE;
    }

    bool has_include = false;
    bool has_parser  = false;
    bool has_scanner = false;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "parser.c") == 0) {
            SD_LogDebug("Found parser.c");
            has_parser = true;
        } else if (strcmp(entry->d_name, "scanner.c") == 0) {
            SD_LogDebug("Found scanner.c");
            has_scanner = true;
        } else if (strcmp(entry->d_name, "tree_sitter") == 0) {
            SD_LogDebug("Found tree_sitter directory");
            has_include = true;
        }
    }

    // the default setup for a tree-sitter grammar
    if (!has_include && !has_parser) {
        fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m does not match conventional tree-sitter grammar structure\n", SD_GetExecName());
        return EXIT_FAILURE;
    }

    CC_Toolchain *cc = cc_new();
    if (cc == NULL) {
        return EXIT_FAILURE;
    }

    cc_add_source(cc, "%s/parser.c", dirstr);
    cc_add_include_path(cc, "%s/tree_sitter", dirstr);
    if (has_scanner) {
        cc_add_source(cc, "%s/scanner.c", dirstr);
    }

    if (mkdir(SD_LANGDUMP) == 0) {
        SD_LogDebug("Created "SD_LANGDUMP" directory");
    }

    cc_set_output_type(cc, CC_OUTPUT_SHARED);
    cc_set_output(cc, SD_LANGDUMP"/%s."CC_SHARED_LIB_EXT, SD_GetModeArgs()[0]);
    cc_invoke(cc);

    SD_LogDebug("INVOKED COMMAND: %s", cc_render_command(cc));

    cc_delete(cc);
    closedir(dir);

    printf("%s: sucessfully registered language: \x1b[1;32m%s\x1b[0m\n", SD_GetExecName(), SD_GetModeArgs()[0]);
    return EXIT_SUCCESS;
}
