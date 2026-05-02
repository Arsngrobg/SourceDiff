//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   The main application.

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>

#include "tree_sitter/api.h"
#include "libcc.h"

#include "sd_debug.h"
#include "sd_config.h"
#include "sd_string.h"
#include "sd_modes.h"

#if !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)
#error SD_VERSION, SD_DESCRIPTION, or SD_DOCS are not defined!
#endif // !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)

#define SD_HELP_STRING                                                                        \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                           \
    "Copyright (c) 2025 James Armstrong (Arsngrobg)\n"                                        \
    "\n"                                                                                      \
    "Usage:\n"                                                                                \
    "  %.*s diff     <file> <file>   The difference between both files\n"                     \
    "  %.*s analyse  <glob>          Structural analysis of the files which match the glob\n" \
    "  %.*s lint     <glob>          Style analysis of the files which match the glob\n"      \
    "  %.*s register <name> <dir>    Registers a new language (C compiler required)\n"        \
    "\n"                                                                                      \
    "Options:\n"                                                                              \
    "  --help             Display this information\n"                                         \
    "  --version          Display version information for SourceDiff\n"                       \
    "  --list-languages   Lists all registered language parsers\n"                            \
    "  -v                 Display diagnostics during analysis\n"                              \
    "  -o <file>          Output digestible, structured analysis results to the <file>\n"     \
    "\n"                                                                                      \
    "For more information: "SD_DOCS"\n"

void sd_strip_ext(const char *file, SD_StringView *view) {
    assert(file != NULL); assert(strlen(file) != 0);

    view->bytes  = file;
    view->length = strlen(file);
    for (size_t pos = view->length - 1; pos != 0; pos--) {
        if (file[pos] == '.') {
            view->length = pos;
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

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            cfg->mode = SD_MODE_DIFF;
            if ((arg + 2) >= (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m missing files to diff with\n", SD_STRING_FORMAT(cfg->exec));
                continue;
            }

            cfg->args[0] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("diff ARG[0] = '%.*s'", SD_STRING_FORMAT(cfg->args[0]));
            cfg->args[1] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("diff ARG[1] = '%.*s'", SD_STRING_FORMAT(cfg->args[1]));
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            cfg->mode = SD_MODE_ANALYSE;
            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", SD_STRING_FORMAT(cfg->exec));
                continue;
            }

            cfg->args[0] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("analyse ARG[0] = '%.*s'", SD_STRING_FORMAT(cfg->args[0]));
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            cfg->mode = SD_MODE_LINT;
            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", SD_STRING_FORMAT(cfg->exec));
                continue;
            }

            cfg->args[0] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("lint ARG[0] = '%.*s'", SD_STRING_FORMAT(cfg->args[0]));
        }
        else if (strcmp(argv[arg], "register") == 0) {
            cfg->mode = SD_MODE_REGISTER;
            if ((arg + 2) >= (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m missing language name and sources\n", SD_STRING_FORMAT(cfg->exec));
                continue;
            }

            cfg->args[0] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("register ARG[0] = '%.*s'", SD_STRING_FORMAT(cfg->args[0]));
            cfg->args[1] = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
            SD_DEBUG_LOGF("register ARG[1] = '%.*s'", SD_STRING_FORMAT(cfg->args[1]));
        }

        // options
        else if (strcmp(argv[arg], "--help") == 0) {
            cfg->options |= SD_OPTION_HELP;
        }
        else if (strcmp(argv[arg], "--version") == 0) {
            cfg->options |= SD_OPTION_VERSION;
        }
        else if (strcmp(argv[arg], "--list-languages") == 0) {
            cfg->options |= SD_OPTION_LIST_LANGUAGES;
        }
        else if (strcmp(argv[arg], "-v") == 0) {
            cfg->options |= SD_OPTION_VERBOSE;
        }
        else if (strcmp(argv[arg], "-o") == 0) {
            cfg->options |= SD_OPTION_OUTPUT;
            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m missing filename after '-o'\n", SD_STRING_FORMAT(cfg->exec));
                continue;
            }

            SD_DEBUG_LOGF("Processing sub-argument of '-o' ('%*s')", SD_STRING_FORMAT(cfg->exec));
            cfg->output = (SD_StringView) { strlen(argv[++arg]), argv[arg] };
        }
    }

    return status;
}

int32_t sd_exec(const SD_Config *cfg) {
    assert(cfg != NULL);

    int32_t status = EXIT_SUCCESS;

    // these arguments have higher priority
    if ((cfg->options & SD_OPTION_HELP) != 0) {
        fprintf(
            stdout,
            SD_HELP_STRING,
            SD_STRING_FORMAT(cfg->exec),
            SD_STRING_FORMAT(cfg->exec),
            SD_STRING_FORMAT(cfg->exec),
            SD_STRING_FORMAT(cfg->exec)
        );
        goto short_circuit;
    } else if ((cfg->options & SD_OPTION_VERSION) != 0) {
        fprintf(stdout, "v%s\n", SD_VERSION);
        goto short_circuit;
    } else if ((cfg->options & SD_OPTION_LIST_LANGUAGES) != 0) {
        struct dirent *entry;
        DIR *dir = opendir("languages");
        if (dir == NULL) {
            printf("%.*s: no languages registered\n", SD_STRING_FORMAT(cfg->exec));
            status = EXIT_SUCCESS;
            goto short_circuit;
        }

        size_t amount = 0;
        while ((entry = readdir(dir)) != NULL) {
            char *dot = strrchr(entry->d_name, '.');
            if (dot == NULL || strcmp(dot+1, CC_SHARED_LIB_EXT) != 0) {
                continue;
            }
            amount++;
        }

        rewinddir(dir);

        if (amount > 0) {
            printf("%.*s: registered languages: \033[1;32m%lld\x1b[0m\n", SD_STRING_FORMAT(cfg->exec), amount);
            while ((entry = readdir(dir)) != NULL) {
                char *dot = strrchr(entry->d_name, '.');
                if (dot == NULL || strcmp(dot+1, CC_SHARED_LIB_EXT) != 0) {
                    continue;
                }

                SD_StringView file_view = { strlen(entry->d_name) - 4, entry->d_name };
                printf(" - %.*s\n", SD_STRING_FORMAT(file_view));
            }
        }

        closedir(dir);

        status = EXIT_SUCCESS;
        goto short_circuit;
    }

    switch (cfg->mode) {
        case SD_MODE_NONE:
            fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m no mode specified\n", SD_STRING_FORMAT(cfg->exec));
            status = EXIT_FAILURE;
            break;
        case SD_MODE_DIFF:
            fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m 'diff' mode not implemented (TODO)\n", SD_STRING_FORMAT(cfg->exec));
            status = EXIT_FAILURE; // sd_mode_diff(cfg);
            break;
        case SD_MODE_ANALYSE:
            fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m 'analyse' mode not implemented (TODO)\n", SD_STRING_FORMAT(cfg->exec));
            status = EXIT_FAILURE; // sd_mode_analyse(cfg);
            break;
        case SD_MODE_LINT:
            fprintf(stderr, "%.*s: \x1b[1;31merror:\x1b[0m 'lint' mode not implemented (TODO)\n", SD_STRING_FORMAT(cfg->exec));
            status = EXIT_FAILURE; // sd_mode_lint(cfg);
            break;
        case SD_MODE_REGISTER:
            status = sd_mode_register(cfg);
            break;
    }

short_circuit:
    return status;
}

int32_t main(int32_t argc, const char *argv[]) {
    SD_Config cfg;
    return sd_parse_args(argc, argv, &cfg) | sd_exec(&cfg);
}
