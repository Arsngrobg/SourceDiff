#include "srcdiff_stdutils.h"
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

/// Overrides the current directory to this one - absolute/relative & returns a voltatile filepath buffer
SDPUBLIC
const char *sd_chdir(const char *fmt, ...) {
    SDPRIVATE char pathbuf[SRCDIFF_MAXPATH];
    assert(fmt != NULL);

    va_list vargs;
    va_start(vargs, fmt);
    vsnprintf(pathbuf, SRCDIFF_MAXPATH, fmt, vargs);
    va_end(vargs);

    chdir(pathbuf);
    return pathbuf;
}

/// Gets the working directory of the binary
SDPUBLIC
const char *sd_getbwd(void) {
    SDPRIVATE char bindir[MAX_PATH];
    SDPRIVATE bool available = false;

    if (available)
        goto short_circuit;

    if (_sd_getbwd(bindir, SRCDIFF_MAXPATH) == 0) {
        sd_log_warn("failed to retrieve binary directory data");
        available = false;
    } else available = true;

    char *last_slash = strrchr(bindir, '\\');
    if (last_slash)
        *last_slash = '\0';

    sd_log_debug("got binary directory: %s", bindir);

short_circuit:
    return available ? bindir : NULL;
}

/// Gets the working directory of the user
SDPUBLIC
const char *sd_getcwd(void) {
    SDPRIVATE char usrdir[MAX_PATH];
    SDPRIVATE bool available = false;

    if (available)
        goto short_circuit;

    if (_sd_getcwd(usrdir, SRCDIFF_MAXPATH) == 0) {
        sd_log_error("failed to retrieve user directory data");
        available = false;
    } else available = true;

    sd_log_debug("got user directory: %s", usrdir);

short_circuit:
    return available ? usrdir : NULL;
}

/// Sets the current working directory to the location of this executable
SDPUBLIC
void sd_set_scope_binary(void) {
    const char *bindir;
    if ((bindir = sd_getbwd()) == NULL)
        return;

    if (_sd_chdir(bindir) == 0)
        sd_log_debug("entering binary working directory");
}

/// Sets the current working directory to the location of this executable
SDPUBLIC
void sd_set_scope_user(void) {
    const char *usrdir;
    if ((usrdir = sd_getcwd()) == NULL)
        return;

    if (_sd_chdir(usrdir) == 0)
        sd_log_debug("entering user working directory");
}

/// Whether the file exists or not - uses format string for ergonomics & returns a voltatile filepath buffer
SDPUBLIC
const char *sd_file_exists(const char *fmt, ...) {
    SDPRIVATE char pathbuf[SRCDIFF_MAXPATH];
    assert(fmt != NULL);

    va_list vargs;
    va_start(vargs, fmt);
    vsnprintf(pathbuf, SRCDIFF_MAXPATH, fmt, vargs);
    va_end(vargs);

    struct stat st;
    return (stat(pathbuf, &st) == 0) ? pathbuf : NULL;
}

/// Reads the entire file and returns the pointer to the string - must be managed yourself
SDPUBLIC
char *sd_read_entire_file(const char *fmt, ...) {
    assert(fmt != NULL);

    va_list vargs;
    char pathbuf[SRCDIFF_MAXPATH];

    va_start(vargs, fmt);
    vsnprintf(pathbuf, SRCDIFF_MAXPATH, fmt, vargs);

    char *file_contents = NULL;
    FILE *file = fopen(pathbuf, "r");
    if (file == NULL) {
        sd_log_error("unable to open the file '%s'", pathbuf);
        goto defer;
    }
    fseek(file, 0, SEEK_END);
    size_t flen = ftell(file);

    file_contents = malloc(sizeof(char) * (flen + 1));
    fwrite(file_contents, sizeof(char), flen, file);
    file_contents[flen] = '\0';

defer:
    va_end(vargs);
    fclose(file);
    return file_contents;
}
