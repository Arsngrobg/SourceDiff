#include "srcdiff_stdutils.h"
#include "srcdiff.h"

/// Destroys the persistent lookup table to default configuration (equivalent to 'lut set {}')
SDPUBLIC
int32_t sd_exec_lut_clear(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_LUT && sd_get_lut_command() == SD_LUT_COMMAND_CLEAR);

    int32_t status = EXIT_SUCCESS;

    SDLut *lut = sd_empty_lut();
    if (lut == NULL) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    status = (int32_t) !sd_write_lut(lut);

short_circuit:
    free(lut);
    return status;
}
