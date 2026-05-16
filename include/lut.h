#ifndef SRCDIFF_LUT_H
#define SRCDIFF_LUT_H

#include <stdbool.h>
#include <stddef.h>

/// A lookup table for matching file extensions to their language parsers
typedef struct SD_Lut SD_Lut;

/// Overwrites the persistent LUT with this one
bool SD_WriteLut(const SD_Lut *lut);

/// Gets the persistent lookup table (empty if none exists)
SD_Lut *SD_GetLut(void);

/// Allocates an empty lookup table
SD_Lut *SD_EmptyLut(void);

/// The number of keys in the lookup table
size_t SD_LutKeyCount(const SD_Lut *lut);

/// The number of values in the lookup table
size_t SD_LutValueCount(const SD_Lut *lut);

/// Whether the lookup table contains the key
bool SD_LutContainsKey(const SD_Lut *lut, const char *key);

/// Whether the lookup table contains the value
bool SD_LutContainsValue(const SD_Lut *lut, const char *value);

/// The key from the lookup table
const char *SD_LutKeyAt(const SD_Lut *lut, size_t idx);

/// The value from the lookup table
const char *SD_LutValueAt(const SD_Lut *lut, size_t idx);

/// The mapped key for the given value in the lookup table
const char *SD_LutMappingFor(const SD_Lut *lut, const char *value);

/// Adds the given lookup table to the persistent lookup table
bool SD_LutAdd(SD_Lut *dst, const SD_Lut *src);

/// Parse the DSL into a tangible SD_Lut object
bool SD_LutParseDSL(const char *dsl, SD_Lut *lut);

#endif // SRCDIFF_LUT_H
