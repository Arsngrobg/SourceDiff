#include "srcdiff_stdutils.h"
#include "srcdiff.h"

// SDPRIVATE
// size_t sd_count_nodes(TSNode node) {
//     assert(ts_node_is_named(node));

//     size_t count = 1;
//     for (size_t childi = 0; childi < ts_node_named_child_count(node); childi++) {
//         TSNode child = ts_node_named_child(node, childi);
//         count += sd_count_nodes(child);
//     }

//     return count;
// }

SDPRIVATE
int32_t sd_analyse_tree(TSNode root) {
    assert(ts_node_is_named(root));
    return 0;
}

/// Executes structural analysis of the file sequence
SDPUBLIC
int32_t sd_exec_analyse(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_ANALYSE);

    TSParser *parser = ts_parser_new();
    for (size_t idx = 0; idx < sd_get_argc(); idx++) {
        const char *file = sd_get_arg(idx);
              char *ext  = strrchr(file, '.');

        if (ext == NULL) {
            sd_log_warn("no extension for filename '%s' - ignoring", file);
            continue;
        }
        ext++;

        const char *lang_name = sd_lut_mapping_for(sd_get_lut(), ext);
        if (lang_name == NULL) {
            sd_log_warn("no language parser mapped to the file type '%s' - ignoring", ext);
            continue;
        }

        const TSLanguage *lang = sd_load_language(lang_name);
        if (lang == NULL) {
            sd_log_warn("no language parser available for file type '%s' - ignoring", ext);
            continue;
        }

        char *src = sd_read_entire_file(file);
        if (src == NULL)
            continue;

        ts_parser_set_language(parser, lang);
        TSTree *tree = ts_parser_parse_string(parser, NULL, src, strlen(src));
        TSNode  root = ts_tree_root_node(tree);
        sd_analyse_tree(root);
    }

    return EXIT_SUCCESS;
}
