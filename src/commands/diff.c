#include <assert.h>
#include <stdint.h>
#include <windows.h>

#include "srcdiff.h"

/// Computes the difference between two file, where: F1 - F2
SDPUBLIC
int32_t sd_exec_diff(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_DIFF);
    return 0;
}
