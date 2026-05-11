//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Global type definitions

#ifndef SRCDIFF_TYPES_H
#define SRCDIFF_TYPES_H

// The modes of SourceDiff
typedef enum {
    // Operational Modes
    SD_MODE_NONE,           // ./srcdiff               ...
    SD_MODE_DIFF,           // ./srcdiff diff          <file>    <file>
    SD_MODE_ANALYSE,        // ./srcdiff analyse       <dir>
    SD_MODE_LINT,           // ./srcdiff lint          <dir>
    SD_MODE_REGISTER,       // ./srcdiff register      <name>    <dir>

    // LUT Configuration Modes
    SD_MODE_LUT_VALIDATE,   // ./srcdiff lut validate
    SD_MODE_LUT_INVALIDATE, // ./srcdiff lut invalidate
    SD_MODE_LUT_SET,        // ./srcdiff lut set        <config>
    SD_MODE_LUT_ADD         // ./srcdiff lut add        <config>
} SD_Mode;

// The global options of SourceDiff - each representing the bit position they configure
typedef enum {
    SD_OPTION_HELP           = 1 << 0, // ./srcdiff --help
    SD_OPTION_VERSION        = 1 << 1, // ./srcdiff --version
    SD_OPTION_LIST_LANGUAGES = 1 << 2, // ./srcdiff --list-languages
    SD_OPTION_VERBOSE        = 1 << 3, // ./srcdiff -v        ...
    SD_OPTION_OUTPUT         = 1 << 4  // ./srcdiff -o <file> ...
} SD_Option;

#endif // SRCDIFF_TYPES_H
