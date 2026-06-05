#include "srcdiff_stdutils.h"
#include "srcdiff.h"

/// Executes stylistic analysis of the file sequence
SDPUBLIC
int32_t sd_exec_lint(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_LINT);
    printf("%p\n", (void*) sd_load_language("python"));
    return EXIT_SUCCESS;
}
