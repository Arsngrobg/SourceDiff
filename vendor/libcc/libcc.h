//     ,──.   ,──.,──.    ,─────. ,─────.
//     │  │   `──'│  │─. '  .──./'  .──./
//     │  │   ,──.│ .─. '│  │    │  │
//     │  '──.│  ││ `─' │'  '──'╲'  '──'╲
//     `─────'`──' `───'  `─────' `─────'
//
//     LibCC - lightweight C compiler invocation library

#ifndef LIBCC_H
#define LIBCC_H

#define CC_COMPILER_DEFAULT "cc"
#define CC_COMPILER_GCC     "gcc"
#define CC_COMPILER_CLANG   "clang"

#if defined(_WIN32)
#define CC_SHARED_LIB_EXT   "dll"
#define CC_SHARED_FLAG      "-shared"
#elif defined(__APPLE__) || defined(__MACH__)
#define CC_SHARED_LIB_EXT   "dylib"
#define CC_SHARED_FLAG      "-dynamiclib"
#else
#define CC_SHARED_LIB_EXT   "so"
#define CC_SHARED_FLAG      "-shared"
#endif

#include <stdint.h>
#include <stdbool.h>

// State
typedef struct CC_Toolchain CC_Toolchain;

// Construction
CC_Toolchain *cc_new   (void);
void          cc_delete(CC_Toolchain *cc);

// Fundamental Operations
bool        cc_set_compiler        (CC_Toolchain *cc, const char *ccid);       // [<CCID>]
bool        cc_add_option          (CC_Toolchain *cc, const char *option);     // [<CCID>] [<OPTION>]
bool        cc_add_include_path    (CC_Toolchain *cc, const char *pathf, ...); // [<CCID>] -I[<PATH>]
bool        cc_add_library_path    (CC_Toolchain *cc, const char *pathf, ...); // [<CCID>] -L[<PATH>]
bool        cc_add_library         (CC_Toolchain *cc, const char *libf,  ...); // [<CCID>] -l[<LIB>]
bool        cc_add_source          (CC_Toolchain *cc, const char *filef, ...); // [<CCID>] [<FILE>]
bool        cc_set_output          (CC_Toolchain *cc, const char *outf,  ...); // [<CCID>] -o [<OUT>]
const char *cc_render_command      (CC_Toolchain *cc);                         // [<CCID>] [<OPTION>]* (-I[<PATH>])* (-L[<PATH>])* (-l[<LIB>])* [<FILE>]+ -o [<OUT>]
int32_t     cc_invoke              (CC_Toolchain *cc);

// Extensions
typedef enum {
    CC_OUTPUT_PREPROCESSED  = 0, // .i
    CC_OUTPUT_ASSEMBLY      = 1, // .S
    CC_OUTPUT_OBJECT        = 2, // .o
    CC_OUTPUT_INTERMEDIATES = 3, // .i,.S,.o
    CC_OUTPUT_SHARED        = 4, // .so/.dll/.dylib
    CC_OUTPUT_EXE           = 5  // .exe
} CC_OutputType;

bool        cc_set_output_type     (CC_Toolchain *cc, CC_OutputType out_type);

#endif // LIBCC_H
