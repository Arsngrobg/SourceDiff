//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   The configuration data of the application through its CLI interface.
//   It contains the configured state of SourceDiff through its supplied arguments.

#ifndef SDCONFIG_H
#define SDCONFIG_H

#include <stdint.h>

// The modes of SourceDiff
typedef enum {
    SD_MODE_NONE     = 0, // ./srcdiff          ...
    SD_MODE_DIFF     = 1, // ./srcdiff diff     <file> <file>
    SD_MODE_ANALYSE  = 2, // ./srcdiff analyse  <dir>
    SD_MODE_LINT     = 3, // ./srcdiff lint     <dir>
    SD_MODE_REGISTER = 4  // ./srcdiff register <name> <dir>
} SD_Mode;

// The global options of SourceDiff - each representing their bit they configure
typedef enum {
    SD_OPTION_HELP           = 1 << 0, // ./srcdiff --help
    SD_OPTION_VERSION        = 1 << 1, // ./srcdiff --version
    SD_OPTION_LIST_LANGUAGES = 1 << 2, // ./srcdiff --list-languages
    SD_OPTION_VERBOSE        = 1 << 3, // ./srcdiff -v        ...
    SD_OPTION_OUTPUT         = 1 << 4  // ./srcdiff -o <file> ...
} SD_Option;

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
} SD_Config;

#endif // SDCONFIG_H
