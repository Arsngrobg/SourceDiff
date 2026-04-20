
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
//   The main application.

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tree_sitter/api.h"

#include "sddebug.h"
#include "sdconfig.h"
#include "sdstring.h"

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

void sd_strip_ext(const char *file, SD_StringView *view) {
    assert(file != NULL); assert(strlen(file) != 0);

    view->src    = file;
    view->length = strlen(file);
    for (size_t pos = view->length - 1; pos != 0; pos--) {
        if (file[pos] == '.') {
            view->length = pos - 1;
            break;
        }
    }
}

int32_t sd_parse_args(int32_t argc, const char *argv[], SD_Config *cfg) {
    assert(argc >= 0); assert(argv != NULL);

    int32_t status = EXIT_SUCCESS;

    sd_strip_ext(argv[0], &cfg->exec);
    cfg->mode = SD_MODE_NONE;

    for (size_t arg = 1; arg < (size_t) argc; arg++) {
        SD_DEBUG_LOGF("Processing command-line argument '%s'", argv[arg]);

        if (strcmp(argv[arg], "diff") == 0) {
            cfg->mode = SD_MODE_DIFF;
        }
        else if (strcmp(argv[arg], "register") == 0) {
            cfg->mode = SD_MODE_REGISTER;
        }

        else if (strcmp(argv[arg], "--help") == 0) {
            cfg->options |= SD_OPTION_HELP;
        }
        else if (strcmp(argv[arg], "--version") == 0) {
            cfg->options |= SD_OPTION_VERSION;
        }
        else if (strcmp(argv[arg], "-v") == 0) {
            cfg->options |= SD_OPTION_VERBOSE;
        }
        else if (strcmp(argv[arg], "-o") == 0) {
            cfg->options |= SD_OPTION_OUTPUT;

            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%*s: \x1b[1;31merror:\x1b[0m missing filename after '-o'\n", (int32_t) cfg->exec.length, cfg->exec.src);
                continue;
            }

            arg++;
            cfg->output = (SD_StringView) { strlen(argv[arg]), argv[arg] };
            SD_DEBUG_LOGF("Processing sub-argument of '-o' ('%*s')", (int32_t) cfg->output.length, cfg->output.src);
        }
    }

    return status;
}

int32_t sd_exec(const SD_Config *cfg) {
    assert(cfg != NULL);

    int32_t status = EXIT_SUCCESS;
    
    // these arguments have higher priority
    if ((cfg->options & SD_OPTION_HELP) != 0) {
        fprintf(stdout, SD_HELP_STRING, (int32_t) cfg->exec.length, cfg->exec.src, (int32_t) cfg->exec.length, cfg->exec.src);
        goto early_exit;
    } else if ((cfg->options & SD_OPTION_VERSION) != 0) {
        fprintf(stdout, "v%s\n", SD_VERSION);
        goto early_exit;
    }

    switch (cfg->mode) {
        case SD_MODE_NONE:
            fprintf(stderr, "%*s: \x1b[1;31merror:\x1b[0m no mode specified\n", (int32_t) cfg->exec.length, cfg->exec.src);
            status = EXIT_FAILURE;
            break;
        case SD_MODE_DIFF:
            fprintf(stderr, "%*s: \x1b[1;31merror:\x1b[0m diff mode not implemented (TODO)\n", (int32_t) cfg->exec.length, cfg->exec.src);
            status = EXIT_FAILURE;
            break;
        case SD_MODE_REGISTER:
            fprintf(stderr, "%*s: \x1b[1;31merror:\x1b[0m register mode not implemented (TODO)\n", (int32_t) cfg->exec.length, cfg->exec.src);
            status = EXIT_FAILURE;
            break;
    }

early_exit:
    return status;
}

int32_t main(int32_t argc, const char *argv[]) {
    SD_Config cfg;
    return !((sd_parse_args(argc, argv, &cfg) == 0) && (sd_exec(&cfg) == 0));
}
