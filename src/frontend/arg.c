#include "../main.h"
#include "../utils.h"
#include "frontend.h"

/*
   TODO: make it clear to user in help/guide text that only two sorting
   flags can be active at any one time, any other ones added will be
   silently ignored
   */

/*
   reference for how to structure this nicely
   timestamp: ~ 01:12:00
   https://www.youtube.com/watch?v=443UNeGrFoM
   */

static char *ArgType_sorting_flag_to_str(const ArgType at) {
    switch (at) {
        case SF_REVERSE:
            return "reverse";
        case SF_NAME:
            return "name";
        case SF_PROGRESS:
            return "progress";
        case SF_RELEASED:
            return "released";
        case SF_SCORE:
            return "score";
        default:
            return "updated";
    }
}

static Arg _Arg_create(ArgType t, const char *n1, const char *n2, void *cmd,
        bool has_sub) {
    Arg a = {0};
    a.t = t;
    a.provided = false;
    a.is_sort_flag = false;
    a.names[0] = n1;
    a.names[1] = n2;
    a.cmd = cmd;
    a.has_sub = has_sub;
    return a;
}

static Arg _Arg_create_sort_flag(ArgType t, const char *n1) {
    Arg a = {0};
    a.t = t;
    a.provided = false;
    a.is_sort_flag = true;
    a.names[0] = n1;
    a.names[1] = NULL;
    a.cmd = NULL;
    a.has_sub = false;
    return a;
}

static Arg _Arg_create_cmd(ArgType t, const char *n1, void *cmd, bool has_sub) {
    Arg a = {0};
    a.t = t;
    a.provided = false;
    a.is_sort_flag = false;
    a.names[0] = n1;
    a.names[1] = NULL;
    a.cmd = cmd;
    a.has_sub = has_sub;
    return a;
}

static Arg args[ARG_COUNT];

void Arg_init_all(void) {
    // flags
    args[0] = _Arg_create(F_FILE, "--file", "-f", NULL, true);
    args[1] = _Arg_create(F_HELP, "--help", "-h", flag_help, false);
    args[2] = _Arg_create(F_VERSION, "--version", "-v", flag_version, false);
    args[3] = _Arg_create(F_GUIDE, "--guide", NULL, flag_guide, false);

    // commands
    args[4] = _Arg_create(C_SEARCH, "search", NULL, cmd_search, true);
    args[5] = _Arg_create(C_LIST, "list", "ls", cmd_list, true);
    args[6] = _Arg_create(C_ADD, "add", NULL, cmd_add, true);
    args[7] = _Arg_create(C_EDIT, "edit", NULL, cmd_edit, true);
    args[8] = _Arg_create(C_REMOVE, "remove", "rm", cmd_rm, true);
    args[9] = _Arg_create(C_INFO, "info", NULL, cmd_info, true);
    args[10] = _Arg_create(C_REC, "rec", NULL, cmd_rec, false);
    args[11] = _Arg_create(C_STATS, "stats", "summary", cmd_stats, false);
    args[12] = _Arg_create(C_EP, "ep", "episode", cmd_ep, false);

    // sorting flags
    args[13] = _Arg_create_sort_flag(SF_NAME, "-n");
    args[14] = _Arg_create_sort_flag(SF_PROGRESS, "-p");
    args[15] = _Arg_create_sort_flag(SF_RELEASED, "-y");
    args[16] = _Arg_create_sort_flag(SF_REVERSE, "-r");
    args[17] = _Arg_create_sort_flag(SF_SCORE, "-s");
    args[18] = _Arg_create_sort_flag(SF_UPDATED, "-u");
}

bool Arg_parse(uint argc, char **argv) {
    if (argc < 2) {
        printf("No arguments were provided -- %s\n", MORE_INFO);
        return false;
    }

    uint i = 1;
    while (i < argc) {
        for (int j = 0; j < ARG_COUNT; j++) {
            bool matched = false;

            if ((args[j].names[0] && !matched) &&
                    (strcmp(argv[i], args[j].names[0]) == 0)) {
                args[j].provided = true;
                matched = true;
            }

            if ((args[j].names[1] && !matched) &&
                    (strcmp(argv[i], args[j].names[1]) == 0)) {
                args[j].provided = true;
                matched = true;
            }

            if (!matched) {
                continue;
            }

            if (args[j].provided && args[j].has_sub) {
                int k = 0;
                while (i + 1 < argc && k < ARG_MAX_PARAM - 1) {
                    if (argv[i + 1][0] == '-') {
                        break;
                    }
                    i++;
                    args[j].param[k++] = argv[i];
                }
                args[j].param[k] = NULL;
            }
        }
        i++;
    }
    return true;
}

PrgVars PrgVars_init() {
    PrgVars pv = {0};
    pv.cmd = NULL;
    memset(pv.filepath, 0, sizeof(char) * 128);
    pv.params = NULL;
    pv.sort_flags[0] = NULL;
    pv.sort_flags[1] = NULL;
    pv.sort_flags_count = 0;
    return pv;
}

void PrgVars_setup(PrgVars *pv) {
    bool cmd_chosen = false;

    for (uint i = 0; i < ARG_COUNT; i++) {
        if (args[i].provided) {

            if (args[i].is_sort_flag && pv->sort_flags_count < 2) {
                pv->sort_flags[pv->sort_flags_count] = &args[i].t;
                pv->sort_flags_count++;
                /*
                   TODO: Sorting is soft-capped at two flags because it simply doesn't
                   make sense to have more than two. You sort by date, name, progress or
                   whichever flag you want, and then you add a second flag "-r" to
                   reverse whatever sort you wanted.

                   But at the moment I am not checking whether double sorting flags have
                   been added or if double reverse flags have been added etc. This setup
                   function should only accept 2 sorting flags:
                   1. a sort flag
                   2. an optional reverse flag
                   */

            } else if (args[i].t == F_FILE) {
                if (args[i].param[0]) {
                    strcpy(pv->filepath, args[i].param[0]);
                }
            } else if (args[i].cmd != NULL && !cmd_chosen) {
                cmd_chosen = true;
                pv->cmd = args[i].cmd;
                pv->params = args[i].param;
            }
        }
    }
}

void PrgVars_debug_print(PrgVars *pv) {
    printf("----- DEBUG PrgVars -----\n\n");

    printf("filepath: %s\n", pv->filepath);
    char *par = *pv->params;
    for (uint i = 0; par != NULL; i++) {
        printf("param: %s\n", par++);
    }
    for (size_t i = 0; i < pv->sort_flags_count; i++) {
        printf("sf: %s\n", ArgType_sorting_flag_to_str(*pv->sort_flags[i]));
    }

    printf("\n-------------------------\n");
}
