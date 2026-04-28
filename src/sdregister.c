//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   The register mode of SourceDiff.

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>

#include "libcc.h"

#include "sdmodes.h"
#include "sddebug.h"

int32_t sd_mode_register(const SD_Config *cfg) {
    assert(cfg != NULL); assert(cfg->mode == SD_MODE_REGISTER);

    const SD_StringView *path_view = &cfg->args[1];
    char dirstr[path_view->length + 1];
    strncpy(dirstr, path_view->buf, path_view->length);
    dirstr[path_view->length] = '\0';

    struct dirent *entry;
    DIR *dir = opendir(dirstr);
    if (dir == NULL) {
        fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m directory does not exist\n", SD_STRING_FORMAT(cfg->exec));
        return EXIT_FAILURE;
    }

    bool has_include = false;
    bool has_parser  = false;
    bool has_scanner = false;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "parser.c") == 0) {
            has_parser = true;
        } else if (strcmp(entry->d_name, "scanner.c") == 0) {
            has_scanner = true;
        } else if (strcmp(entry->d_name, "tree_sitter") == 0) {
            has_include = true;
        }
    }

    // the default setup for a tree-sitter grammar
    if (!has_include && !has_parser) {
        fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m does not match conventional tree-sitter grammar structure\n", SD_STRING_FORMAT(cfg->exec));
        return EXIT_FAILURE;
    }

    CC_Toolchain *cc = cc_new();
    if (cc == NULL) {
        return EXIT_FAILURE;
    }

    cc_add_source(cc, "\"%s/parser.c\"", dirstr);
    cc_add_include_path(cc, "\"%s/tree_sitter\"", dirstr);
    if (has_scanner) {
        cc_add_source(cc, "\"%s/scanner.c\"", dirstr);
    }

    // UGLY: bit clunky right now
    mkdir("languages");
    cc_set_output_type(cc, CC_OUTPUT_SHARED);
    cc_set_output(cc, "languages/%.*s.dll", SD_STRING_FORMAT(cfg->args[0]));
    cc_invoke(cc);

    SD_DEBUG_LOGF("INVOKED COMMAND: %s", cc_render_command(cc));

    cc_delete(cc);
    closedir(dir);
    return EXIT_SUCCESS;
}
