#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "srcdiff.h"

/// Executes stylistic analysis of the file sequence
SDPUBLIC
int32_t sd_exec_lint(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_ANALYSE);
    return EXIT_SUCCESS;
}
