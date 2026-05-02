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
    ((int)(s).length),((s).bytes)

// A length-based, owned string
typedef struct {
    size_t length;
    char   bytes[];
} SD_String;

// A view into a string (either char* or SD_String)
typedef struct {
    size_t      length;
    const char *bytes;
} SD_StringView;

SD_String     *sd_str_ofstrc (const char *cstr);
SD_String     *sd_str_substr (const char *cstr, size_t offset, size_t length);
SD_StringView  sd_str_viewof (SD_String *str, size_t offset, size_t length);
SD_StringView  sd_str_viewofc(const char *cstr, size_t offset, size_t length);

#endif // SDSTRING_H
