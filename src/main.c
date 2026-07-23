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
    USE_DEVICONS = true;

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

    if (Args_find_arg(args, C_ADD, C_ADD)) {
        cmd_add(&af);
    }

    // first field is for the -r reverse flag if added
    // second field is for sorting flag itself
    ArgType sort_type;
    bool reverse_sort = false;

    if (Args_find_arg(args, F_SORT_REVR, F_SORT_REVR)) {
        reverse_sort = true;
    }
    if (Args_find_arg(args, F_SORT_NAME, F_SORT_NAME)) {
        sort_type = F_SORT_NAME;
    }
    if (Args_find_arg(args, F_SORT_PROG, F_SORT_PROG)) {
        sort_type = F_SORT_PROG;
    }
    if (Args_find_arg(args, F_SORT_UPDA, F_SORT_UPDA)) {
        sort_type = F_SORT_UPDA;
    }
    if (Args_find_arg(args, F_SORT_SCOR, F_SORT_SCOR)) {
        sort_type = F_SORT_SCOR;
    }
    if (Args_find_arg(args, F_SORT_RELE, F_SORT_RELE)) {
        sort_type = F_SORT_RELE;
    }

    if (Args_find_arg(args, C_LIST, C_LS)) {

        ArgType list_type = C_LIST;

        if (Args_find_arg(args, C_LIST_COMPL, C_LIST_COMPL)) {
            list_type = C_LIST_COMPL;
        }
        if (Args_find_arg(args, C_LIST_DROPP, C_LIST_DROPP)) {
            list_type = C_LIST_DROPP;
        }
        if (Args_find_arg(args, C_LIST_ONHOL, C_LIST_ONHOL)) {
            list_type = C_LIST_ONHOL;
        }
        if (Args_find_arg(args, C_LIST_PLANN, C_LIST_PLANN)) {
            list_type = C_LIST_PLANN;
        }
        if (Args_find_arg(args, C_LIST_WATCH, C_LIST_WATCH)) {
            list_type = C_LIST_WATCH;
        }

        cmd_list(&af, list_type, reverse_sort, sort_type);
    }

    if (Args_find_arg(args, C_REC, C_REC)) {
        cmd_rec(&af);
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
            cmd_search(&af, tmp->s);
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
            cmd_info(&af, atoi(tmp->s));
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
