#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "lut.h"

#define SD_LUT_DUMPFILE    "LUT"                                // the file that the persistent lookup table is stored
#define SD_LUT_MAXSTRLEN   (32)                                 // the maximum length of a string in a lookup table
#define SD_LUT_MAXKEYS     (256)                                // the maximum number of keys in a lookup table
#define SD_LUT_MAXVALUES   (256)                                // the maximum number of values in a lookup table

// the lookup table representation in memory
struct SD_Lut {
    char    keys   [SD_LUT_MAXKEYS]  [SD_LUT_MAXSTRLEN+1]; // the keys
    char    vals   [SD_LUT_MAXVALUES][SD_LUT_MAXSTRLEN+1]; // the values
    uint8_t mapping[SD_LUT_MAXVALUES][2];                  // 1:1 associative array of values to keys
};

/// Overwrites the persistent LUT with this one
bool SD_WriteLut(const SD_Lut *lut) {
    assert(lut != NULL);

    // TODO: add proper error detection
    FILE *lut_file = fopen(SD_LUT_DUMPFILE, "w+b");
    if (lut_file == NULL) {
        SD_LogError("unable to obtain a handle to the LUT file");
        return false;
    }

    if (fwrite(lut, sizeof(SD_Lut), 1, lut_file) != 1) {
        SD_LogError("unable to write to the LUT file");
        return false;
    }
    fclose(lut_file);
    return true;
}

/// Gets the persistent lookup table (empty if none exists)
SD_Lut *SD_GetLut(void) {
    static SD_Lut lut    = {0};
    static bool   loaded = false;

    if (!loaded) {
        FILE *lut_file = fopen(SD_LUT_DUMPFILE, "rb");
        if (lut_file != NULL) {
            if (fread(&lut, 1, sizeof(SD_Lut), lut_file) != sizeof(SD_Lut)) {
                SD_LogWarn("Unable to properly read the LUT file");
            }
            fclose(lut_file);
        }
        loaded = true;
    }

    return &lut;
}

/// Allocates an empty lookup table
SD_Lut *SD_EmptyLut(void) {
    SD_Lut *lut = calloc(1, sizeof(SD_Lut)); // 0 init for parity
    if (lut == NULL) {
        SD_LogError("out of system memory");
    }

    return lut;
}

/// The number of keys in the lookup table
size_t SD_LutKeyCount(const SD_Lut *lut) {
    assert(lut != NULL);

    size_t count = 0;
    for (size_t idx = 0; idx < SD_LUT_MAXKEYS && lut->keys[idx][0] != '\0'; idx++) {
        count++;
    }
    return count;
}

/// The number of values in the lookup table
size_t SD_LutValueCount(const SD_Lut *lut) {
    assert(lut != NULL);

    size_t count = 0;
    for (size_t idx = 0; idx < SD_LUT_MAXVALUES && lut->vals[idx][0] != '\0'; idx++) {
        count++;
    }
    return count;
}

/// Whether the lookup table contains the key
bool SD_LutContainsKey(const SD_Lut *lut, const char *key) {
    assert(lut != NULL && key != NULL);

    size_t keyc = SD_LutKeyCount(lut);
    for (size_t idx = 0; idx < keyc; idx++) {
        if (strcmp(lut->keys[idx], key) == 0) {
            return true;
        }
    }
    return false;
}

/// Whether the lookup table contains the value
bool SD_LutContainsValue(const SD_Lut *lut, const char *value) {
    assert(lut != NULL && value != NULL);

    size_t valc = SD_LutValueCount(lut);
    for (size_t idx = 0; idx < valc; idx++) {
        if (strcmp(lut->vals[idx], value) == 0) {
            return true;
        }
    }
    return false;
}

/// The key from the lookup table
const char *SD_LutKeyAt(const SD_Lut *lut, size_t idx) {
    assert(lut != NULL);

    return lut->keys[idx];
}

/// The value from the lookup table
const char *SD_LutValueAt(const SD_Lut *lut, size_t idx) {
    assert(lut != NULL);

    return lut->vals[idx];
}

/// The mapped key for the given value in the lookup table
const char *SD_LutMappingFor(const SD_Lut *lut, const char *value) {
    assert(lut != NULL && value != NULL);

    size_t value_count = SD_LutValueCount(lut);
    for (size_t idx = 0; idx < value_count; idx++) {
        const char *value_there = lut->vals[lut->mapping[idx][0]];
        if (strcmp(value_there, value) == 0) {
            return lut->keys[lut->mapping[idx][1]];
        }
    }

    return NULL;
}

/// Adds the given lookup table to the persistent lookup table
bool SD_LutAdd(SD_Lut *dst, const SD_Lut *src) {
    assert(dst != NULL && src != NULL);

    size_t dst_keyc = SD_LutKeyCount  (dst);
    size_t dst_valc = SD_LutValueCount(dst);
    size_t src_keyc = SD_LutKeyCount  (src);
    size_t src_valc = SD_LutValueCount(src);

    if ((src_keyc + dst_keyc) > SD_LUT_MAXKEYS || (src_valc + dst_valc) > SD_LUT_MAXVALUES) {
        SD_LogError("resulting LUT will be greater in size than allowed");
        return false;
    }

    size_t offset = 0;
    for (size_t idx = 0; idx < src_keyc; idx++) {
        SD_LogDebug("src key = '%s'", src->keys[idx]);

        if (SD_LutContainsKey(dst, src->keys[idx])) {
            offset++;
        } else {
            char *dst_key = dst->keys[dst_keyc + idx - offset];
            const char *src_key = src->keys[idx];
            memcpy(dst_key, src_key, SD_LUT_MAXSTRLEN); // all luts are zero initialised so is safe
        }
    }

    offset = 0;
    for (size_t idx = 0; idx < src_valc; idx++) {
        SD_LogDebug("src val = '%s'", src->vals[idx]);

        if (SD_LutContainsValue(dst, src->vals[idx])) {
            offset++;
        } else {
            char *dst_val = dst->vals[dst_valc + idx - offset];
            const char *src_val = src->vals[idx];
            memcpy(dst_val, src_val, SD_LUT_MAXSTRLEN); // all luts are zero initialised so is safe
        }
    }

    return true;
}

// LUT DSL:
// L = {python:[py,pyw,pyi],c:[c,h]}
//
// Grammar:
// <config>          ::= '{' <key_values> '}'
//
// <key_values>      ::= <key_value> <key_values_tail>
// <key_values_tail> ::= ε
//                    |  ',' <key_values>
//
// <key_value>       ::= <key> ':' <list>
//
// <list>            ::= '[' <values> ']'
//
// <values>          ::= <value> <values_tail>
// <values_tail>     ::= ε
//                    | ',' <values>
//
// <key>             ::= [VALID FILENAME]
// <value>           ::= [VALID FILE EXTENSION]

// parsers
static bool SD_LutParseDSL_config         (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_key_values     (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_key_values_tail(const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_key_value      (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_list           (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_values         (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_values_tail    (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_value          (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_key            (const char **dsl, SD_Lut *lut);
static bool SD_LutParseDSL_value          (const char **dsl, SD_Lut *lut);

/// Parse the DSL into a tangible SD_Lut object
bool SD_LutParseDSL(const char *dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    (*lut) = (SD_Lut) {0};
    return SD_LutParseDSL_config(&dsl, lut);
}

static void SD_IgnoreWhitespace(const char **src) {
    assert(src != NULL);

    while ((**src) == ' ') {
        (*src)++;
    }
}

static bool SD_ParseChar(const char **src, const char *name, char ch) {
    assert(src != NULL && name != NULL);

    SD_IgnoreWhitespace(src);
    if ((**src) != ch) {
        SD_LogError("expected %s - got %c instead", name, (**src));
        return false;
    }
    (*src)++;
    return true;
}

// <config> ::= '{' <key_values> '}'
static bool SD_LutParseDSL_config(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <config>");

    return SD_ParseChar(dsl, "opening brace", '{')
      &&   SD_LutParseDSL_key_values(dsl, lut)
      &&   SD_ParseChar(dsl, "closing brace", '}');
}

// <key_values> ::= <key_value> <key_values_tail>
static bool SD_LutParseDSL_key_values(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <key_values>");

    return SD_LutParseDSL_key_value      (dsl, lut)
      &&   SD_LutParseDSL_key_values_tail(dsl, lut);
}

// <key_values_tail> ::= ε
//                    |  ',' <key_values>
static bool SD_LutParseDSL_key_values_tail(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <key_values_tail>");

    if (**dsl != ',') return true;
    (*dsl)++;
    return SD_LutParseDSL_key_values(dsl, lut);
}

// <key_value> ::= <key> ':' <list>
static bool SD_LutParseDSL_key_value(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <key_values>");

    // TODO: key & value duplication detection

    // for mapping
    size_t keyc = SD_LutKeyCount(lut);

    // get count before parsing to know where to write mappings
    size_t valc0 = SD_LutValueCount(lut);
    bool ok = SD_LutParseDSL_key (dsl, lut)
       &&     SD_ParseChar(dsl, "colon", ':')
       &&     SD_LutParseDSL_list(dsl, lut);
    if (!ok) return false;

    // TODO: just noticed the redundancy of having the integers pairs when we only need keys
    // TODO: mappings are ordered in value order - hence value index is no longer required
    size_t valc1 = SD_LutValueCount(lut);
    for (size_t mapidx = valc0; mapidx < valc1; mapidx++) {
        lut->mapping[mapidx][0] = keyc;
        lut->mapping[mapidx][1] = mapidx;
    }

    return true;
}

// <list> ::= '[' <values> ']'
static bool SD_LutParseDSL_list(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <list>");

    return SD_ParseChar(dsl, "opening bracket", '[')
      &&   SD_LutParseDSL_values(dsl, lut)
      &&   SD_ParseChar(dsl, "opening bracket", ']');
}

// <values> ::= <value> <values_tail>
static bool SD_LutParseDSL_values(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <values>");

    return SD_LutParseDSL_value      (dsl, lut)
      &&   SD_LutParseDSL_values_tail(dsl, lut);
}

// <values_tail> ::= ε
//                 | ',' <values>
static bool SD_LutParseDSL_values_tail(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <values_tail>");

    if (**dsl != ',') return true;
    (*dsl)++;
    return SD_LutParseDSL_values(dsl, lut);
}

// # the lookup table representation in memory
// struct SD_Lut {
//     char    keys   [SD_LUT_MAXKEYS]  [SD_LUT_MAXSTRLEN+1]; # the keys
//     char    vals   [SD_LUT_MAXVALUES][SD_LUT_MAXSTRLEN+1]; # the values
//     uint8_t mapping[SD_LUT_MAXVALUES][2];                  # 1:1 associative array of values to keys
// };

// <key> ::= [VALID FILENAME]
static bool SD_LutParseDSL_key(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <key>");

    size_t keyc = SD_LutKeyCount(lut);

    size_t len = 0;
    while (!strchr("{}[]:,", (**dsl)) && len != SD_LUT_MAXSTRLEN) {
        lut->keys[keyc][len] = (**dsl);

        (*dsl)++;
        len++;
    }

    return true;
}

// <value> ::= [VALID FILE EXTENSION]
static bool SD_LutParseDSL_value(const char **dsl, SD_Lut *lut) {
    assert(dsl != NULL && lut != NULL);
    SD_LogDebug("PARSE RULE: <value>");

    size_t valc = SD_LutValueCount(lut);

    size_t len = 0;
    while (!strchr("{}[]:,", (**dsl)) && len != SD_LUT_MAXSTRLEN) {
        lut->vals[valc][len] = (**dsl);

        (*dsl)++;
        len++;
    }

    return true;
}
