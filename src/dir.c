#include <stdbool.h>

#include "tree_sitter/api.h"

#include "srcdiff.h"

#ifdef _WIN32
#   include <windows.h>
#   include <direct.h>
#   define SRCDIFF_MAXPATH MAX_PATH
#   define _sd_getbwd(path, size) GetModuleFileNameA(NULL, path, size)
#   define _sd_getcwd             _getcwd
#   define _sd_chdir              _chdir
#else
#   include <limits.h>
#   include <unistd.h>
#   define SRCDIFF_MAXPATH PATH_MAX
#   define _sd_getbwd(path, size) readlink("/proc/self/exe", path, size)
#   define _sd_getcwd             getcwd
#   define _sd_chdir              chdir
#endif // _WIN32

SDPRIVATE
const char *sd_getbwd(void) {
    SDPRIVATE char bindir[MAX_PATH];
    SDPRIVATE bool available = false;

    if (available)
        goto short_circuit;

    if (_sd_getbwd(bindir, SRCDIFF_MAXPATH) == 0) {
        sd_log_error("failed to retrieve binary directory data");
        available = false;
    } else available = true;

    char *last_slash = strrchr(bindir, '\\');
    if (last_slash)
        *last_slash = '\0';

    sd_log_debug("got binary directory: %s", bindir);

short_circuit:
    return available ? bindir : NULL;
}

SDPRIVATE
const char *sd_getcwd(void) {
    SDPRIVATE char usrdir[MAX_PATH];
    SDPRIVATE bool available = false;

    if (available)
        goto short_circuit;

    if (_sd_getcwd(usrdir, SRCDIFF_MAXPATH) == 0) {
        sd_log_error("failed to retrieve user directory data");
        available = false;
    } else available = true;

    char *last_slash = strrchr(usrdir, '\\');
    if (last_slash)
        *last_slash = '\0';

    sd_log_debug("got user directory: %s", usrdir);

short_circuit:
    return available ? usrdir : NULL;
}

/// Sets the current working directory to the location of this executable
SDPUBLIC
bool sd_enter_bin_dir(void) {
    const char *bindir;
    if ((bindir = sd_getbwd()) == NULL)
        return false;

    sd_log_debug("entering binary working directory");
    return _sd_chdir(bindir) == 0;
}

/// Sets the current working directory to the default
SDPUBLIC
bool sd_exit_bin_dir(void) {
    const char *usrdir;
    if ((usrdir = sd_getcwd()) == NULL)
        return false;

    sd_log_debug("entering user working directory");
    return _sd_chdir(usrdir) == 0;
}
