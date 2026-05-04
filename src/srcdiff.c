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
#include "sd_modes.h"

#if !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)
#error SD_VERSION, SD_DESCRIPTION, or SD_DOCS are not defined!
#endif // !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)

#define SD_HELP_STRING                                                                      \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                         \
    "Copyright (c) 2025 James Armstrong (Arsngrobg)\n"                                      \
    "\n"                                                                                    \
    "Usage:\n"                                                                              \
    "  %s diff     <file> <file>   The difference between both files\n"                     \
    "  %s analyse  <glob>          Structural analysis of the files which match the glob\n" \
    "  %s lint     <glob>          Style analysis of the files which match the glob\n"      \
    "  %s register <name> <dir>    Registers a new language (C compiler required)\n"        \
    "\n"                                                                                    \
    "Options:\n"                                                                            \
    "  --help             Display this information\n"                                       \
    "  --version          Display version information for SourceDiff\n"                     \
    "  --list-languages   Lists all registered language parsers\n"                          \
    "  -v                 Display diagnostics during analysis\n"                            \
    "  -o <file>          Output digestible, structured analysis results to the <file>\n"   \
    "\n"                                                                                    \
    "For more information: "SD_DOCS"\n"

int32_t SD_parse_args(int32_t argc, const char *argv[], SD_Config *cfg) {
    assert(argc >= 0); assert(argv != NULL);

    int32_t status = EXIT_SUCCESS;

    cfg->exec = argv[0];
    cfg->mode = SD_MODE_NONE;

    for (size_t arg = 1; arg < (size_t) argc; arg++) {
        SD_DEBUG_LOGF("Processing command-line argument '%s'", argv[arg]);

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            cfg->mode = SD_MODE_DIFF;
            if ((arg + 2) >= (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing files to diff with\n", cfg->exec);
                continue;
            }

            cfg->args[0] = argv[++arg];
            SD_DEBUG_LOGF("diff ARG[0] = '%s'", cfg->args[0]);
            cfg->args[1] = argv[++arg];
            SD_DEBUG_LOGF("diff ARG[1] = '%s'", cfg->args[1]);
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            cfg->mode = SD_MODE_ANALYSE;
            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", cfg->exec);
                continue;
            }

            cfg->args[0] = argv[++arg];
            SD_DEBUG_LOGF("analyse ARG[0] = '%s'", cfg->args[0]);
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            cfg->mode = SD_MODE_LINT;
            if ((arg + 1) == (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", cfg->exec);
                continue;
            }

            cfg->args[0] = argv[++arg];
            SD_DEBUG_LOGF("lint ARG[0] = '%s'", cfg->args[0]);
        }
        else if (strcmp(argv[arg], "register") == 0) {
            cfg->mode = SD_MODE_REGISTER;
            if ((arg + 2) >= (size_t) argc) {
                status = EXIT_FAILURE;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing language name and sources\n", cfg->exec);
                continue;
            }

            cfg->args[0] = argv[++arg];
            SD_DEBUG_LOGF("register ARG[0] = '%s'", cfg->args[0]);
            cfg->args[1] = argv[++arg];
            SD_DEBUG_LOGF("register ARG[1] = '%s'", cfg->args[1]);
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
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing filename after '-o'\n", cfg->exec);
                continue;
            }

            SD_DEBUG_LOGF("Processing sub-argument of '-o' ('%s')", cfg->exec);
            cfg->output = argv[++arg];
        }
    }

    return status;
}

int32_t SD_exec(const SD_Config *cfg) {
    assert(cfg != NULL);

    int32_t status = EXIT_SUCCESS;

    // these arguments have higher priority
    if ((cfg->options & SD_OPTION_HELP) != 0) {
        fprintf(
            stdout,
            SD_HELP_STRING,
            cfg->exec, cfg->exec, cfg->exec, cfg->exec
        );
        goto short_circuit;
    } else if ((cfg->options & SD_OPTION_VERSION) != 0) {
        fprintf(stdout, "v%s\n", SD_VERSION);
        goto short_circuit;
    } else if ((cfg->options & SD_OPTION_LIST_LANGUAGES) != 0) {
        struct dirent *entry;
        DIR *dir = opendir("languages");
        if (dir == NULL) {
            printf("%s: no languages registered\n", cfg->exec);
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
            printf("%s: registered languages: \033[1;32m%lld\x1b[0m\n", cfg->exec, amount);
            while ((entry = readdir(dir)) != NULL) {
                char *dot = strrchr(entry->d_name, '.');
                if (dot == NULL || strcmp(dot+1, CC_SHARED_LIB_EXT) != 0) {
                    continue;
                }
                *dot = '\0';

                printf(" - %s\n", entry->d_name);
            }
        }

        closedir(dir);

        status = EXIT_SUCCESS;
        goto short_circuit;
    }

    switch (cfg->mode) {
        case SD_MODE_NONE:
            fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m no mode specified\n", cfg->exec);
            status = EXIT_FAILURE;
            break;
        case SD_MODE_DIFF:
            fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m 'diff' mode not implemented (TODO)\n", cfg->exec);
            status = EXIT_FAILURE; // SD_mode_diff(cfg);
            break;
        case SD_MODE_ANALYSE:
            fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m 'analyse' mode not implemented (TODO)\n", cfg->exec);
            status = EXIT_FAILURE; // SD_mode_analyse(cfg);
            break;
        case SD_MODE_LINT:
            fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m 'lint' mode not implemented (TODO)\n", cfg->exec);
            status = EXIT_FAILURE; // SD_mode_lint(cfg);
            break;
        case SD_MODE_REGISTER:
            status = SD_mode_register(cfg);
            break;
    }

short_circuit:
    return status;
}

int32_t main(int32_t argc, const char *argv[]) {
    SD_Config cfg;
    return SD_parse_args(argc, argv, &cfg) | SD_exec(&cfg);
}
