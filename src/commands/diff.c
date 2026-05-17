#include "srcdiff_stdutils.h"
#include "srcdiff.h"

SDPRIVATE
int32_t sd_exec_structural_diff(const char *file1, const char *file2) {
    assert(file1 != NULL && file2 != NULL);
}

/// Computes the difference between two file, where: F1 - F2
SDPUBLIC
int32_t sd_exec_diff(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_DIFF);

    const char *file1 = sd_get_arg(0);
    const char *file2 = sd_get_arg(1);

    const char *file1_ext = strrchr(file1, '.');
    const char *file2_ext = strrchr(file2, '.');

    // TODO: default plaintext mode (probably line-by-line diff)
    // both files have no extension
    if (file1_ext == NULL && file2_ext == NULL) {
        sd_log_warn("both files do not have a file extension - hence cannot be represented by a language");
    }

    return 0;
}
