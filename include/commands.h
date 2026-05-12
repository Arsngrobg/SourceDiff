#ifndef SRCDIFF_COMMANDS_H
#define SRCDIFF_COMMANDS_H

#include <stdint.h>

/// Computes the difference between two file, where: F1 - F2
int32_t SD_Exec_Diff(void);

/// Executes structural analysis of the files defined by the glob pattern
int32_t SD_Exec_Analyse(void);

/// Executes stylistic analysis of the files defined by the glob pattern
int32_t SD_Exec_Lint(void);

/// Registers a Tree Sitter grammar to SourceDiff
int32_t SD_Exec_Register(void);

/// Validates the persistent lookup table to make sure file integrity is ok
int32_t SD_Exec_LUTValidate(void);

/// Destroys the persistent lookup table to default configuration (equivalent to 'lut set {}')
int32_t SD_Exec_LUTInvalidate(void);

/// Sets the persistent lookup table using the high-level DSL
int32_t SD_Exec_LUTSet(void);

/// Adds the high-level DSL to the persistent lookup table
int32_t SD_Exec_LUTGet(void);

#endif // SRCDIFF_COMMANDS_H
