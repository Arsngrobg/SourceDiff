#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>

#include "libcc.h"

#include "srcdiff.h"

/// Lists all registered languages - according to the user's platform
SDPUBLIC
int32_t sd_exec_list_languages(void) {
    assert(sd_is_argv_parsed() && sd_is_option_set(SD_OPTION_LIST_LANGUAGES));

    int32_t status = EXIT_SUCCESS;

    if (!sd_enter_bin_dir()) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    struct dirent *entry;
    DIR *dir = opendir(SRCDIFF_LANGSTORE);
    if (dir == NULL) {
        sd_log("no languages registered");
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
    status = !sd_exit_bin_dir();

short_circuit:
    return status;
}
