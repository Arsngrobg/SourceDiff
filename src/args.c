#include "srcdiff_stdutils.h"
#include "srcdiff.h"

#define SRCDIFF_ARGC     (2)                        // the most arguments ever required
#define SRCDIFF_ASBIT(o) ((SDOptionSet)(1u << (o))) // for getting the bit position for an SDMode

// The bits we use to store enabled options
typedef uint8_t SDOptionSet; // 000ovLVH

typedef struct {
    const char   *exec;    // the name of the executable
    const char  **args;    // the start of the arguments of the specific mode
    size_t        argc;    // the number of arguments for the mode
    SDMode        mode;    // the current mode of the application
    SDLutCommand  lutcmd;  // the LUT configuration command (if applicable)
    SDOptionSet   options; // the options enabled
    FILE         *outfile; // the output file (if -o flag was used)
#ifndef NDEBUG
    bool          parsed;  // debug flag
#endif // NDEBUG
} SDArgs;

SDPRIVATE SDArgs args;

/// Parses the raw `argv`
SDPUBLIC
bool sd_parse_argv(int32_t argc, const char *argv[]) {
    assert(argc >= 0 && argv != NULL);

    args = (SDArgs) {0};
    args.exec = argv[0];
#ifndef NDEBUG
    args.parsed = true;
#endif // NDEBUG

    bool error = false;
    for (int32_t arg = 1; arg < argc; arg++) {
        sd_log_debug("Processing command-line argument '%s'", argv[arg]);

        // modes
        if (strcmp(argv[arg], "diff") == 0) {
            args.mode = SD_MODE_DIFF;
            if ((arg + 2) >= argc) {
                error = true;
                sd_log_error("missing files to diff with");
                continue;
            }

            sd_log_debug("diff ARG[0] = '%s'", argv[arg+1]);
            sd_log_debug("diff ARG[1] = '%s'", argv[arg+2]);
            args.args = argv + arg + 1;
            args.argc = 2;
        }
        else if (strcmp(argv[arg], "analyse") == 0) {
            args.mode = SD_MODE_ANALYSE;
            if ((arg + 1) == argc) {
                error = true;
                sd_log_error("missing glob pattern");
                continue;
            }

            sd_log_debug("analyse ARG[0] = '%s'", argv[0]);
            args.args = argv + arg + 1;
            args.argc = 1;
        }
        else if (strcmp(argv[arg], "lint") == 0) {
            args.mode = SD_MODE_LINT;
            if ((arg + 1) == argc) {
                error = true;
                sd_log_error("missing glob pattern");
                continue;
            }

            sd_log_debug("lint ARG[0] = '%s'", argv[arg+1]);
            args.args = argv + arg + 1;
            args.argc = 1;
        }
        else if (strcmp(argv[arg], "register") == 0) {
            args.mode = SD_MODE_REGISTER;
            if ((arg + 2) >= argc) {
                error = true;
                sd_log_error("missing language name and sources");
                continue;
            }

            sd_log_debug("register ARG[0] = '%s'", argv[arg+1]);
            sd_log_debug("register ARG[1] = '%s'", argv[arg+2]);
            args.args = argv + arg + 1;
            args.argc = 2;
        }
        else if (strcmp(argv[arg], "lut") == 0) {
            args.mode = SD_MODE_LUT;
            if ((arg + 1) >= argc) {
                error = true;
                sd_log_error("missing submode for lut configuration");
                continue;
            }

            sd_log_debug("Processing sub-argument of 'lut' ('%s')", argv[arg+1]);
            arg++;
            if (strcmp(argv[arg], "info") == 0) {
                args.lutcmd = SD_LUT_COMMAND_INFO;
            } else if (strcmp(argv[arg], "clear") == 0) {
                args.lutcmd = SD_LUT_COMMAND_CLEAR;
            } else if (strcmp(argv[arg], "set") == 0) {
                args.lutcmd = SD_LUT_COMMAND_SET;
                if ((arg + 1) == argc) {
                    error = true;
                    sd_log_error("missing config");
                    continue;
                }

                sd_log_debug("lut set ARG[0] = '%s'", argv[arg+1]);
                args.args = argv + arg + 1;
                args.argc = 1;
            } else if (strcmp(argv[arg], "add") == 0) {
                args.lutcmd = SD_LUT_COMMAND_ADD;
                if ((arg + 1) >= argc) {
                    error = true;
                    sd_log_error("missing config");
                    continue;
                }

                sd_log_debug("lut add ARG[0] = '%s'", argv[arg+1]);
                args.args = argv + arg + 1;
                args.argc = 1;
            } else {
                error = true;
                sd_log_error("illegal lut sub mode");
            }
        }

        // options
        else if (strcmp(argv[arg], "--help") == 0) {
            args.options |= SRCDIFF_ASBIT(SD_OPTION_HELP);
        }
        else if (strcmp(argv[arg], "--version") == 0) {
            args.options |= SRCDIFF_ASBIT(SD_OPTION_VERSION);
        }
        else if (strcmp(argv[arg], "--list-languages") == 0) {
            args.options |= SRCDIFF_ASBIT(SD_OPTION_LIST_LANGUAGES);
        }
        else if (strcmp(argv[arg], "-v") == 0) {
            args.options |= SRCDIFF_ASBIT(SD_OPTION_VERBOSE);
        }
        else if (strcmp(argv[arg], "-o") == 0) {
            args.options |= SRCDIFF_ASBIT(SD_OPTION_OUTPUT);
            if ((arg + 1) == argc) {
                error = true;
                sd_log_error("missing filename after '-o'");
                continue;
            }

            sd_log_debug("Processing sub-argument of '-o' ('%s')", argv[arg+1]);
            args.outfile = fopen(argv[++arg], "w+");
            if (args.outfile == NULL) {
                sd_log_error("Unable to open the output file");
                error = true;
                continue;
            }
        }
        else {
            sd_log_error("unknown argument '%s'", argv[arg]);
        }
    }

    return !error;
}

/// Gets the name of the executable
SDPUBLIC
const char *sd_exec_name(void) {
    assert(sd_is_argv_parsed());
    return args.exec;
}

/// Gets the argument at `idx`
SDPUBLIC
const char *sd_get_arg(size_t idx) {
    assert(sd_is_argv_parsed() && idx < args.argc);
    return args.args[idx];
}

/// Gets the output file (if -o flag was used)
SDPUBLIC
FILE *sd_get_output_file(void) {
    assert(sd_is_argv_parsed());
    return args.outfile;
}

/// Gets the configured mode
SDPUBLIC
SDMode sd_get_mode(void) {
    assert(sd_is_argv_parsed());
    return args.mode;
}

/// Gets the LUT configuration command
SDPUBLIC
SDLutCommand sd_get_lut_command(void) {
    assert(sd_is_argv_parsed());
    return args.lutcmd;
}

/// Tests to see whether the option is enabled
SDPUBLIC
bool sd_is_option_set(SDOption option) {
    assert(sd_is_argv_parsed());
    SDOptionSet options = args.options;
    return (options & SRCDIFF_ASBIT(option)) != 0;
}

#ifndef NDEBUG
/// Debug function for assertions
SDPUBLIC
bool sd_is_argv_parsed(void) {
    return args.parsed;
}
#endif // NDEBUG
