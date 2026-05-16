#include <assert.h>
#include <stdlib.h>

#include "logging.h"
#include "argv.h"
#include "lut.h"

/// Adds the high-level DSL to the persistent lookup table
int32_t SD_Exec_LutAdd(void) {
    assert(SD_IsArvParsed() && SD_GetMode() == SD_MODE_LUT_ADD);

    int32_t status = EXIT_SUCCESS;

    SD_Lut *lut = SD_EmptyLut();
    if (lut == NULL) {
        status = EXIT_FAILURE;
        goto short_circuit;
    }

    status = (int32_t) !(
           SD_LutParseDSL(SD_GetArgv(0), lut)
        && SD_LutAdd(lut, SD_GetLut())
        && SD_WriteLut(lut)
    );

short_circuit:
    free(lut);
    return status;
}
