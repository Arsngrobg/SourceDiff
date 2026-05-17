#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "libcc.h"

#include "srcdiff.h"

#define SD_LANGDUMP "languages"

/// Registers a Tree Sitter grammar to SourceDiff
SDPUBLIC
int32_t sd_exec_register(void) {
    assert(sd_is_argv_parsed() && sd_get_mode() == SD_MODE_REGISTER);

    if (!sd_enter_bin_dir())
        return EXIT_FAILURE;

    const char *dirstr = sd_get_arg(1);

    struct dirent *entry;
    DIR *dir = opendir(dirstr);
    if (dir == NULL) {
        sd_log_error("directory does not exist");
        return EXIT_FAILURE;
    }

    bool has_include = false;
    bool has_parser  = false;
    bool has_scanner = false;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "parser.c") == 0) {
            sd_log_debug("Found parser.c");
            has_parser = true;
        } else if (strcmp(entry->d_name, "scanner.c") == 0) {
            sd_log_debug("Found scanner.c");
            has_scanner = true;
        } else if (strcmp(entry->d_name, "tree_sitter") == 0) {
            sd_log_debug("Found tree_sitter directory");
            has_include = true;
        }
    }

    // the default setup for a tree-sitter grammar
    if (!has_include && !has_parser) {
        sd_log_error("does not match conventional tree-sitter grammar structure");
        return EXIT_FAILURE;
    }

    CC_Toolchain *cc = cc_new();
    if (cc == NULL) {
        return EXIT_FAILURE;
    }

    cc_add_option(cc, "-O2");

    cc_add_source(cc, "%s/parser.c", dirstr);
    cc_add_include_path(cc, "%s/tree_sitter", dirstr);
    if (has_scanner) {
        cc_add_source(cc, "%s/scanner.c", dirstr);
    }

    if (mkdir(SD_LANGDUMP) == 0) {
        sd_log_debug("Created "SD_LANGDUMP" directory");
    }

    cc_set_output_type(cc, CC_OUTPUT_SHARED);
    cc_set_output(cc, SD_LANGDUMP"/%s."CC_SHARED_LIB_EXT, sd_get_arg(0));
    cc_invoke(cc);

    sd_log_debug("INVOKED COMMAND: %s", cc_render_command(cc));

    cc_delete(cc);
    closedir(dir);
    return !sd_exit_bin_dir();
}
