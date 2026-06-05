#include "srcdiff_stdutils.h"
#include "srcdiff.h"

#define SRCDIFF_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define SRCDIFF_MAX(a, b) (((a) < (b)) ? (b) : (a))

/// Computes the difference between two file, where: F1 - F2
SDPUBLIC
int32_t sd_exec_diff(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_DIFF);

    // resources
    TSParser *parser = NULL;
    char     *src_a  = NULL;
    char     *src_b  = NULL;
    TSTree   *tree_a = NULL;
    TSTree   *tree_b = NULL;

    int32_t status = EXIT_SUCCESS;

    // load the file into memory
    const char *file_a = sd_get_arg(0);
    const char *file_b = sd_get_arg(1);
    src_a = sd_read_entire_file(file_a);
    src_b = sd_read_entire_file(file_b);

    // extract file extension and load the language
    char *ext_a = strrchr(file_a, '.');
    char *ext_b = strrchr(file_b, '.');
    if (ext_a == NULL || ext_b == NULL) { // no file extension for either
        sd_log_error("either files do not have a file extensions");
        goto defer;
    }
    ext_a++;
    ext_b++;

    SDLut *lut = sd_get_lut();
    const TSLanguage *lang_a = sd_load_language(sd_lut_mapping_for(lut, ext_a));
    const TSLanguage *lang_b = sd_load_language(sd_lut_mapping_for(lut, ext_b));
    if (lang_a != lang_b) { // languages do not match
        sd_log_error("both files map to different language types - will not execute diff analysis");
        goto defer;
    }

    // parse source files
    parser = ts_parser_new();
    ts_parser_set_language(parser, lang_a);

    tree_a = ts_parser_parse_string(parser, NULL, src_a, strlen(src_a));
    tree_b = ts_parser_parse_string(parser, NULL, src_b, strlen(src_b));

    TSNode root_a = ts_tree_root_node(tree_a);
    TSNode root_b = ts_tree_root_node(tree_b);
    (void)root_a;
    (void)root_b;

    // compute edit distance
    printf("Yeah mate\n");
    return 0;

defer:
    ts_parser_delete(parser);
    ts_tree_delete(tree_a);
    ts_tree_delete(tree_b);
    free(src_a);
    free(src_b);
    return status;
}
