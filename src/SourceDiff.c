#define MAX_PATH (260)         // windows
#define MAX_CMD  (32767)       // windows
#define LANG_DIR ".\\languages\\" // windows
#define CMD_FORM ( \
    "(cd %s) & " \
    "(mkdir .\\languages\\cache) & " \
    "(CC -fPIC -shared .\\languages\\parser.c -I.\\languages\\%s\\tree_sitter -o .\\languages\\%s\\cache\\%s-lang.dll)" \
)

int SD_CompileLanguage   (const char *language_id);            // compiles the language into a dynamic library
int SD_LoadLanguage      (const char *language_id);            // loads the library - either compiles or loads cache
int SD_LoadCachedLanguage(const char *language_id);            // loads the cached language library
int SD_ReadSource        (const char *file_path, char *buf[]); // reads the contents of the source file into the buffer

#include <stdio.h>  // for file operations
#include <stdlib.h> // for malloc
#include <unistd.h> // for cwd

int SD_CompileLanguage(const char *language_id) {
    char cwd[MAX_PATH];
    getcwd(cwd, MAX_PATH);

    char cmd[MAX_CMD];
    if (sprintf(cmd, "(cd %s) & (mkdir languages/cache) & (CC -fPIC -shared %s%s\\parser.c -I%s%s\\tree_sitter -o %scache\\%s-lang.dll)", cwd, LANG_DIR, language_id, LANG_DIR, language_id, LANG_DIR, language_id) == 0) {
        printf("SourceDiff: Unable to compile language\n");
        return 0;
    }

    printf("%s\n", cmd);

    // TODO: using 'system' is apparently insecure
    if (system(cmd) == 0) {
        printf("SourceDiff: Failed to execute compilation request.");
        return 0;
    }

    return 1;
}

int SD_ReadSource(const char *file_path, char *buf[]) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("SourceDiff: the file (%s) cannot was not found\n", file_path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    const size_t size = ftell(file);
    rewind(file);

    *buf = malloc(sizeof(char) * (size + 1));
    if (*buf == NULL) {
        printf("SourceDiff: failed to allocate enough memory for the string buffer for file (%s)\n", file_path);
        return 0;
    }

    const size_t bytes = fread(*buf, sizeof(char), size, file);
    (*buf)[bytes] = '\0';
    fclose(file);
    return 1;
}

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(source1, "source1", "The source file to compare with source2") \
    REQUIRED_STRING_ARG(source2, "source2", "The source file to compare with source1")

#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(lang, "file ext", "--lang", "dir", "The language to use (must be present in the 'languages' directory)")

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
        printf("SourceDiff (ver 1.0)\n");
    } else if (args.source1 && args.source2) {
        char *source1_buf; char *source2_buf;
        SD_ReadSource(args.source1, &source1_buf);
        SD_ReadSource(args.source2, &source2_buf);
        printf("%s\n", source1_buf);
        printf("%s\n", source2_buf);
        printf("%s, %s\n", args.source1, args.source2);
        return SD_CompileLanguage("C");
    }

    return 0;
}
