#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "sddebug.h"
#include "sdstring.h"

SD_String *sd_str_ofstrc(const char *cstr) {
    assert(cstr != NULL);

    size_t len = strlen(cstr);
    SD_String *str = malloc(sizeof(SD_String) + sizeof(char) * len);
    if (str == NULL) {
        fprintf(stderr, "SourceDiff: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    str->length = len;
    memcpy(str->bytes, cstr, len);
    return str;
}

SD_String *sd_str_substr(const char *cstr, size_t offset, size_t length) {
    assert(cstr != NULL); assert(strlen(cstr + offset) >= length);

    SD_String *str = malloc(sizeof(SD_String) + sizeof(char) * (len - offset));
    if (str == NULL) {
        fprintf(stderr, "SourceDiff: \x1b[1;31merror:\x1b[0m out of memory error\n");
        return NULL;
    }

    str->length = length;
    memcpy(str->bytes, cstr + offset, length)
    return str;
}

SD_StringView sd_str_viewof(SD_String *str, size_t offset, size_t length) {
    assert(str != NULL);
    return sd_str_viewofc(str->bytes, offset, length);
}

SD_StringView sd_str_viewofc(const char *cstr, size_t offset, size_t length) {
    assert(cstr != NULL); assert(strlen(cstr + offset) >= length);
    return (SD_StringView) { length, (cstr + offset) };
}
