#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "logging.h"
#include "lut.h"
#include "commands.h"

/// Validates the persistent lookup table to make sure file integrity is ok
int32_t SD_Exec_LutInfo(void) {
    assert(SD_IsArvParsed() && SD_GetMode() == SD_MODE_LUT_INFO);

    SD_Lut *lut        = SD_GetLut();
    size_t  lang_count = SD_LutKeyCount(lut);
    size_t  ext_count  = SD_LutValueCount(lut);

    SD_Log("lookup table configuration:");
    printf(" - languages mapped (" ANSI_INFO "%lld" ANSI_RESET ")\n", lang_count);
    for (size_t idx = 0; idx < lang_count; idx++) {
        printf("   - %s\n", SD_LutKeyAt(lut, idx));
    }
    printf(" - extensions mapped (" ANSI_INFO "%lld" ANSI_RESET ")\n", ext_count);
    for (size_t idx = 0; idx < ext_count; idx++) {
        const char *value = SD_LutValueAt(lut, idx);
        if (SD_IsOptionSet(SD_OPTION_VERBOSE)) {
            printf("   - .%s -> %s\n", value, SD_LutMappingFor(lut, value));
        } else {
            printf("   - .%s\n", value);
        }
    }

    return EXIT_SUCCESS;
}
