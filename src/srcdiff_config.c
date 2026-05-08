//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Configuration State Management

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "srcdiff.h"

// The bits we use to denote enabled or disabled options
// 000ovLVH
typedef uint8_t SD_OptionSet;

// The configuration of SourceDiff
typedef struct {
    const char   *exec;    // The name of the executable
    const char   *args[2]; // The modal arguments supplied to the application
    FILE         *outfile; // The output file
    SD_Mode       mode;    // The current mode
    SD_OptionSet  options; // the global boolean options
#ifndef NDEBUG
    bool          parsed;  // debug
#endif // NDEBUG
} SD_Config;

// Static storage for an SD_Config
SD_Config *SD_GetConfig(void) {
    static SD_Config cfg = {0};
    return &cfg;
}

int32_t SD_ParseCLArgs(int32_t argc, const char *argv[]) {
    assert(argc >= 0);
    assert(argv != NULL);

    SD_Config *cfg = SD_GetConfig();
#ifndef NDEBUG
    cfg->parsed = true;
#endif // NDEBUG
    cfg->mode = SD_MODE_NONE;
    if (argc == 0) {
        cfg->exec = "SourceDiff";
        return EXIT_SUCCESS;
    } else cfg->exec = argv[0];

    bool valid = true;

    for (int32_t arg = 1; arg < argc; arg++) {
        SD_LogDebug("Processing command-line argument '%s'", argv[arg]);

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            cfg->mode = SD_MODE_DIFF;
            if ((arg + 2) >= argc) {
                valid = false;
                SD_LogError("missing files to diff with");
                continue;
            }

            SD_LogDebug("diff ARG[0] = '%s'", argv[arg+1]);
            SD_LogDebug("diff ARG[1] = '%s'", argv[arg+2]);
            cfg->args[0] = argv[++arg];
            cfg->args[1] = argv[++arg];
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            cfg->mode = SD_MODE_ANALYSE;
            if ((arg + 1) == argc) {
                valid = false;
                SD_LogError("missing glob pattern");
                continue;
            }

            SD_LogDebug("analyse ARG[0] = '%s'", cfg->args[0]);
            cfg->args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            cfg->mode = SD_MODE_LINT;
            if ((arg + 1) == argc) {
                valid = false;
                SD_LogError("missing glob pattern");
                continue;
            }

            SD_LogDebug("lint ARG[0] = '%s'", cfg->args[arg+1]);
            cfg->args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "register") == 0) {
            cfg->mode = SD_MODE_REGISTER;
            if ((arg + 2) >= argc) {
                valid = false;
                SD_LogError("missing language name and sources");
                continue;
            }

            SD_LogDebug("register ARG[0] = '%s'", argv[arg+1]);
            SD_LogDebug("register ARG[1] = '%s'", argv[arg+2]);
            cfg->args[0] = argv[++arg];
            cfg->args[1] = argv[++arg];
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
            if ((arg + 1) == argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing filename after '-o'\n", cfg->exec);
                continue;
            }

            SD_LogDebug("Processing sub-argument of '-o' ('%s')", argv[arg+1]);
            SD_LogDebug("Creating output file '%s'", argv[arg+1]);
            cfg->outfile = fopen(argv[++arg], "w+");
            if (cfg->outfile == NULL) {
                valid = false;
                SD_LogError("unable to create output file '%s'", argv[arg]);
                continue;
            }
            SD_LogDebug("Got file handle for output file '%s'", argv[arg]);
        }
    }

    return valid;
}

const char *SD_GetExecName(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->exec;
}

const char **SD_GetModeArgs(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->args;
}

FILE *SD_GetOutputFile(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->outfile;
}

SD_Mode SD_GetMode(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->mode;
}

bool SD_IsOptionSet(SD_Option option) {
    assert(SD_CLArgsParsed());
    SD_OptionSet options = SD_GetConfig()->options;
    return (options & option) != 0;
}

#ifndef NDEBUG
bool SD_CLArgsParsed(void) {
    return SD_GetConfig()->parsed;
}
#endif // NDEBUG
