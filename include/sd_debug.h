//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Debugging macros used in debug builds

#ifndef SD_DEBUG_H
#define SD_DEBUG_H

// this header includes useful macros for debugging:
// - SD_LOG(fmt, ...): a logging macro, contains function it was instered into
// - SD_UNUSED(o):     explict declaration of an unused variable (bypass -Wunused)

#ifndef NDEBUG
#include <assert.h>
#include <stdio.h>
#define SD_LOG(fmt, ...) printf("[DEBUG::%s] "fmt"\n", __func__, ##__VA_ARGS__)
#define SD_UNUSED(o)     ((void)o)
#else
#define SD_LOG(fmt, ...)
#define SD_UNUSED(str)
#endif // NDEBUG

#endif // SD_DEBUG_H
