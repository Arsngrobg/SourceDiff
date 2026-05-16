#ifndef SRCDIFF_ARGV_H
#define SRCDIFF_ARGV_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/// The modes of SourceDiff
typedef enum {
    SD_MODE_NONE,             // ./srcdiff                                <options>
    SD_MODE_DIFF,             // ./srcdiff diff           <file>   <file>
    SD_MODE_ANALYSE,          // ./srcdiff analyse        <glob>
    SD_MODE_LINT,             // ./srcdiff lint           <glob>
    SD_MODE_REGISTER,         // ./srcdiff register       <name>   <src>
    SD_MODE_LUT_INFO,         // ./srcdiff lut info                       <options>
    SD_MODE_LUT_SET,          // ./srcdiff lut set        <config>        <options>
    SD_MODE_LUT_ADD,          // ./srcdiff lut add        <config>        <options>
    SD_MODE_LUT_CLEAR,        // ./srcdiff lut clear                      <options>
} SD_Mode;

/// The global options that override behaviour
typedef enum {
    SD_OPTION_HELP,           // ./srcdiff --help
    SD_OPTION_VERSION,        // ./srcdiff --version
    SD_OPTION_LIST_LANGUAGES, // ./srcdiff --list-languages
    SD_OPTION_VERBOSE,        // ./srcdiff -v
    SD_OPTION_OUTPUT          // ./srcdiff -o foo.log
} SD_Option;

/// Parses the `argv`
bool SD_ParseArgv(int32_t argc, const char *argv[]);

/// Gets the name of the executable
const char *SD_GetExecName(void);

/// Gets the argument at `idx`
const char *SD_GetArgv(size_t idx);

/// Gets the output file (if -o flag was used)
FILE *SD_GetOutputFile(void);

/// Gets the configured mode
SD_Mode SD_GetMode(void);

/// Tests to see whether the option is enabled
bool SD_IsOptionSet(SD_Option option);

#ifndef NDEBUG
/// DEBUG: is `argv` parsed?
bool SD_IsArvParsed(void);
#endif // NDEBUG

#endif // SRCDIFF_ARGV_H
