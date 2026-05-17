#include "srcdiff_stdutils.h"
#include "srcdiff.h"

#define SRCDIFF_LUTSTORE     "LUT" // the file that the persistent lookup table is stored
#define SRCDIFF_MAXLUTSTRLEN (32)  // the maximum length of a string in a lookup table
#define SRCDIFF_MAXLUTKEYS   (256) // the maximum number of keys in a lookup table
#define SRCDIFF_MAXLUTVALUES (256) // the maximum number of values in a lookup table

// the lookup table representation in memory
struct SDLut {
    char    keys   [SRCDIFF_MAXLUTKEYS]  [SRCDIFF_MAXLUTSTRLEN+1]; // the keys
    char    vals   [SRCDIFF_MAXLUTVALUES][SRCDIFF_MAXLUTSTRLEN+1]; // the values
    uint8_t mapping[SRCDIFF_MAXLUTVALUES][2];                      // 1:1 associative array of values to keys
};

/// Overwrites the persistent LUT with this one
SDPUBLIC
bool sd_write_lut(const SDLut *lut) {
    assert(lut != NULL);

    sd_enter_bin_dir();
    FILE *lut_file = fopen(SRCDIFF_LUTSTORE, "w+b");
    if (lut_file == NULL) {
        sd_log_error("unable to obtain a handle to the LUT file");
        return false;
    }

    if (fwrite(lut, sizeof(SDLut), 1, lut_file) != 1) {
        sd_log_error("unable to write to the LUT file");
        return false;
    }
    fclose(lut_file);
    sd_exit_bin_dir();
    return true;
}

/// Gets the persistent lookup table (empty if none exists)
SDPUBLIC
SDLut *sd_get_lut(void) {
    SDPRIVATE SDLut lut    = {0};
    SDPRIVATE bool   loaded = false;

    sd_enter_bin_dir();
    if (!loaded) {
        FILE *lut_file = fopen(SRCDIFF_LUTSTORE, "rb");
        if (lut_file != NULL) {
            if (fread(&lut, 1, sizeof(SDLut), lut_file) != sizeof(SDLut)) {
                sd_log_warn("Unable to properly read the LUT file");
            }
            fclose(lut_file);
        }
        loaded = true;
    }
    sd_exit_bin_dir();

    return &lut;
}

/// Allocates an empty lookup table
SDPUBLIC
SDLut *sd_empty_lut(void) {
    SDLut *lut = calloc(1, sizeof(SDLut)); // 0 init for parity
    if (lut == NULL) {
        sd_log_error("out of system memory");
    }

    return lut;
}

/// The number of keys in the lookup table
SDPUBLIC
size_t sd_lut_key_count(const SDLut *lut) {
    assert(lut != NULL);

    size_t count = 0;
    for (size_t idx = 0; idx < SRCDIFF_MAXLUTKEYS && lut->keys[idx][0] != '\0'; idx++) {
        count++;
    }
    return count;
}

/// The number of values in the lookup table
SDPUBLIC
size_t sd_lut_value_count(const SDLut *lut) {
    assert(lut != NULL);

    size_t count = 0;
    for (size_t idx = 0; idx < SRCDIFF_MAXLUTVALUES && lut->vals[idx][0] != '\0'; idx++) {
        count++;
    }
    return count;
}

/// Whether the lookup table contains the key
SDPUBLIC
bool sd_lut_has_key(const SDLut *lut, const char *key) {
    assert(lut != NULL && key != NULL);

    size_t keyc = sd_lut_key_count(lut);
    for (size_t idx = 0; idx < keyc; idx++) {
        if (strcmp(lut->keys[idx], key) == 0) {
            return true;
        }
    }
    return false;
}

/// Whether the lookup table contains the value
SDPUBLIC
bool sd_lut_has_value(const SDLut *lut, const char *value) {
    assert(lut != NULL && value != NULL);

    size_t valc = sd_lut_value_count(lut);
    for (size_t idx = 0; idx < valc; idx++) {
        if (strcmp(lut->vals[idx], value) == 0) {
            return true;
        }
    }
    return false;
}

/// The index of the key in the LUT - is the number of keys if it does not exist
SDPUBLIC
size_t sd_lut_key_idx(const SDLut *lut, const char *key) {
    assert(lut != NULL && key != NULL);

    size_t idx = 0;
    while (strcmp(sd_lut_key_at(lut, idx), key) != 0) {
        idx++;
    }
    return idx;
}

/// The index of the value in the LUT - is the number of values if it does not exist
SDPUBLIC
size_t sd_lut_value_idx(const SDLut *lut, const char *value) {
    assert(lut != NULL && key != NULL);

    size_t idx = 0;
    while (strcmp(sd_lut_value_at(lut, idx), value) != 0) {
        idx++;
    }
    return idx;
}

/// The key from the lookup table
SDPUBLIC
const char *sd_lut_key_at(const SDLut *lut, size_t idx) {
    assert(lut != NULL && idx >= sd_lut_key_count(lut));

    return lut->keys[idx];
}

/// The value from the lookup table
SDPUBLIC
const char *sd_lut_value_at(const SDLut *lut, size_t idx) {
    assert(lut != NULL && idx >= sd_lut_value_count(lut));

    return lut->vals[idx];
}

/// The mapped key for the given value in the lookup table
SDPUBLIC
const char *sd_lut_mapping_for(const SDLut *lut, const char *value) {
    assert(lut != NULL && value != NULL);

    size_t value_count = sd_lut_value_count(lut);
    for (size_t idx = 0; idx < value_count; idx++) {
        const char *value_there = lut->vals[lut->mapping[idx][0]];
        if (strcmp(value_there, value) == 0) {
            return lut->keys[lut->mapping[idx][1]];
        }
    }

    return NULL;
}

/// Adds the given lookup table to the persistent lookup table
SDPUBLIC
bool sd_lut_add(SDLut *dst, const SDLut *src) {
    assert(dst != NULL && src != NULL);

    size_t dst_keyc = sd_lut_key_count  (dst);
    size_t dst_valc = sd_lut_value_count(dst);
    size_t src_keyc = sd_lut_key_count  (src);
    size_t src_valc = sd_lut_value_count(src);

    if ((src_keyc + dst_keyc) > SRCDIFF_MAXLUTKEYS || (src_valc + dst_valc) > SRCDIFF_MAXLUTVALUES) {
        sd_log_error("resulting LUT will be greater in size than allowed");
        return false;
    }

    size_t offset = 0;
    for (size_t idx = 0; idx < src_keyc; idx++) {
        sd_log_debug("src key = '%s'", src->keys[idx]);

        if (sd_lut_has_key(dst, src->keys[idx])) {
            offset++;
        } else {
            char *dst_key = dst->keys[dst_keyc + idx - offset];
            const char *src_key = src->keys[idx];
            memcpy(dst_key, src_key, SRCDIFF_MAXLUTSTRLEN); // all luts are zero initialised so is safe
        }
    }

    offset = 0;
    for (size_t idx = 0; idx < src_valc; idx++) {
        sd_log_debug("src val = '%s'", src->vals[idx]);

        if (sd_lut_has_value(dst, src->vals[idx])) {
            offset++;
        } else {
            char *dst_val = dst->vals[dst_valc + idx - offset];
            const char *src_val = src->vals[idx];
            memcpy(dst_val, src_val, SRCDIFF_MAXLUTSTRLEN); // all luts are zero initialised so is safe
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
// <key_values>      ::= ε
//                    |  <key_value> <key_values_tail>
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

// config parsers
SDPRIVATE bool sd_lut_parse_config         (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_key_values     (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_key_values_tail(const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_key_value      (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_list           (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_values         (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_values_tail    (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_value          (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_key            (const char **dsl, SDLut *lut);
SDPRIVATE bool sd_lut_parse_value          (const char **dsl, SDLut *lut);

SDPRIVATE
void sd_ignore_whitespace(const char **src) {
    assert(src != NULL);

    while ((**src) == ' ') {
        (*src)++;
    }
}

SDPRIVATE
bool sd_parse_char(const char **src, const char *name, char ch) {
    assert(src != NULL && name != NULL);

    sd_ignore_whitespace(src);
    if ((**src) != ch) {
        sd_log_error("expected %s - got %c instead", name, (**src));
        return false;
    }
    (*src)++;
    return true;
}

/// Parse the DSL into a tangible SDLut object
SDPUBLIC
bool sd_lut_parse(const char *dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    (*lut) = (SDLut) {0};
    return sd_lut_parse_config(&dsl, lut);
}

// <config> ::= '{' <key_values> '}'
SDPRIVATE
bool sd_lut_parse_config(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <config>");

    return sd_parse_char(dsl, "opening brace", '{')
      &&   sd_lut_parse_key_values(dsl, lut)
      &&   sd_parse_char(dsl, "closing brace", '}');
}

// <key_values> ::= ε
//               |  <key_value> <key_values_tail>
SDPRIVATE
bool sd_lut_parse_key_values(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <key_values>");

    if ((*(*dsl)+1) == '}') return true;

    return sd_lut_parse_key_value      (dsl, lut)
      &&   sd_lut_parse_key_values_tail(dsl, lut);
}

// <key_values_tail> ::= ε
//                    |  ',' <key_values>
SDPRIVATE
bool sd_lut_parse_key_values_tail(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <key_values_tail>");

    if (**dsl != ',') return true;
    (*dsl)++;
    return sd_lut_parse_key_values(dsl, lut);
}

// <key_value> ::= <key> ':' <list>
SDPRIVATE
bool sd_lut_parse_key_value(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <key_values>");

    // TODO: key & value duplication detection

    // for mapping
    size_t keyc = sd_lut_key_count(lut);

    // get count before parsing to know where to write mappings
    size_t valc0 = sd_lut_value_count(lut);
    bool ok = sd_lut_parse_key (dsl, lut)
       &&     sd_parse_char(dsl, "colon", ':')
       &&     sd_lut_parse_list(dsl, lut);
    if (!ok) return false;

    // TODO: just noticed the redundancy of having the integers pairs when we only need keys
    // TODO: mappings are ordered in value order - hence value index is no longer required
    size_t valc1 = sd_lut_value_count(lut);
    for (size_t mapidx = valc0; mapidx < valc1; mapidx++) {
        lut->mapping[mapidx][0] = keyc;
        lut->mapping[mapidx][1] = mapidx;
    }

    return true;
}

// <list> ::= '[' <values> ']'
SDPRIVATE
bool sd_lut_parse_list(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <list>");

    return sd_parse_char(dsl, "opening bracket", '[')
      &&   sd_lut_parse_values(dsl, lut)
      &&   sd_parse_char(dsl, "opening bracket", ']');
}

// <values> ::= <value> <values_tail>
SDPRIVATE
bool sd_lut_parse_values(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <values>");

    return sd_lut_parse_value      (dsl, lut)
      &&   sd_lut_parse_values_tail(dsl, lut);
}

// <values_tail> ::= ε
//                 | ',' <values>
SDPRIVATE
bool sd_lut_parse_values_tail(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <values_tail>");

    if (**dsl != ',') return true;
    (*dsl)++;
    return sd_lut_parse_values(dsl, lut);
}

// <key> ::= [VALID FILENAME]
SDPRIVATE
bool sd_lut_parse_key(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <key>");

    size_t keyc = sd_lut_key_count(lut);

    size_t len = 0;
    while (!strchr("{}[]:,", (**dsl)) && len != SRCDIFF_MAXLUTSTRLEN) {
        lut->keys[keyc][len] = (**dsl);

        (*dsl)++;
        len++;
    }

    return true;
}

// <value> ::= [VALID FILE EXTENSION]
SDPRIVATE
bool sd_lut_parse_value(const char **dsl, SDLut *lut) {
    assert(dsl != NULL && lut != NULL);
    sd_log_debug("PARSE RULE: <value>");

    size_t valc = sd_lut_value_count(lut);

    size_t len = 0;
    while (!strchr("{}[]:,", (**dsl)) && len != SRCDIFF_MAXLUTSTRLEN) {
        lut->vals[valc][len] = (**dsl);

        (*dsl)++;
        len++;
    }

    return true;
}
