#include <assert.h>
#include <stdlib.h>

#include "srcdiff.h"

/// Adds the high-level DSL to the persistent lookup table
SDPUBLIC
int32_t sd_exec_lut_add(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_LUT && sd_get_lut_command() == SD_LUT_COMMAND_ADD);

    int32_t status = EXIT_SUCCESS;

    SDLut *lut = sd_empty_lut();
    if (lut == NULL) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    SDLut *plut = sd_get_lut();
    if (plut == NULL) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    status = (int32_t) !(
           sd_lut_parse(sd_get_arg(0), lut)
        && sd_lut_add  (lut, plut)
        && sd_write_lut(lut)
    );

short_circuit:
    free(lut);
    return status;
}
