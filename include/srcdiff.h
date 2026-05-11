//      ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
//    ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
//    ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
//     ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
//         ▀██   ██       ██        ██    ██     ██       ██        ██
//    █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
//    ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
//
//   Global API functions for the project

#ifndef SRCDIFF_H
#define SRCDIFF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "srcdiff_types.h"

// Logging
void         SD_Log          (const char *fmt, ...);              // Logs the specified message
void         SD_LogWarn      (const char *fmt, ...);              // Logs the specified message as a warning
void         SD_LogError     (const char *fmt, ...);              // Logs the specified message as an error
void         SD_LogDebug     (const char *fmt, ...);              // Logs the specified message only in debug mode

// Configuration
int32_t      SD_ParseCLArgs  (int32_t argc, const char *argv[]);  // Parses the CL args
const char  *SD_GetExecName  (void);                              // Gets the name of the executable that hosts SourceDiff
const char **SD_GetModeArgs  (void);                              // Gets the modal CL arguments
size_t       SD_GetArgCount  (void);                              // Gets the number of modal CL arguments
FILE        *SD_GetOutputFile(void);                              // Gets the output file (NULL if not set)
SD_Mode      SD_GetMode      (void);                              // Gets the configured mode
bool         SD_IsOptionSet  (SD_Option option);                  // Is this option enabled?
#ifndef NDEBUG
bool         SD_CLArgsParsed (void);                              // debug assertion
#endif // NDEBUG

// Dispatching
int32_t      SD_Exec         (void);                              // Executes on the SourceDiff configuration (dispatcher)
int32_t      SD_ExecDiff     (void);                              // Executes diff mode
int32_t      SD_ExecLint     (void);                              // Executes lint mode
int32_t      SD_ExecAnalyse  (void);                              // Executes analyse mode
int32_t      SD_ExecRegister (void);                              // Executes register mode

// LUT Queries
size_t       SD_LangCount    (void);                              // The number of languages with extensions mapped
size_t       SD_TotalExtCount(void);                              // The total number of extensions mapped
size_t       SD_ExtCount     (const char *lang_name);             // The number of extensions mapped for to the `lang_name`
const char  *SD_GetMappedExt (const char *lang_name, size_t idx); // The mapped extension to the `lang_name` at `idx`
bool         SD_ExtMapsToLang(const char *ext);                   // Does this extension map to a language?

// LUT Configuration
int32_t      SD_LUTValidate  (void);                              // Validates the LUT
int32_t      SD_LUTInvalidate(void);                              // Invalidates the LUT
int32_t      SD_LUTSet       (void);                              // Overwrites the LUT using a configuration DSL
int32_t      SD_LUTAdd       (void);                              // Amends the LUT using a configuration DSL

#endif // SRCDIFF_H
