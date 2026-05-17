#include <assert.h>
#include <string.h>
#include <dirent.h>

#include "libcc.h"

#include "srcdiff.h"

#if !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)
#error SD_VERSION, SD_DESCRIPTION, or SD_DOCS are not defined!
#endif // !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)

#define SD_HELP_STRING                                                                    \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                       \
    "Copyright (c) 2025 James Armstrong (Arsngrobg)\n"                                    \
    "\n"                                                                                  \
    "Usage:\n"                                                                            \
    "  %s diff     <file>  <file>   The difference between both files\n"                  \
    "  %s analyse  <files>          Structural analysis of the files\n"                   \
    "  %s lint     <files>          Style analysis of the files which match the glob\n"   \
    "  %s register <name>  <dir>    Registers a new language (C compiler required)\n"     \
    "\n"                                                                                  \
    "Lookup Table Configuration:\n"                                                       \
    "  %s lut info                 Retrieves data about the lookup table\n"               \
    "  %s lut [set|add] <config>   Sets/adds the lookup table entries using the config\n" \
    "  %s lut clear                Clears the lookup table\n"                             \
    "\n"                                                                                  \
    "Options:\n"                                                                          \
    "  --help             Display this information\n"                                     \
    "  --version          Display version information for SourceDiff\n"                   \
    "  --list-languages   Lists all registered language parsers\n"                        \
    "  -v                 Display extra information during execution\n"                   \
    "  -o <file>          Output digestible, structured analysis results to the <file>\n" \
    "\n"                                                                                  \
    "For usage where <files> are required, the application will greedily consume all\n"   \
    "tokens, including all option tokens - be careful.\n"                                 \
    "\n"                                                                                  \
    "For more information: "SD_DOCS"\n"

int32_t sd_exec(void) {
    assert(sd_is_argv_parsed());

    int32_t status = EXIT_SUCCESS;

    // these arguments have higher priority
    if (sd_is_option_set(SD_OPTION_HELP)) {
        const char *exec_name = sd_exec_name();
        fprintf(stdout,
            SD_HELP_STRING,
            exec_name, exec_name, exec_name, exec_name,
            exec_name, exec_name, exec_name
        );
        goto short_circuit;
    } else if (sd_is_option_set(SD_OPTION_VERSION)) {
        fprintf(stdout, "v%s\n", SD_VERSION);
        goto short_circuit;
    } else if (sd_is_option_set(SD_OPTION_LIST_LANGUAGES)) {
        struct dirent *entry;
        DIR *dir = opendir("languages");
        if (dir == NULL) {
            sd_log("no languages registered");
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
            sd_log("registered languages: " ANSI_INFO "%lld" ANSI_RESET, amount);
            while ((entry = readdir(dir)) != NULL) {
                char *dot = strrchr(entry->d_name, '.');
                if (dot == NULL || strcmp(dot+1, CC_SHARED_LIB_EXT) != 0) {
                    continue;
                }
                *dot = '\0';

                fprintf(stdout, " - %s\n", entry->d_name);
            }
        }

        closedir(dir);

        status = EXIT_SUCCESS;
        goto short_circuit;
    }

    switch (sd_get_mode()) {
        case SD_MODE_NONE:
            sd_log_error("no mode specified");
            status = EXIT_FAILURE;
            break;
        case SD_MODE_DIFF:
            sd_log_debug("Entering DIFF mode");
            sd_log_error("'diff' mode not implemented (TODO)");
            status = EXIT_FAILURE; // sd_exec_diff();
            break;
        case SD_MODE_ANALYSE:
            sd_log_debug("Entering ANALYSE mode");
            sd_log_error("'analyse' mode not implemented (TODO)");
            status = EXIT_FAILURE; // sd_exec_analyse();
            break;
        case SD_MODE_LINT:
            sd_log_debug("Entering LINT mode");
            sd_log_error("'lint' mode not implemented (TODO)");
            status = EXIT_FAILURE; // sd_exec_lint();
            break;
        case SD_MODE_REGISTER:
            sd_log_debug("Entering REGISTER mode");
            status = sd_exec_register();
            break;
        case SD_MODE_LUT:
            switch (sd_get_lut_command()) {
                case SD_LUT_COMMAND_NONE:
                    sd_log_error("no lut command specified");
                    status = EXIT_FAILURE;
                    break;
                case SD_LUT_COMMAND_INFO:
                    sd_log_debug("Entering LUT INFO mode");
                    status = sd_exec_lut_info();
                    break;
                case SD_LUT_COMMAND_SET:
                    sd_log_debug("Entering LUT SET mode");
                    status = sd_exec_lut_set();
                    break;
                case SD_LUT_COMMAND_ADD:
                    sd_log_debug("Entering LUT ADD mode");
                    status = sd_exec_lut_add();
                    break;
                case SD_LUT_COMMAND_CLEAR:
                    sd_log_debug("Entering LUT CLEAR mode");
                    status = sd_exec_lut_clear();
                    break;
            }
    }

short_circuit:
    return status;
}

int32_t main(int32_t argc, const char *argv[]) {
    return sd_parse_argv(argc, argv) ? sd_exec() : EXIT_FAILURE;
}
