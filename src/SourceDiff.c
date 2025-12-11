#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>

#include "tree_sitter/api.h"

// macro constants
#define SD_NAME          "SourceDiff"        // the string literal of the project name
#define SD_VERSION_MAJOR 0                   // the major version of the project
#define SD_VERSION_MINOR 1                   // the minor version of the project
#define SD_LANGUAGE_DIR  ".\\languages"      // the directory used by the language loader

#ifdef __WIN32
#define SD_ENV_DELIMITER ";"                 // the delimiter for the PATH variable on window's systems
#define SD_MAX_ARG       32767               // the maximum length of a cmd line argument on a window's system
#define SD_MAX_PATH      260                 // the maximum length of a path on window's
#else // __WIN32
#define SD_ENV_DELIMITER ":"                 // the delimiter for the PATH variable on unix systems
#endif // __WIN32

#ifdef __WIN32
#define SD_LIB_EXT ".dll"
#define SD_LIB_ARG "-shared"
#elif defined(__linux__)
#define SD_LIB_EXT ".so"
#define SD_LIB_ARG "-fPIC -shared"
#else
#define SD_LIB_EXT
#define SD_LIB_ARG "-dynamiclib"
#endif


// unsigned integer types
typedef uint64_t u64;                        // shorthand name for an unsigned, 64-bit width integer
typedef uint32_t u32;                        // shorthand name for an unsigned, 32-bit width integer
typedef uint16_t u16;                        // shorthand name for an unsigned, 16-bit width integer
typedef uint8_t  u8;                         // shorthand name for an unsigned, 8-bit width integer

// signed integer types
typedef  int64_t i64;                        // shorthand name for an unsigned, 64-bit width integer
typedef  int32_t i32;                        // shorthand name for an unsigned, 32-bit width integer
typedef  int16_t i16;                        // shorthand name for an unsigned, 16-bit width integer
typedef  int8_t  i8;                         // shorthand name for an unsigned, 8-bit width integer

// floating-point types
typedef float  f32;                          // shorthand name for a 32-bit width floating-point number
typedef double f64;                          // shorthand name for a 64-bit width floating-point number

// addressable type
typedef void* addr_t;                        // type alias for a generic pointer type

// custom boolean type to maintain maximum portability across compilers (<stdbool.h> is a little weird)
typedef enum {
    SD_TRUE  = 1,                            // a 'truthy' value
    SD_FALSE = 0                             // a NOT 'truthy' value
} SD_Bool;

// macro to convert the literal value into a SD_Bool
#define SD_BOOL(x) ((x) ? SD_TRUE : SD_FALSE)

typedef TSLanguage* (*LanguageProducer)();

// struct for packing data related to a loaded language
typedef struct {
    char       *id;                          // the ID of the language
    TSLanguage *ts_lang;                     // the tree-sitter language backend
} SD_LanguageData;

typedef enum {
    SD_TREE_DELETE  = 3,
    SD_TREE_INSERT  = 3,
    SD_TREE_RELABEL = 2
} SD_TreeOperation;

// function prototypes
SD_Bool SD_ExecuteCommand (const char *format, ...);       // executes the supplied command
SD_Bool SD_GetCompiler    (char **cc);                     // probes a set of compilers to use for language compilation
SD_Bool SD_CompileLanguage(const char *id);                // compiles the language of the specified ID
SD_Bool SD_LoadLanguage   (const char *id);                // loads the language of the specified ID
SD_Bool SD_LoadFile       (const char *path, char *buf[]); // loads in the contents of the file into the buffer
SD_Bool SD_LoadGlob       (const char *pattern);           // loads the files using the specified glob pattern
SD_Bool SD_GetDiff        (const char *a, const char *b);  // analyses the two strings
u32     SD_GetTreeDiff    (TSNode st_a, TSNode st_b);      // gets the edit distance for the two subtrees

static SD_LanguageData data = {0};

SD_Bool SD_ExecuteCommand(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char cmd[SD_MAX_ARG];
    if (vsnprintf(cmd, SD_MAX_ARG, format, args) < 0) {
        fprintf(stderr, "SourceDiff: Failed to format command");
        return SD_FALSE;
    }

#ifdef __WIN32
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(!CreateProcess(NULL,
        cmd,
        NULL, NULL, FALSE, 0, NULL,
        NULL,
        &si, &pi
    )) {
        fprintf(stderr, "SourceDiff: Unable to create process (%s)\n", cmd);
        return SD_FALSE;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
        fprintf(stderr, "SourceDiff: Unable to obtain exit code of process (%s)\n", cmd);
        return SD_FALSE;
    }

    if (exit_code != 0) {
        fprintf(stderr, "SourceDiff: exit code for proc (%s) is %ld\n", cmd, exit_code);
        return SD_FALSE;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#endif

    return SD_TRUE;
}

SD_Bool SD_GetCompiler(char **cc) {
    if ((*cc = getenv("PATH")) == NULL) {
        fprintf(stderr, "SourceDiff: Unable to obtain PATH");
        return SD_FALSE;
    }

    struct dirent *entry;

    const char *path_var = strtok(*cc, SD_ENV_DELIMITER);
    while (path_var != NULL) {
        DIR* dir;
        if ((dir = opendir(path_var)) == NULL) {
            fprintf(stderr, "SourceDiff: Unable to open directory (%s)\n", path_var);
            return SD_FALSE;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') continue;
            if (strcmp(entry->d_name, "gcc.exe"  ) == 0 || strcmp(entry->d_name, "gcc"  ) == 0 ||
                strcmp(entry->d_name, "clang.exe") == 0 || strcmp(entry->d_name, "clang") == 0 ||
                strcmp(entry->d_name, "cl.exe"   ) == 0 || strcmp(entry->d_name, "cl   ") == 0
            ) {
                *cc = entry->d_name;
                closedir(dir);
                return SD_TRUE;
            }
        }

        closedir(dir);
        path_var = strtok(NULL, SD_ENV_DELIMITER);
    }

    *cc = NULL;
    return SD_FALSE;
}

SD_Bool SD_CompileLanguage(const char *id) {
    if (id == NULL) return SD_FALSE;

    char *cc;
    if (!SD_GetCompiler(&cc)) {
        fprintf(stderr, "SourceDiff: Unable to get the system compiler\n");
        return SD_FALSE;
    }

    mkdir(SD_LANGUAGE_DIR"\\bin");
    if (!SD_ExecuteCommand(
        "%s "SD_LIB_ARG" "
        SD_LANGUAGE_DIR"\\%s\\parser.c -I"SD_LANGUAGE_DIR"\\%s\\tree_sitter "
        "-o "SD_LANGUAGE_DIR"\\bin\\%s-lang"SD_LIB_EXT,
        cc, id, id, id
    )) {
        return SD_FALSE;
    }

    return SD_TRUE;
}

SD_Bool SD_LoadLanguage(const char *id) {
    if (id == NULL) return SD_FALSE;

    char path[SD_MAX_PATH];
    if (snprintf(path, SD_MAX_PATH, SD_LANGUAGE_DIR"\\bin\\%s-lang.dll", id) < 0) {
        fprintf(stderr, "SourceDiff: Unable to format directory for language directory\n");
        return SD_FALSE;
    }

    char fn[SD_MAX_PATH];
    if (snprintf(fn, SD_MAX_PATH, "tree_sitter_%s", id) < 0) {
        fprintf(stderr, "SourceDiff: Unable to format function identifier for language producer\n");
        return SD_FALSE;
    }

    const HMODULE module = LoadLibraryA(path);
    if (module == NULL) {
        fprintf(stderr, "SourceDiff: No language binary found for ID (\"%s\")\n", id);
        return SD_FALSE;
    }

    const LanguageProducer p = (LanguageProducer) GetProcAddress(module, fn);
    data.id      = (char *) id;
    data.ts_lang = p();

    return SD_TRUE;
}

SD_Bool SD_LoadFile(const char *path, char **buf) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr,"SourceDiff: the file (%s) cannot was not found\n", path);
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
            path
        );
        return 0;
    }

    const size_t bytes = fread(*buf, sizeof(char), len, file);
    (*buf)[bytes] = '\0';
    fclose(file);
    return 1;
}

SD_Bool SD_GetDiff(const char *a, const char *b) {
    if (a == NULL || b == NULL) return SD_FALSE;

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, data.ts_lang);

    TSTree *tree_a = ts_parser_parse_string(parser, NULL, a, strlen(a));
    TSTree *tree_b = ts_parser_parse_string(parser, NULL, b, strlen(b));

    const TSNode root_a = ts_tree_root_node(tree_a);
    const TSNode root_b = ts_tree_root_node(tree_b);

    const u32 diff = SD_GetTreeDiff(root_a, root_b);
    printf("Diff: %d\n", diff);

    ts_tree_delete(tree_a);
    ts_tree_delete(tree_b);
    ts_parser_delete(parser);
    return SD_TRUE;
}

u32 SD_GetTreeDiff(const TSNode st_a, const TSNode st_b) {
    const u32 nc_a = ts_node_child_count(st_a);
    const u32 nc_b = ts_node_child_count(st_b);
    u32 accum = 0;

    const u32 min_children = min(nc_a, nc_b);
    for (u32 idx = 0; idx < min_children; idx++) {
        const TSNode n_a = ts_node_child(st_a, idx);
        const TSNode n_b = ts_node_child(st_b, idx);
        if (strcmp(ts_node_type(n_a), ts_node_type(n_b)) != 0) {
            accum += SD_TREE_RELABEL;
        }
        accum += SD_GetTreeDiff(n_a, n_b);
    }

    if (nc_a > nc_b) {
        for (u32 idx = nc_b; idx < nc_a; idx++) {
            const TSNode n_a = ts_node_child(st_a, idx);
            accum += ts_node_descendant_count(n_a) * SD_TREE_DELETE;
        }
    } else {
        for (u32 idx = nc_b; idx < nc_b; idx++) {
            const TSNode n_b = ts_node_child(st_b, idx);
            accum += ts_node_descendant_count(n_b) * SD_TREE_INSERT;
        }
    }

    return accum;
}

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(glob_a, "GLOB_A", "the glob pattern to match against all files in a single directory") \
    REQUIRED_STRING_ARG(glob_b, "GLOB_B", "the glob pattern to match against all files in a single directory")

#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(language, NULL, "--LANGUAGE", "ID", "The language parser to use")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help,       "-h",                 "displays the usage of "SD_NAME) \
    BOOLEAN_ARG(version,    "-v",                 "displays the version of "SD_NAME) \
    BOOLEAN_ARG(invalidate, "--invalidate-cache", "invalidates the bin directory")

#include "easyargs.h"

i32 main(const i32 argc, char *argv[]) {
    args_t args = make_default_args();
    const SD_Bool success = parse_args(argc, argv, &args);

    if (args.language) {
        if (args.invalidate && !SD_CompileLanguage(args.language)) {
            fprintf(stderr, "SourceDiff: Failed to compile language parser (ID: %s)\n", args.language);
            return EXIT_FAILURE;
        }
        if (!SD_LoadLanguage(args.language) && !SD_CompileLanguage(args.language) && !SD_LoadLanguage(args.language)) {
            fprintf(stderr, "SourceDiff: Failed to load language parser (ID: %s)\n", args.language);
            return EXIT_FAILURE;
        }
    }

    if (args.glob_a && args.glob_b) {
        char *buf_a; char *buf_b;
        if (!SD_LoadFile(args.glob_a, &buf_a) || !SD_LoadFile(args.glob_b, &buf_b)) {
            return EXIT_FAILURE;
        }

        return !SD_GetDiff(buf_a, buf_b);
    }

    if (args.version) {
        printf("%s (ver %d.%d)\n", SD_NAME, SD_VERSION_MAJOR, SD_VERSION_MINOR);
        return EXIT_SUCCESS;
    }

    if (!success || args.help) {
        print_help(SD_NAME);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
