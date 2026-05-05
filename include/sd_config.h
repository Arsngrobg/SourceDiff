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

#ifndef SD_CONFIG_H
#define SD_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// The modes of SourceDiff
typedef enum {
    SD_MODE_NONE     = 0, // ./srcdiff          ...
    SD_MODE_DIFF     = 1, // ./srcdiff diff     <file> <file>
    SD_MODE_ANALYSE  = 2, // ./srcdiff analyse  <dir>
    SD_MODE_LINT     = 3, // ./srcdiff lint     <dir>
    SD_MODE_REGISTER = 4  // ./srcdiff register <name> <dir>
} SD_Mode;

// The global options of SourceDiff - each representing the bit position they configure
typedef enum {
    SD_OPTION_HELP           = 1 << 0, // ./srcdiff --help
    SD_OPTION_VERSION        = 1 << 1, // ./srcdiff --version
    SD_OPTION_LIST_LANGUAGES = 1 << 2, // ./srcdiff --list-languages
    SD_OPTION_VERBOSE        = 1 << 3, // ./srcdiff -v        ...
    SD_OPTION_OUTPUT         = 1 << 4  // ./srcdiff -o <file> ...
} SD_Option;

const char  *SD_GetExecName      (void);                             // Gets the name of the executable (argv[0])
const char **SD_GetCLArgs        (void);                             // Gets the modal arguments
const char  *SD_GetOutputFileName(void);                             // Gets the name of the output file (NULL if none specified)
SD_Mode      SD_GetMode          (void);                             // Gets the current mode
bool         SD_IsOptionEnabled  (SD_Option option);                 // Whether the global boolean option is enabled
bool         SD_ParseCLArgs      (int32_t argc, const char *argv[]); // Parses the CLI arguments and storing it in a structured global object
#ifndef NDEBUG
bool         SD_CLArgsParsed     (void);                             // DEBUG: flag for assertions purposes
#endif // NDEBUG

#endif // SD_CONFIG_H
