#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "libtcc.h"

int compile_language(const char *language_id) {
    if (language_id == NULL) return false;
    TCCState *cc = tcc_new();
    tcc_delete(cc);
    return true;
}

// TOOD: make application
int main(void) {
    compile_language("c");
    return 0;
}
