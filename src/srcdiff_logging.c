//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Custom logging

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "srcdiff.h"

#define SD_MAXLOG (2048) // The maximum length of a log message

void SD_Log(const char *fmt, ...) {
    char buffer[SD_MAXLOG];

    va_list vargs;
    va_start(vargs, fmt);
    size_t length = vsnprintf(buffer, SD_MAXLOG, fmt, vargs);
    buffer[length] = '\0';
    va_end(vargs);

    printf("%s: %s\n", SD_GetExecName(), buffer);
}

void SD_LogWarn(const char *fmt, ...) {
    char buffer[SD_MAXLOG];

    va_list vargs;
    va_start(vargs, fmt);
    size_t length = vsnprintf(buffer, SD_MAXLOG, fmt, vargs);
    buffer[length] = '\0';
    va_end(vargs);

    fprintf(stderr, "%s: \x1b[1;33mwarning:\x1b[0m %s\n", SD_GetExecName(), buffer);
}

void SD_LogError(const char *fmt, ...) {
    char buffer[SD_MAXLOG];

    va_list vargs;
    va_start(vargs, fmt);
    size_t length = vsnprintf(buffer, SD_MAXLOG, fmt, vargs);
    buffer[length] = '\0';
    va_end(vargs);

    fprintf(stderr, "%s: \x1b[1;31merror:\x1b[0m %s\n", SD_GetExecName(), buffer);
}

void SD_LogDebug(const char *fmt, ...) {
#ifndef NDEBUG
    char buffer[SD_MAXLOG];

    va_list vargs;
    va_start(vargs, fmt);
    size_t length = vsnprintf(buffer, SD_MAXLOG, fmt, vargs);
    buffer[length] = '\0';
    va_end(vargs);

    printf("%s: \x1b[36mdebug:\x1b[0m %s\n", SD_GetExecName(), buffer);
#else
    return;
#endif // NDEBUG
}
