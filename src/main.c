#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(source1, "source1", "The source file to compare with source2") \
    REQUIRED_STRING_ARG(source2, "source2", "The source file to compare with source1")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help,    "-h", "Shows the usage of SourceDiff") \
    BOOLEAN_ARG(version, "-v", "Prints the current version of SourceDiff") \

#include "easyargs.h"

int main(const int argc, char *argv[]) {
    args_t args = make_default_args();
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help("SourceDiff");
        return 1;
    }

    if (args.version) {
        printf("SourceDiff (ver 1.0)\n");
    } else if (args.source1 && args.source2) {
        printf("%s, %s\n", args.source1, args.source2);
    }

    return 0;
}
