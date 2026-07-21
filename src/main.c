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

    bool use_devicons = ({
            Arg *a = Args_find_arg(args, F_ICONS, F_ICONS_LONG);
            !a;
            });

    if (Args_find_arg(args, C_ADD, C_ADD)) {
        cmd_add(&af);
    }

    if (Args_find_arg(args, C_STATS, C_STATS)) {
        cmd_stats(&af);
    }

    {
        Arg *tmp = Args_find_arg(args, C_EDIT, C_EDIT);
        if (tmp) {
            cmd_edit(&af, atoi(tmp->s));
            f = fopen(arg_file->s, "w");
            write_anifile(&af, f);
            return 0;
        }
    }

    {
        Arg *tmp = Args_find_arg(args, C_EP, C_EP);
        if (tmp) {
            cmd_ep(&af, atoi(tmp->s));
        }
    }

    {
        Arg *tmp = Args_find_arg(args, C_RM, C_RM);
        if (tmp) {
            cmd_rm(&af, atoi(tmp->s));
        }
    }

    {
        Arg *tmp = Args_find_arg(args, C_INFO, C_INFO);
        if (tmp) {
            cmd_info(&af, atoi(tmp->s), use_devicons);
        }
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
