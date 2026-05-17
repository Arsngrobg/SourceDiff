#ifndef SRCDIFF_H
#define SRCDIFF_H

#define SDPUBLIC  extern // Explicitly describes a function as a local function
#define SDPRIVATE static // Explicitly describes a function as a local function

// =============================================================================
//                                   Logging
// =============================================================================

#define ANSI_PREFIX "\x1b["
#define ANSI_SUFFIX "m"
#define ANSI_RESET  ANSI_PREFIX "0"    ANSI_SUFFIX
#define ANSI_ERROR  ANSI_PREFIX "1;31" ANSI_SUFFIX
#define ANSI_INFO   ANSI_PREFIX "1;32" ANSI_SUFFIX
#define ANSI_WARN   ANSI_PREFIX "1;33" ANSI_SUFFIX
#define ANSI_DEBUG  ANSI_PREFIX "1;36" ANSI_SUFFIX

/// Default logging function for any regular output
#define sd_log(fmt, ...) \
    fprintf(stdout, "%s: "fmt"\n", sd_exec_name(), ##__VA_ARGS__)

/// Warning messages and any issues that are not 100% detrimental to the lifetime of the program
#define sd_log_warn(fmt, ...) \
    fprintf(stderr, "%s: "ANSI_WARN"warn:"ANSI_RESET" "fmt"\n", sd_exec_name(), ##__VA_ARGS__)

/// Evil messages for really bad stuff
#define sd_log_error(fmt, ...) \
    fprintf(stderr, "%s: "ANSI_ERROR"error:"ANSI_RESET" "fmt"\n", sd_exec_name(), ##__VA_ARGS__)

/// Informative messages only when verbose is enabled
#define sd_log_info(fmt, ...)                                                                           \
    do {                                                                                                \
        if (sd_is_option_set(SD_OPTION_VERBOSE))                                                        \
            fprintf(stdout, "%s: "ANSI_INFO"info:"ANSI_RESET" "fmt"\n", sd_exec_name(), ##__VA_ARGS__); \
    } while (0)

/// Debug-only logging
#ifndef NDEBUG
#define sd_log_debug(fmt, ...) \
    fprintf(stdout, "%s: "ANSI_DEBUG"debug (%s):"ANSI_RESET" "fmt"\n", sd_exec_name(), __func__, ##__VA_ARGS__)
#else
#define sd_log_debug(fmt, ...) \
    ((void)fmt) // noop
#endif // NDEBUG

// =============================================================================
//                          Command Line Processing
// =============================================================================

/// The modes of SourceDiff
typedef enum {
    SD_MODE_NONE,             // ./srcdiff                                <options>
    SD_MODE_DIFF,             // ./srcdiff diff           <file>   <file> <options>
    SD_MODE_ANALYSE,          // ./srcdiff analyse        <glob>          <options>
    SD_MODE_LINT,             // ./srcdiff lint           <glob>          <options>
    SD_MODE_REGISTER,         // ./srcdiff register       <name>   <src>  <options>
    SD_MODE_LUT               // ./srcdiff lut ...
} SDMode;

/// The commands relating to the persistent lookup table
typedef enum {
    SD_LUT_COMMAND_NONE,      // ./srcdiff lut                            <options>
    SD_LUT_COMMAND_INFO,      // ./srcdiff lut info                       <options>
    SD_LUT_COMMAND_SET,       // ./srcdiff lut set                        <options>
    SD_LUT_COMMAND_ADD,       // ./srcdiff lut add                        <options>
    SD_LUT_COMMAND_CLEAR      // ./srcdiff lut clear                      <options>
} SDLutCommand;

/// The global options that override behaviour
typedef enum {
    SD_OPTION_HELP,           // ./srcdiff --help           ...
    SD_OPTION_VERSION,        // ./srcdiff --version        ...
    SD_OPTION_LIST_LANGUAGES, // ./srcdiff --list-languages ...
    SD_OPTION_VERBOSE,        // ./srcdiff -v               ...
    SD_OPTION_OUTPUT          // ./srcdiff -o log           ...
} SDOption;

/// Parses the raw `argv`
SDPUBLIC
bool sd_parse_argv(int32_t argc, const char *argv[]);

/// Gets the name of the executable
SDPUBLIC
const char *sd_exec_name(void);

/// Gets the argument at `idx`
SDPUBLIC
const char *sd_get_arg(size_t idx);

/// Gets the output file (if -o flag was used)
SDPUBLIC
FILE *sd_get_output_file(void);

/// Gets the configured mode
SDPUBLIC
SDMode sd_get_mode(void);

/// Gets the LUT configuration command
SDPUBLIC
SDLutCommand sd_get_lut_command(void);

/// Tests to see whether the option is enabled
SDPUBLIC
bool sd_is_option_set(SDOption option);

#ifndef NDEBUG
/// Debug function for assertions
SDPUBLIC
bool sd_is_argv_parsed(void);
#endif // NDEBUG

// =============================================================================
//                              Execution Modes
// =============================================================================

/// Computes the difference between two file, where: F1 - F2
SDPUBLIC
int32_t sd_exec_diff(void);

/// Executes structural analysis of the file sequence
SDPUBLIC
int32_t sd_exec_analyse(void);

/// Executes stylistic analysis of the file sequence
SDPUBLIC
int32_t sd_exec_lint(void);

/// Registers a Tree Sitter grammar to SourceDiff
SDPUBLIC
int32_t sd_exec_register(void);

/// Outputs information about the persistent lookup table
SDPUBLIC
int32_t sd_exec_lut_info(void);

/// Sets the persistent lookup table using the high-level DSL
SDPUBLIC
int32_t sd_exec_lut_set(void);

/// Adds the high-level DSL to the persistent lookup table
SDPUBLIC
int32_t sd_exec_lut_add(void);

/// Destroys the persistent lookup table to default configuration (equivalent to 'lut set {}')
SDPUBLIC
int32_t sd_exec_lut_clear(void);

/// Lists all registered languages - according to the user's platform
SDPUBLIC
int32_t sd_exec_list_languages(void);

// =============================================================================
//                        Lookup Table for Language Grammars
// =============================================================================

/// A lookup table for matching file extensions to their language parsers
typedef struct SDLut SDLut;

/// Overwrites the persistent LUT with this one
SDPUBLIC
bool sd_write_lut(const SDLut *lut);

/// Gets the persistent lookup table (empty if none exists)
SDPUBLIC
SDLut *sd_get_lut(void);

/// Allocates an empty lookup table
SDPUBLIC
SDLut *sd_empty_lut(void);

/// The number of keys in the lookup table
SDPUBLIC
size_t sd_lut_key_count(const SDLut *lut);

/// The number of values in the lookup table
SDPUBLIC
size_t sd_lut_value_count(const SDLut *lut);

/// Whether the lookup table contains the key
SDPUBLIC
bool sd_lut_has_key(const SDLut *lut, const char *key);

/// Whether the lookup table contains the value
SDPUBLIC
bool sd_lut_has_value(const SDLut *lut, const char *value);

/// The key from the lookup table
SDPUBLIC
const char *sd_lut_key_at(const SDLut *lut, size_t idx);

/// The value from the lookup table
SDPUBLIC
const char *sd_lut_value_at(const SDLut *lut, size_t idx);

/// The mapped key for the given value in the lookup table
SDPUBLIC
const char *sd_lut_mapping_for(const SDLut *lut, const char *value);

/// Adds the given lookup table to the persistent lookup table
SDPUBLIC
bool sd_lut_add(SDLut *dst, const SDLut *src);

/// Parse the DSL into a tangible SD_Lut object
SDPUBLIC
bool sd_lut_parse(const char *dsl, SDLut *lut);

// =============================================================================
//                              Language Loading
// =============================================================================

/// The directory where languages are dumped into
#define SRCDIFF_LANGSTORE "languages/"

/// Loads the language specified by the `lang_name`
SDPUBLIC
const TSLanguage *sd_load_language(const char *lang_name);

/// Deallocates the memory used by the language registry
SDPUBLIC
void sd_clear_languages(void);

// =============================================================================
//                     Working Directory Control & Query
// =============================================================================

/// Sets the current working directory to the location of this executable
SDPUBLIC
void sd_enter_bin_dir(void);

/// Sets the current working directory to the default
SDPUBLIC
void sd_exit_bin_dir(void);

/// Whether the file exists or not - uses format string for ergonomics & returns a voltatile filepath buffer
SDPUBLIC
const char *sd_file_exists(const char *fmt, ...);

#endif // SRCDIFF_H
