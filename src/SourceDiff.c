#define MAX_PATH (260)   // windows only
#define MAX_CMD  (32767) // windows only

#include <stdio.h>   // for file operations
#include <stdlib.h>  // for malloc
#include <unistd.h>  // for cwd, mkdir
#include <stdarg.h>  // for variadic arguments
#include <windows.h> // for .dll loading

#include "tree_sitter/api.h"

int SD_ExecuteCommand    (const char *format, ...);            // executes the command
int SD_CompileLanguage   (const char *language_id);            // compiles the language into a dynamic library
int SD_LoadCachedLanguage(const char *language_id);            // loads the cached language library
int SD_ReadSource        (const char *file_path, char *buf[]); // reads the contents of the source file into the buffer
int SD_AnalyseSource     (const char *s1, const char* s2);     // analyses the two source files
int SD_OutputTree        (TSNode root, int depth);             // prints out the tree

typedef TSLanguage* (*TSLanguageProducer)();
static TSLanguageProducer producer = NULL; // loaded in via SD_LoadLanguage

int SD_ExecuteCommand(const char *format, ...) {
    va_list args;
    char cwd[MAX_PATH]; // the current working directory
    char cmd[MAX_CMD];  // the final command created by this function

    if (getcwd(cwd, MAX_PATH) == NULL) {
        fprintf(stderr, "SourceDiff: Unable to get current working directory\n");
        return 0;
    }

    va_start(args, format);
    if (vsnprintf(cmd, MAX_CMD, format, args) == 0) {
        fprintf(stderr, "SourceDiff: Unable to format command\n");
        return 0;
    }

    if (system(cmd) == -1) {
        fprintf(stderr, "SourceDiff: Unable to execute command: %s\n", cmd);
        return 0;
    }

    return 1;
}

int SD_CompileLanguage(const char *language_id) {
    mkdir("languages\\bin");
    SD_ExecuteCommand(
        "gcc -fPIC -shared languages\\%s\\parser.c -Ilanguages\\%s\\tree_sitter -o languages\\bin\\%s-lang.dll",
        language_id, language_id, language_id
    );
    return 1;
}

int SD_ReadSource(const char *file_path, char *buf[]) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr,"SourceDiff: the file (%s) cannot was not found\n", file_path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    const size_t len = ftell(file);
    rewind(file);

    *buf = malloc(sizeof(char) * (len + 1));
    if (*buf == NULL) {
        fprintf(
            stderr,
            "SourceDiff: failed to allocate enough memory for the string buffer for file (%s)\n",
            file_path
        );
        return 0;
    }

    const size_t bytes = fread(*buf, sizeof(char), len, file);
    (*buf)[bytes] = '\0';
    fclose(file);
    return 1;
}

int SD_LoadCachedLanguage(const char *language_id) {
    char path[MAX_PATH];

    if (snprintf(path, MAX_PATH, "languages\\bin\\%s-lang.dll", language_id) == 0) {
        fprintf(stderr, "SourceDiff: Failed to format path to language cache\n");
        return 0;
    }

    const HMODULE proc = LoadLibraryA(path);
    if (LoadLibraryA(path) == NULL) {
        fprintf(stderr, "SourceDiff: Failed to load the language (%s) from cache\n", language_id);
        return 0;
    }

    producer = (TSLanguageProducer) GetProcAddress(proc, "tree_sitter_c");

    return 1;
}

int SD_AnalyseSource(const char *s1, const char *s2) {
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, producer());

    TSTree *left = ts_parser_parse_string(
        parser,
        NULL,
        s1,
        strlen(s1)
    );
    TSTree *right = ts_parser_parse_string(
        parser,
        NULL,
        s2,
        strlen(s2)
    );

    printf("== FILE1 ==\n");
    SD_OutputTree(ts_tree_root_node(left), 0);
    printf("\n\n== FILE2 ==\n");
    SD_OutputTree(ts_tree_root_node(right), 0);

    ts_tree_delete(left);
    ts_tree_delete(right);
    ts_parser_delete(parser);

    return 1;
}

int SD_OutputTree(const TSNode root, const int depth) {
    for (int _ = 0; _ < depth; _++) {
        printf("|   ");
    }

    printf("[%s]\n", ts_node_type(root));

    for (int childIdx = 0; childIdx < ts_node_child_count(root); childIdx++) {
        const TSNode child = ts_node_child(root, childIdx);
        SD_OutputTree(child, depth + 1);
    }

    return 1;
}

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(source1, "source1", "The source file to compare with source2") \
    REQUIRED_STRING_ARG(source2, "source2", "The source file to compare with source1")

#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(language, NULL, "--LANGUAGE", "ID", "The language ID to use (must be valid ID in 'languages')")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help,    "-h", "Shows the usage of SourceDiff") \
    BOOLEAN_ARG(version, "-v", "Prints the current version of SourceDiff") \

#include "easyargs.h"

int main(const int argc, char *argv[]) {
    args_t args = make_default_args();
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help("SourceDiff");
        return EXIT_SUCCESS;
    }

    if (args.version) {
        printf("SourceDiff (ver 0.1)\n");
    } else if (args.source1 && args.source2) {
        if (args.language) {
            SD_CompileLanguage(args.language);
            SD_LoadCachedLanguage(args.language);
        }

        char *source1_buf; char *source2_buf;
        SD_ReadSource(args.source1, &source1_buf);
        SD_ReadSource(args.source2, &source2_buf);

        SD_AnalyseSource(source1_buf, source2_buf);

        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
