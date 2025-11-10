#include <stdio.h>
#include <string.h>

#include "tree_sitter/api.h"

const TSLanguage *tree_sitter_c(void);

void print_tree(const TSNode root, const int depth) {
    for (int _ = 0; _ < depth; _++) {
        printf("|   ");
    }
    printf("[%s]\n", ts_node_type(root));
    for (int childIdx = 0; childIdx < ts_node_named_child_count(root); childIdx++) {
        const TSNode child = ts_node_named_child(root, childIdx);
        print_tree(child, depth + 1);
    }
}

int main(void) {
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    const char *source_code =
        "#include <stdio.h>\n\n"
        "int main() {\n"
        "    printf(\"Hello, World!\\n\");\n"
        "    return 0;\n"
        "};\n";

    printf("-----main.c-----\n%s\n----------------\n", source_code);

    TSTree *tree = ts_parser_parse_string(
        parser, NULL,
        source_code,
        strlen(source_code)
    );

    const TSNode root = ts_tree_root_node(tree);
    print_tree(root, 0);

    ts_parser_delete(parser);
    ts_tree_delete(tree);

    return 0;
}