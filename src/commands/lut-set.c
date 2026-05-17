#include "srcdiff_stdutils.h"
#include "srcdiff.h"

/// Sets the persistent lookup table using the high-level DSL
SDPUBLIC
int32_t sd_exec_lut_set(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_LUT && sd_get_lut_command() == SD_LUT_COMMAND_SET);

    int32_t status = EXIT_SUCCESS;

    SDLut *lut = sd_empty_lut();
    if (lut == NULL) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    status = (int32_t) !(sd_lut_parse(sd_get_arg(0), lut) && sd_write_lut(lut));

short_circuit:
    free(lut);
    return status;
}
