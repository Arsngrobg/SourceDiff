//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Safe string handling and viewing with a custom string type and views into strings.
//   Strings can references to SD_Strings or regular C strings (like argv from the CLI).

#ifndef SDSTRING_H
#define SDSTRING_H

#include <stddef.h>

#define SD_STRING_FORMAT(s) \
    ((int)(s).length),((s).buf)

// A length-based, owned string
typedef struct {
    size_t length;
    char   buf[];
} SD_String;

// A view into a string (either char* or SD_String)
typedef struct {
    size_t      length;
    const char *buf;
} SD_StringView;

#endif // SDSTRING_H
