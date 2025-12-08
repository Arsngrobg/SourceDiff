#define MAX_PATH (260)   // windows
#define MAX_CMD  (32767) // windows

int SD_CompileLanguage   (const char *language_id);            // compiles the language into a dynamic library
int SD_LoadCachedLanguage(const char *language_id);            // loads the cached language library
int SD_LoadLanguage      (const char *language_id);            // loads the library - either compiles or loads cache
int SD_ReadSource        (const char *file_path, char *buf[]); // reads the contents of the source file into the buffer

#include <stdio.h>    // for file operations
#include <stdlib.h>   // for malloc
#include <unistd.h>   // for cwd, mkdir
#include <stdarg.h>   // for variadic arguments

int SD_ExecuteCommand(const char *format, ...) {
    va_list args;
}

int SD_CompileLanguage(const char *language_id) {
    char cwd[MAX_PATH];
    getcwd(cwd, MAX_PATH);

    char dir[MAX_PATH];
    if (sprintf(dir, "%s\\languages\\cache", cwd) == 0) {
        fprintf(
            stderr,
            "SourceDiff: Unable to create cache directory for compiled language binaries (ID: %s)",
            language_id
        );
        return 0;
    }
    mkdir(dir);

    char cmd[MAX_CMD];
    if (sprintf(cmd,
        "CC -fPIC -shared languages\\%s\\parser.c -Ilanguages\\%s\\tree_sitter -o languages\\cache\\%s-lang.dll",
        language_id, language_id, language_id
    ) == -1) {
        fprintf(stderr, "SourceDiff: Unable to compile language\n");
        return 0;
    }

    printf("%s\n", cmd);

    // TODO: using 'system' is apparently insecure
    if (system(cmd) == -1) {
        fprintf(
            stderr,
            "SourceDiff: Permission denied, unable to complete dynamic compilation for ID:%s\n",
            language_id
        );
        return 0;
    }

    return 1;
}

int SD_ReadSource(const char *file_path, char *buf[]) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr,"SourceDiff: the file (%s) cannot was not found\n", file_path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    const size_t size = ftell(file);
    rewind(file);

    *buf = malloc(sizeof(char) * (size + 1));
    if (*buf == NULL) {
        fprintf(
            stderr,
            "SourceDiff: failed to allocate enough memory for the string buffer for file (%s)\n",
            file_path
        );
        return 0;
    }

    const size_t bytes = fread(*buf, sizeof(char), size, file);
    (*buf)[bytes] = '\0';
    fclose(file);
    return 1;
}

#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(lang, NULL, "--lang", "ID", "The language ID to use (must be valid ID in 'languages')")

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(source1, "source1", "The source file to compare with source2") \
    REQUIRED_STRING_ARG(source2, "source2", "The source file to compare with source1")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help,    "-h", "Shows the usage of SourceDiff") \
    BOOLEAN_ARG(version, "-v", "Prints the current version of SourceDiff") \

#include "easyargs.h"

int main(const int argc, char *argv[]) {
    args_t args = make_default_args();
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help("SourceDiff");
        return 0;
    }

    if (args.version) {
        printf("SourceDiff (ver 0.1)\n");
    } else if (args.source1 && args.source2) {
        char *source1_buf; char *source2_buf;
        SD_ReadSource(args.source1, &source1_buf);
        SD_ReadSource(args.source2, &source2_buf);
        return SD_CompileLanguage("C");
    }

    return 0;
}
