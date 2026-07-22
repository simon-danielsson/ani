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

// located in the users home dir
#define SETFILE_NAME ".ani"

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    Args *args = parse_args(argc, argv);

#define FAILED_TO_OPEN                                                         \
    printf("Error: failed to open file -- %s", MORE_INFO);                       \
    exit(EXIT_FAILURE);

    // retrieve file from args
    FILE *f = NULL;
    Arg *arg_file = NULL;
    arg_file = Args_find_arg(args, F_FILE, F_FILE_LONG);
    if (arg_file) {
        f = fopen(arg_file->s, "r+");
    } else if (!f) {

        FILE *f_fallback = NULL;
        char tmp[256];
        {
            char *ani_loc = ".ani";
            char *home = getenv("HOME");
            snprintf(tmp, sizeof(tmp), "%s/%s", home, ani_loc);
            f_fallback = fopen(tmp, "r+");
        }

        if (!f_fallback) {
            printf("Missing .ani file: %s\n", tmp);
            printf("This is required to run ani without a file flag...\n");
            FAILED_TO_OPEN
        }

        // retrieve path of fallback file within HOME/.ani
        char *fallback_file = get_set_ani_path(f_fallback);

        f = fopen(fallback_file, "r+");
        arg_file = &(Arg){.s = fallback_file, .t = F_FILE};

        if (!f) {
            printf("Error: missing or broken fallback '%s' inside '%s'\n",
                    fallback_file, tmp);
            FAILED_TO_OPEN
        }
    }

    // read
    AniFile af = read_anifile(f);
    AniFile snapshot = af;

    bool use_devicons = true;
    if (Args_find_arg(args, F_ICONS, F_ICONS_LONG) != NULL) {
        use_devicons = false;
    }

    if (Args_find_arg(args, C_ADD, C_ADD)) {
        cmd_add(&af);
    }

    if (Args_find_arg(args, C_REC, C_REC)) {
        cmd_rec(&af, use_devicons);
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
        Arg *tmp = Args_find_arg(args, C_SEARCH, C_SEARCH);
        if (tmp) {
            cmd_search(&af, tmp->s, use_devicons);
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
