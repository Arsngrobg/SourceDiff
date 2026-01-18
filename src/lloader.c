// ================================================================================================================== //
//      _____                            ____  _ ________                                                             //
//     / ___/____  __  _______________  / __ \(_) __/ __/                                                             //
//     \__ \/ __ \/ / / / ___/ ___/ _ \/ / / / / /_/ /_                                                               //
//    ___/ / /_/ / /_/ / /  / /__/  __/ /_/ / / __/ __/                                                               //
//   /____/\____/\__,_/_/   \___/\___/_____/_/_/ /_/                                                                  //
//                                                                                                                    //
// ================================================================================================================== //
// File:    lloader.c                                                                                                 //
// Purpose: The language parser loader for SourceDiff. Responsible for compiling/loading Tree Sitter language parsers //
// Author:  Arsngrobg                                                                                                 //
// Version: v1.0                                                                                                      //
// ================================================================================================================== //

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include "libtcc.h"

#define LANGUAGE_DIRECTORY "languages"
#define LANGUAGE_CACHE     "languages/.bin"

/**
 * <p>Compiles the <i>Tree Sitter</i> language parser <i>(+scanners)</i>.</p>
 * <p>The <c>language_id</c> must be a directory in the <c>LANGUAGE_DIRECTORY</c>, if not this function returns
 *    <c>false</c>.
 * </p>
 *
 * @param  language_id the name of the directory that has the <i>Tree Sitter</i> language parser; if the id is invalid,
 *                     then this function returns <c>false</c>
 * @return             <c>true</c> if the compilation was successful, <c>false</c> if compilation failed, or the
 *                     <c>language_id</c> is invalid
 * @author             Arsngrobg
 */
bool SD_CompileLanguage(const char *language_id);
/**
 * <p>Loads the compiled language parser binary, if it exists.</p>
 *
 * @param  language_id the name of the language binary that may exist in <c>LANGUAGE_CACHE</c>
 * @return             <c>true</c> if the injection was successful, <c>false</c> if otherwise
 * @author             Arsngrobg
 */
bool SD_LoadLanguage   (const char *language_id);

bool SD_CompileLanguage(const char *language_id) {
    if (language_id == NULL) return false;

    // format path string
    char path[260];
    if (sprintf(path, LANGUAGE_CACHE"/%s-lang.dll", language_id) == -1) { // TODO: .DLL & .SO support
        return false;
    }

    TCCState *cc = tcc_new();

    // TODO: add tree sitter parser (+scanner)

    tcc_set_output_type(cc, TCC_OUTPUT_DLL);
    tcc_output_file(cc, path);

    tcc_delete(cc);
    return true;
}
