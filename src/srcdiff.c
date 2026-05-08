//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   The main driver of the application

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>

#include "libcc.h"

#include "srcdiff.h"

#if !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)
#error SD_VERSION, SD_DESCRIPTION, or SD_DOCS are not defined!
#endif // !defined(SD_VERSION) || !defined(SD_DESCRIPTION) || !defined(SD_DOCS)

#define SD_HELP_STRING                                                                      \
    "SourceDiff v"SD_VERSION" - "SD_DESCRIPTION"\n"                                         \
    "Copyright (c) 2025 James Armstrong (Arsngrobg)\n"                                      \
    "\n"                                                                                    \
    "Usage:\n"                                                                              \
    "  %s diff     <file> <file>   The difference between both files\n"                     \
    "  %s analyse  <glob>          Structural analysis of the files which match the glob\n" \
    "  %s lint     <glob>          Style analysis of the files which match the glob\n"      \
    "  %s register <name> <dir>    Registers a new language (C compiler required)\n"        \
    "\n"                                                                                    \
    "Options:\n"                                                                            \
    "  --help             Display this information\n"                                       \
    "  --version          Display version information for SourceDiff\n"                     \
    "  --list-languages   Lists all registered language parsers\n"                          \
    "  -v                 Display diagnostics during analysis\n"                            \
    "  -o <file>          Output digestible, structured analysis results to the <file>\n"   \
    "\n"                                                                                    \
    "For more information: "SD_DOCS"\n"

int32_t SD_Exec(void) {
    assert(SD_CLArgsParsed());

    int32_t status = EXIT_SUCCESS;

    // these arguments have higher priority
    if (SD_IsOptionSet(SD_OPTION_HELP)) {
        fprintf(
            stdout,
            SD_HELP_STRING,
            SD_GetExecName(), SD_GetExecName(), SD_GetExecName(), SD_GetExecName()
        );
        goto short_circuit;
    } else if (SD_IsOptionSet(SD_OPTION_VERSION)) {
        fprintf(stdout, "v%s\n", SD_VERSION);
        goto short_circuit;
    } else if (SD_IsOptionSet(SD_OPTION_LIST_LANGUAGES)) {
        struct dirent *entry;
        DIR *dir = opendir("languages");
        if (dir == NULL) {
            printf("%s: no languages registered\n", SD_GetExecName());
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
            SD_Log("%s: registered languages: \x1b[1;32m%lld\x1b[0m\n", SD_GetExecName(), amount);
            while ((entry = readdir(dir)) != NULL) {
                char *dot = strrchr(entry->d_name, '.');
                if (dot == NULL || strcmp(dot+1, CC_SHARED_LIB_EXT) != 0) {
                    continue;
                }
                *dot = '\0';

                printf(" - %s\n", entry->d_name);
            }
        }

        closedir(dir);

        status = EXIT_SUCCESS;
        goto short_circuit;
    }

    switch (SD_GetMode()) {
        case SD_MODE_NONE:
            SD_LogError("no mode specified");
            status = EXIT_FAILURE;
            break;
        case SD_MODE_DIFF:
            SD_LogDebug("Entering DIFF mode");
            SD_LogError("'diff' mode not implemented (TODO)");
            status = EXIT_FAILURE; // SD_ExecDiff();
            break;
        case SD_MODE_ANALYSE:
            SD_LogDebug("Entering ANALYSE mode");
            SD_LogError("'analyse' mode not implemented (TODO)");
            status = EXIT_FAILURE; // SD_ExecAnalyse();
            break;
        case SD_MODE_LINT:
            SD_LogDebug("Entering LINT mode");
            SD_LogError("'lint' mode not implemented (TODO)");
            status = EXIT_FAILURE; // SD_ExecLint();
            break;
        case SD_MODE_REGISTER:
            SD_LogDebug("Entering REGISTER mode");
            status = SD_ExecRegister();
            break;
    }

short_circuit:
    return status;
}

int32_t main(int32_t argc, const char *argv[]) {
    return SD_ParseCLArgs(argc, argv) ? SD_Exec() : EXIT_FAILURE;
}
