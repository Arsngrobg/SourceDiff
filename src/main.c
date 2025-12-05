#include <stdbool.h>
#include <stdio.h>

#define

#define HELP                                    \
"SourceDiff (ver %s) Usage:                 \n" \
"   Usage:                                  \n" \
"       - SourceDiff [-h/--help]            \n" \
"         Displays the usage of SourceDiff  \n" \
"                                           \n" \
"       - SourceDiff [-v/--version]         \n" \
"         Displays the version of SourceDiff\n" \

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION VERSION_MAJOR#"."#VERSION_MINOR

#define SHORTHAND_HELP    ("-h"       )
#define LONGHAND_HELP     ("--help"   )
#define SHORTHAND_VERSION ("-v"       )
#define LONGHAND_VERSION  ("--version")

bool SD_CLI_ParseCommands(int argc, char *argv[]) {
    if (argc == 1) {
        printf(HELP, VERSION);
        return true;
    }

    return false;
}

int main(int argc, char *argv[]) {
    if (SD_CLI_ParseCommands(argc, argv)) {
        return 1;
    }
    return 0;
}
