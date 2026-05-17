#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "srcdiff.h"

/// Outputs information about the persistent lookup table
SDPUBLIC
int32_t sd_exec_lut_info(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_LUT && sd_get_lut_command() == SD_LUT_COMMAND_INFO);

    SDLut  *lut        = sd_get_lut        ();
    size_t  lang_count = sd_lut_key_count  (lut);
    size_t  ext_count  = sd_lut_value_count(lut);

    sd_log("lookup table configuration:");
    printf(" - languages mapped (" ANSI_INFO "%lld" ANSI_RESET ")\n", lang_count);
    for (size_t idx = 0; idx < lang_count; idx++) {
        printf("   - %s\n", sd_lut_key_at(lut, idx));
    }
    printf(" - extensions mapped (" ANSI_INFO "%lld" ANSI_RESET ")\n", ext_count);
    for (size_t idx = 0; idx < ext_count; idx++) {
        const char *value = sd_lut_value_at(lut, idx);
        if (sd_is_option_set(SD_OPTION_VERBOSE)) {
            printf("   - .%s -> %s\n", value, sd_lut_mapping_for(lut, value));
        } else {
            printf("   - .%s\n", value);
        }
    }

    return EXIT_SUCCESS;
}
