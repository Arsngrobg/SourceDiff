#ifndef SRCDIFF_LOGGING_H
#define SRCDIFF_LOGGING_H

#include <stdio.h>

#include "argv.h"

#define ANSI_PREFIX "\x1b["
#define ANSI_SUFFIX "m"
#define ANSI_RESET  ANSI_PREFIX "0"    ANSI_SUFFIX
#define ANSI_ERROR  ANSI_PREFIX "1;31" ANSI_SUFFIX
#define ANSI_INFO   ANSI_PREFIX "1;32" ANSI_SUFFIX
#define ANSI_WARN   ANSI_PREFIX "1;33" ANSI_SUFFIX
#define ANSI_DEBUG  ANSI_PREFIX "1;36" ANSI_SUFFIX

#define SD_Log(fmt, ...)      fprintf(stdout, "%s: "                                         fmt "\n", SD_GetExecName(),           ##__VA_ARGS__)
#define SD_LogInfo(fmt, ...)  fprintf(stdout, "%s: " ANSI_INFO  "info: "      ANSI_RESET " " fmt "\n", SD_GetExecName(),           ##__VA_ARGS__)
#define SD_LogError(fmt, ...) fprintf(stderr, "%s: " ANSI_ERROR "error: "     ANSI_RESET " " fmt "\n", SD_GetExecName(),           ##__VA_ARGS__)
#define SD_LogDebug(fmt, ...) fprintf(stdout, "%s: " ANSI_DEBUG "debug (%s):" ANSI_RESET " " fmt "\n", SD_GetExecName(), __func__, ##__VA_ARGS__)

#endif // SRCDIFF_LOGGING_H
