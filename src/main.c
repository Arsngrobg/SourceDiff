#include <stdio.h>
#include <string.h>

#include "tree_sitter/api.h"

const TSLanguage *tree_sitter_c(void);

int main(void) {
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    const char *source_code =
    "#include <stdio.h>\n\n"
    "int main() {\n"
    "    printf(\"Hello, World!\\n\");\n"
    "    return 0;\n"
    "};\n";

    printf("-----main.c-----\n%s\n----------------", source_code);

    TSTree *tree = ts_parser_parse_string(
        parser, NULL,
        source_code,
        strlen(source_code)
    );
    const TSNode root = ts_tree_root_node(tree);
    char *string = ts_node_string(root);
    printf("Syntax tree: %s\n", string);
    return 0;
}