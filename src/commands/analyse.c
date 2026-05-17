#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "srcdiff.h"

/// Executes structural analysis of the files defined by the glob pattern
SDPUBLIC
int32_t sd_exec_analyse(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_ANALYSE);
    return EXIT_SUCCESS;
}
