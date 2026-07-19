#include "main.h"
#include "backend/api.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "cli/arg.h"
#include "cli/cli.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    Args *args = parse_args(argc, argv);

    // retrieve file from args
    FILE *f = NULL;
    Arg *arg_file = NULL;

#define FAILED_TO_OPEN                                                         \
    printf("Error: failed to open file -- %s", MORE_INFO);                       \
    exit(EXIT_FAILURE);

    arg_file = Args_find_arg(args, F_FILE, F_FILE_LONG);
    if (!arg_file) {
        FAILED_TO_OPEN
    }

    f = fopen(arg_file->s, "r+");

    if (!f) {
        FAILED_TO_OPEN
    }

    // read
    AniFile af = read_anifile(f);
    AniFile snapshot = af;

    if (Args_find_arg(args, C_ADD, C_ADD)) {
        cmd_add(&af);
    }

    // AniFile_debug_print(af);

    {
        if (AniFile_has_changed(&snapshot, &af)) {
            f = fopen(arg_file->s, "w");
            write_anifile(&af, f);
        };
    }

    // TODO: free memory of Args and AniFile properly at end of program

    return 0;
}
