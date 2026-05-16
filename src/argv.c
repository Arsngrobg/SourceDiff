#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "argv.h"

#define SD_MAXARGV  (2)        // the maximum number of literal arguments any mode could have
#define SD_ASBIT(o) (1 << (o)) // transformes the `SD_Option` into its equivalent bit position in `SD_OptionSet`

/// The bits we use to store enabled options
typedef uint8_t SD_OptionSet; // 000ovLVH

static struct SD_Argv {
    const char   *exec;             // the name of the executable
    const char   *args[SD_MAXARGV]; // the literal arguments
    FILE         *outfile;          // the output file
    SD_Mode       mode;             // the configured mode
    SD_OptionSet  options;          // the options enabled
#ifndef NDEBUG
    bool          parsed;           // debug: whether argv has been parsed
#endif // NDEBUG
} sd_argv = {0};

/// Parses the `argv`
bool SD_ParseArgv(int32_t argc, const char *argv[]) {
    assert(argc > 0 && argv != NULL);
    bool valid = true;

#ifndef NDEBUG
    sd_argv.parsed = true;
#endif // NDEBUG

    sd_argv.exec = argv[0];
    sd_argv.mode = SD_MODE_NONE;

    for (int32_t arg = 1; arg < argc; arg++) {
        SD_LogDebug("Processing command-line argument '%s'", argv[arg]);

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            sd_argv.mode = SD_MODE_DIFF;
            if ((arg + 2) >= argc) {
                valid = false;
                SD_LogError("missing files to diff with");
                continue;
            }

            SD_LogDebug("diff ARG[0] = '%s'", argv[arg+1]);
            SD_LogDebug("diff ARG[1] = '%s'", argv[arg+2]);
            sd_argv.args[0] = argv[++arg];
            sd_argv.args[1] = argv[++arg];
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            sd_argv.mode = SD_MODE_ANALYSE;
            if ((arg + 1) == argc) {
                valid = false;
                SD_LogError("missing glob pattern");
                continue;
            }

            SD_LogDebug("analyse ARG[0] = '%s'", argv[0]);
            sd_argv.args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            sd_argv.mode = SD_MODE_LINT;
            if ((arg + 1) == argc) {
                valid = false;
                SD_LogError("missing glob pattern");
                continue;
            }

            SD_LogDebug("lint ARG[0] = '%s'", argv[arg+1]);
            sd_argv.args[0] = argv[++arg];
        }
        else if (strcmp(argv[arg], "register") == 0) {
            sd_argv.mode = SD_MODE_REGISTER;
            if ((arg + 2) >= argc) {
                valid = false;
                SD_LogError("missing language name and sources");
                continue;
            }

            SD_LogDebug("register ARG[0] = '%s'", argv[arg+1]);
            SD_LogDebug("register ARG[1] = '%s'", argv[arg+2]);
            sd_argv.args[0] = argv[++arg];
            sd_argv.args[1] = argv[++arg];
        }
        else if (strcmp(argv[arg], "lut") == 0) {
            if ((arg + 1) >= argc) {
                valid = false;
                SD_LogError("missing submode for lut configuration");
                continue;
            }

            arg++;
            if (strcmp(argv[arg], "info") == 0) {
                sd_argv.mode = SD_MODE_LUT_INFO;
            } else if (strcmp(argv[arg], "clear") == 0) {
                sd_argv.mode = SD_MODE_LUT_CLEAR;
            } else if (strcmp(argv[arg], "set") == 0) {
                sd_argv.mode = SD_MODE_LUT_SET;
                if ((arg + 1) >= argc) {
                    valid = false;
                    SD_LogError("missing config");
                    continue;
                }

                SD_LogDebug("lut set ARG[0] = '%s'", argv[arg+1]);
                sd_argv.args[0] = argv[++arg];
            } else if (strcmp(argv[arg], "add") == 0) {
                sd_argv.mode = SD_MODE_LUT_ADD;
                if ((arg + 1) >= argc) {
                    valid = false;
                    SD_LogError("missing config");
                    continue;
                }

                SD_LogDebug("lut add ARG[0] = '%s'", argv[arg+1]);
                sd_argv.args[0] = argv[++arg];
            } else {
                valid = false;
                SD_LogError("illegal lut sub mode");
            }
        }

        // options
        else if (strcmp(argv[arg], "--help") == 0) {
            sd_argv.options |= SD_ASBIT(SD_OPTION_HELP);
        }
        else if (strcmp(argv[arg], "--version") == 0) {
            sd_argv.options |= SD_ASBIT(SD_OPTION_VERSION);
        }
        else if (strcmp(argv[arg], "--list-languages") == 0) {
            sd_argv.options |= SD_ASBIT(SD_OPTION_LIST_LANGUAGES);
        }
        else if (strcmp(argv[arg], "-v") == 0) {
            sd_argv.options |= SD_ASBIT(SD_OPTION_VERBOSE);
        }
        else if (strcmp(argv[arg], "-o") == 0) {
            sd_argv.options |= SD_ASBIT(SD_OPTION_OUTPUT);
            if ((arg + 1) == argc) {
                valid = false;
                SD_LogError("missing filename after '-o'");
                continue;
            }

            SD_LogDebug("Processing sub-argument of '-o' ('%s')", argv[arg+1]);
            sd_argv.outfile = fopen(argv[++arg], "w+");
            if (sd_argv.outfile == NULL) {
                SD_LogError("Unable to open the output file");
                valid = false;
                continue;
            }
        }
    }

    return valid;
}

/// Gets the name of the executable
const char *SD_GetExecName(void) {
    assert(SD_IsArvParsed());
    return sd_argv.exec;
}

/// Gets the argument at `idx`
const char *SD_GetArgv(size_t idx) {
    assert(SD_IsArvParsed());
    return sd_argv.args[idx];
}

/// Gets the output file (if -o flag was used)
FILE *SD_GetOutputFile(void) {
    assert(SD_IsArvParsed());
    return sd_argv.outfile;
}

/// Gets the configured mode
SD_Mode SD_GetMode(void) {
    assert(SD_IsArvParsed());
    return sd_argv.mode;
}

/// Tests to see whether the option is enabled
bool SD_IsOptionSet(SD_Option option) {
    assert(SD_IsArvParsed());
    return (sd_argv.options & SD_ASBIT(option)) != 0;
}

#ifndef NDEBUG
/// DEBUG: is `argv` parsed?
bool SD_IsArvParsed(void) {
    return sd_argv.parsed;
}
#endif // NDEBUG
