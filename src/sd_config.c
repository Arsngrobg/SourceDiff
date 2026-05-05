#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sd_debug.h"
#include "sd_config.h"

// The bits we use to denote enabled or disabled options
// 000ovLVH
typedef uint8_t SD_OptionSet;

// The configuration of SourceDiff
typedef struct {
    const char   *exec;    // the name of the executable hosting SourceDiff
    const char   *args[2]; // the regular arguments supplied to the application
    const char   *output;  // the output file
    SD_Mode       mode;    // the set mode
    SD_OptionSet  options; // the global options
#ifndef NDEBUG
    bool          parsed;  // flag for debugging assertions
#endif // NDEBUG
} SD_Config;

SD_Config *SD_GetConfig(void) {
    static SD_Config cfg = {0};
    return &cfg;
}

const char *SD_GetExecName(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->exec;
}

const char **SD_GetCLArgs(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->args;
}

const char *SD_GetOutputFileName(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->output;
}

SD_Mode SD_GetMode(void) {
    assert(SD_CLArgsParsed());
    return SD_GetConfig()->mode;
}

bool SD_IsOptionEnabled(SD_Option option) {
    assert(SD_CLArgsParsed());
    SD_OptionSet options = SD_GetConfig()->options;
    return (options & option) != 0;
}

bool SD_ParseCLArgs(int32_t argc, const char *argv[]) {
    assert(argc >= 0);
    assert(argv != NULL);

    bool valid = true;

    SD_Config *cfg = SD_GetConfig();
    cfg->exec = argv[0];
    cfg->mode = SD_MODE_NONE;

    for (size_t arg = 1; arg < (size_t) argc; arg++) {
        SD_LOG("Processing command-line argument '%s'", argv[arg]);

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            cfg->mode = SD_MODE_DIFF;
            if ((arg + 2) >= (size_t) argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing files to diff with\n", cfg->exec);
                continue;
            }

            SD_LOG("diff ARG[0] = '%s'", argv[arg+1]);
            SD_LOG("diff ARG[1] = '%s'", argv[arg+2]);
            cfg->args[0] = argv[++arg];
            cfg->args[1] = argv[++arg];
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            cfg->mode = SD_MODE_ANALYSE;
            if ((arg + 1) == (size_t) argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", cfg->exec);
                continue;
            }

            SD_LOG("analyse ARG[0] = '%s'", cfg->args[0]);
            cfg->args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            cfg->mode = SD_MODE_LINT;
            if ((arg + 1) == (size_t) argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing glob pattern\n", cfg->exec);
                continue;
            }

            SD_LOG("lint ARG[0] = '%s'", cfg->args[arg+1]);
            cfg->args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "register") == 0) {
            cfg->mode = SD_MODE_REGISTER;
            if ((arg + 2) >= (size_t) argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing language name and sources\n", cfg->exec);
                continue;
            }

            SD_LOG("register ARG[0] = '%s'", argv[arg+1]);
            SD_LOG("register ARG[1] = '%s'", argv[arg+2]);
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
            if ((arg + 1) == (size_t) argc) {
                valid = false;
                fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m missing filename after '-o'\n", cfg->exec);
                continue;
            }

            SD_LOG("Processing sub-argument of '-o' ('%s')", argv[arg+1]);
            cfg->output = argv[++arg];
        }
    }

#ifndef NDEBUG
    cfg->parsed = true;
#endif // NDEBUG
    return valid;
}

#ifndef NDEBUG
bool SD_CLArgsParsed(void) {
    return SD_GetConfig()->parsed;
}
#endif // NDEBUG
