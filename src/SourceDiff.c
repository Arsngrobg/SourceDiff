#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>

#include "tree_sitter/api.h"
#include "libtcc.h"

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
#define SD_BOOL(x)             ((x) ? SD_TRUE : SD_FALSE)

typedef TSLanguage* (*LanguageProducer)();

// struct for packing data related to a loaded language
typedef struct {
    char       *id;                          // the ID of the language
    TSLanguage *ts_lang;                     // the tree-sitter language backend
} SD_LanguageData;

// the operation types to execute on the tree
typedef enum {
    SD_TREE_DELETE,
    SD_TREE_INSERT,
    SD_TREE_RELABEL
} SD_TreeOperation;

#define SD_TREE_DELETE_COST  3
#define SD_TREE_INSERT_COST  3
#define SD_TREE_RELABEL_COST 1
#define SD_DIFF_MAX_OPS      500

// struct to pack the diff data in, it primarily records the operations that must be applied to sa to reach sb
typedef struct {
    u64         ops;               // the number of operations
    const char *sa;                // source code A
    const char *sb;                // source code B
    struct {
        SD_TreeOperation op;       // the operation to perform
        TSPoint start;             // the position row/col
        u32 bytes[4];              // the character start and end index
    } op_seq[SD_DIFF_MAX_OPS];
} SD_Diff;

// function prototypes
SD_Bool SD_ExecuteCommand (const char *format, ...);           // executes the supplied command
SD_Bool SD_GetCompiler    (char **cc);                         // probes a set of compilers to use for language comp
SD_Bool SD_CompileLanguage(const char *id);                    // compiles the language of the specified ID
SD_Bool SD_LoadLanguage   (const char *id);                    // loads the language of the specified ID
SD_Bool SD_LoadFile       (const char *path, char *buf[]);     // loads in the contents of the file into the buffer
SD_Bool SD_GetDiff        (SD_Diff *diff);                     // analyzes the two source files
SD_Bool SD_GetTreeDiff    (SD_Diff *diff, TSNode a, TSNode b); // gets the edit distance for the two subtrees
SD_Bool SD_PrintDiff      (const SD_Diff *diff);               // prints the diff
SD_Bool SD_OutputTree     (const char* src, TSNode root, int depth);

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

SD_Bool SD_GetDiff(SD_Diff *diff) {
    if (diff == NULL) return SD_FALSE;

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, data.ts_lang);

    TSTree *tree_a = ts_parser_parse_string(parser, NULL, diff->sa, strlen(diff->sa));
    TSTree *tree_b = ts_parser_parse_string(parser, NULL, diff->sb, strlen(diff->sb));

    const TSNode root_a = ts_tree_root_node(tree_a);
    const TSNode root_b = ts_tree_root_node(tree_b);

    SD_GetTreeDiff(diff, root_a, root_b);

    ts_tree_delete(tree_a);
    ts_tree_delete(tree_b);
    ts_parser_delete(parser);
    return SD_TRUE;
}

SD_Bool SD_GetTreeDiff(SD_Diff *diff, const TSNode a, const TSNode b) {
    if (diff == NULL) return SD_FALSE;

    const u32 a_cc = ts_node_child_count(a);
    const u32 b_cc = ts_node_child_count(b);

    const u32 min_children = min(a_cc, b_cc);
    for (u32 idx = 0; idx < min_children; idx++) {
        const TSNode c_a = ts_node_child(a, idx);
        const TSNode c_b = ts_node_child(b, idx);

        if (ts_node_child_count(c_a) != 0 || ts_node_child_count(c_b) != 0) {
            SD_GetTreeDiff(diff, c_a, c_b);
        } else {
            const u32 c_a_b0 = ts_node_start_byte(c_a);
            const u32 c_a_b1 = ts_node_end_byte  (c_a);
            const u32 c_b_b0 = ts_node_start_byte(c_b);
            const u32 c_b_b1 = ts_node_end_byte  (c_b);

            char slice_a[c_a_b1 - c_a_b0 + 1];
            char slice_b[c_b_b1 - c_b_b0 + 1];
            strncpy(slice_a, diff->sa + c_a_b0, c_a_b1 - c_a_b0);
            strncpy(slice_b, diff->sb + c_b_b0, c_b_b1 - c_b_b0);
            slice_a[c_a_b1 - c_a_b0] = '\0';
            slice_b[c_b_b1 - c_b_b0] = '\0';

            if (strcmp(slice_a, slice_b) != 0) {
                diff->op_seq[diff->ops].op       = SD_TREE_RELABEL;
                diff->op_seq[diff->ops].start    = ts_node_start_point(a);
                diff->op_seq[diff->ops].bytes[0] = c_a_b0;
                diff->op_seq[diff->ops].bytes[1] = c_a_b1;
                diff->op_seq[diff->ops].bytes[2] = c_b_b0;
                diff->op_seq[diff->ops].bytes[3] = c_b_b1;
                diff->ops++;
            }
        }
    }

    if (a_cc > b_cc) {
        for (u32 idx = b_cc; idx < a_cc; idx++) {
            const TSNode node = ts_node_child(a, idx);
            diff->op_seq[diff->ops].op       = SD_TREE_DELETE;
            diff->op_seq[diff->ops].start    = ts_node_start_point(node);
            diff->op_seq[diff->ops].bytes[0] = ts_node_start_byte (node);
            diff->op_seq[diff->ops].bytes[1] = ts_node_end_byte   (node);
            diff->ops++;
        }
    } else {
        for (u32 idx = a_cc; idx < b_cc; idx++) {
            const TSNode node = ts_node_child(b, idx);
            diff->op_seq[diff->ops].op       = SD_TREE_INSERT;
            diff->op_seq[diff->ops].start    = ts_node_start_point(node);
            diff->op_seq[diff->ops].bytes[0] = ts_node_start_byte (node);
            diff->op_seq[diff->ops].bytes[1] = ts_node_end_byte   (node);
            diff->ops++;
        }
    }

    return SD_TRUE;
}

SD_Bool SD_OutputTree(const char* src, const TSNode root, const int depth) {
    for (int _ = 0; _ < depth; _++) {
        printf("|   ");
    }

    const u32 start = ts_node_start_byte(root);
    const u32 end   = ts_node_end_byte(root);
    char str[end - start + 1];
    strncpy(str, src + start, end - start);
    str[end - start] = '\0';

    printf("[%s]\n", str);

    for (int childIdx = 0; childIdx < ts_node_child_count(root); childIdx++) {
        const TSNode child = ts_node_child(root, childIdx);
        SD_OutputTree(src, child, depth + 1);
    }

    return 1;
}

SD_Bool SD_PrintDiff(const SD_Diff *diff) {
    u32 cost = 0;
    for (u32 idx = 0; idx < diff->ops; idx++) {
        if (diff->op_seq[idx].op == SD_TREE_RELABEL) {
            const u32 c_a_b0 = diff->op_seq[idx].bytes[0];
            const u32 c_a_b1 = diff->op_seq[idx].bytes[1];
            const u32 c_b_b0 = diff->op_seq[idx].bytes[2];
            const u32 c_b_b1 = diff->op_seq[idx].bytes[3];

            char slice_a[c_a_b1 - c_a_b0 + 1];
            char slice_b[c_b_b1 - c_b_b0 + 1];
            strncpy(slice_a, diff->sa + c_a_b0, c_a_b1 - c_a_b0);
            strncpy(slice_b, diff->sb + c_b_b0, c_b_b1 - c_b_b0);
            slice_a[c_a_b1 - c_a_b0] = '\0';
            slice_b[c_b_b1 - c_b_b0] = '\0';

            cost += SD_TREE_RELABEL_COST * abs((i32) strlen(slice_a) - (i32) strlen(slice_b));

            printf(
                "[%2d:%2d] RELABEL \"%s\", \"%s\"\n",
                diff->op_seq[idx].start.row, diff->op_seq[idx].start.column,
                slice_a, slice_b
            );
        } else {
            if (diff->op_seq[idx].op == SD_TREE_INSERT) {
                const u32 start = diff->op_seq[idx].bytes[0];
                const u32 end   = diff->op_seq[idx].bytes[1];

                char slice[end - start + 1];
                strncpy(slice, diff->sb + start, end - start);
                slice[end - start] = '\0';

                cost += SD_TREE_INSERT_COST * strlen(slice);

                printf(
                    "[%2d:%2d] INSERT  \"%s\"\n",
                    diff->op_seq[idx].start.row, diff->op_seq[idx].start.column,
                    slice
                );
            } else {
                const u32 start = diff->op_seq[idx].bytes[0];
                const u32 end   = diff->op_seq[idx].bytes[1];

                char slice[end - start + 1];
                strncpy(slice, diff->sa + start, end - start);
                slice[end - start] = '\0';

                cost += SD_TREE_DELETE_COST * strlen(slice);

                printf(
                    "[%2d:%2d] DELETE  \"%s\"\n",
                    diff->op_seq[idx].start.row, diff->op_seq[idx].start.column,
                    slice
                );
            }
        }
    }

    printf("Cost of transform: %d\n", cost);
    printf("Transforms: %llu\n", diff->ops);

    return SD_TRUE;
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

        SD_Diff diff = {
            .sa = buf_a,
            .sb = buf_b
        };
        SD_GetDiff  (&diff);
        SD_PrintDiff(&diff);
        return EXIT_SUCCESS;
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
