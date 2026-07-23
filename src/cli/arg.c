#include "arg.h"
#include "../static/guide.h"
#include "../static/help.h"
#include "../utils.h"

bool USE_DEVICONS = true;

char *arg_as_str(ArgType at) {
    static char *args[_ARGS_N] = {
        [F_HELP] = "-h",
        [F_HELP_LONG] = "--help",
        [F_GUIDE] = "--guide",
        [F_FILE] = "-f",
        [F_FILE_LONG] = "--file",
        [F_ICONS] = "-i",
        [F_ICONS_LONG] = "--no-icons",
        [C_ADD] = "add",
        [C_EP] = "ep",
        [C_EDIT] = "edit",
        [C_INFO] = "info",
        [C_RM] = "rm",
        [C_STATS] = "stats",
        [C_REC] = "rec",
        [C_SEARCH] = "search",
        [C_LIST] = "list",
        [C_LS] = "ls",
        [C_LIST_WATCH] = "watching",
        [C_LIST_COMPL] = "completed",
        [C_LIST_ONHOL] = "on-hold",
        [C_LIST_DROPP] = "dropped",
        [C_LIST_PLANN] = "planned",
        [F_SORT_REVR] = "-r",
        [F_SORT_NAME] = "-n",
        [F_SORT_SCOR] = "-s",
        [F_SORT_UPDA] = "-u",
        [F_SORT_RELE] = "-y",
        [F_SORT_PROG] = "-p",
        [F_VERS] = "-v",
        [F_VERS_LONG] = "--version",
    };
    return args[at];
}

Args *Args_init() {
    Args *container = malloc(sizeof(Args));
    if (!container) {
        panic("memory allocation failed");
    }

    static int init_size = 8;

    container->items = malloc(init_size * sizeof(Arg));
    container->size = 0;
    container->capacity = init_size;

    if (!container->items) {
        free(container);
        panic("memory allocation failed");
    }
    return container;
}

void Args_push_arg(Args *args, Arg a) {
    if (args->size == args->capacity) {
        size_t new_capacity = args->capacity << 1;
        Arg *new_items = realloc(args->items, new_capacity * sizeof(Arg));
        if (!new_items) {
            panic("out of memory\n");
        }
        args->items = new_items;
        args->capacity = new_capacity;
    }
    args->items[args->size++] = a;
}

Arg Arg_new(const char *s, ArgType t) {
    Arg a = {0};
    a.t = t;
    if (s) {
        a.s = malloc((strlen(s) + 1) * sizeof(char));
        if (a.s) {
            strcpy(a.s, s);
        }
    }
    return a;
}

typedef struct {
    char **begin;   // first argument (argv+1)
    char **current; // next item to return
    char **end;     // one-past-last
} ArgIter;

ArgIter ArgIter_init(int argc, char **argv) {
    char **first = argv + 1;
    return (ArgIter){.begin = first, .current = first, .end = argv + argc};
}

bool ArgIter_has_next(const ArgIter *it) { return it->current < it->end; }

char *ArgIter_next(ArgIter *it) {
    return ArgIter_has_next(it) ? *it->current++ : NULL;
}

#define CMD_PARSE_ERR(not_provided)                                            \
    do {                                                                         \
        if (!ArgIter_has_next(&it)) {                                              \
            fprintf(stderr, "No %s provided after '%s' -- %s\n", (not_provided),     \
                    arg, MORE_INFO);                                                 \
            exit(EXIT_FAILURE);                                                      \
        }                                                                          \
    } while (0)

Args *parse_args(int argc, char **argv) {
    if (argc < 2) {
        printf("No arguments were provided -- %s\n", MORE_INFO);
        exit(EXIT_FAILURE);
    }

    ArgIter it = ArgIter_init(argc, argv);
    Args *args = Args_init();

    while (ArgIter_has_next(&it)) {
        char *arg = ArgIter_next(&it);

        if (strcmp(arg, arg_as_str(F_HELP)) == 0 ||
                (strcmp(arg, arg_as_str(F_HELP_LONG)) == 0)) {
            for (size_t i = 0; i < help_txt_len; i++) {
                printf("%c", help_txt[i]);
            }
            exit(EXIT_SUCCESS);
        }

        if (strcmp(arg, arg_as_str(F_VERS)) == 0 ||
                (strcmp(arg, arg_as_str(F_VERS_LONG)) == 0)) {
            printf("========================================\n");
            printf("%s %s (%.8s)\n", ENV_NAME, ENV_GITTAG, ENV_GITHASH);
            printf("Anime progress tracker for the CLI.\n");
            printf("%s\n", ENV_REPO);
            printf("----------------------------------------\n");
            printf("© 2026 %s - MIT License\n", ENV_AUTHOR);
            printf("Contact: %s\n", ENV_CONTACT);
            printf("========================================\n");
            exit(EXIT_SUCCESS);

        } else if (strcmp(arg, arg_as_str(F_GUIDE)) == 0) {
            for (size_t i = 0; i < guide_txt_len; i++) {
                printf("%c", guide_txt[i]);
            }
            exit(EXIT_SUCCESS);

        } else if (strcmp(arg, arg_as_str(F_ICONS)) == 0 ||
                (strcmp(arg, arg_as_str(F_ICONS_LONG)) == 0)) {
            // Args_push_arg(args, Arg_new(NULL, F_ICONS));
            USE_DEVICONS = false;

        } else if (strcmp(arg, arg_as_str(F_FILE)) == 0 ||
                (strcmp(arg, arg_as_str(F_FILE_LONG)) == 0)) {
            CMD_PARSE_ERR("path");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), F_FILE));

        } else if (strcmp(arg, arg_as_str(C_EP)) == 0) {
            CMD_PARSE_ERR("id");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_EP));

        } else if (strcmp(arg, arg_as_str(C_RM)) == 0) {
            CMD_PARSE_ERR("id");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_RM));

        } else if (strcmp(arg, arg_as_str(C_SEARCH)) == 0) {
            CMD_PARSE_ERR("search term");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_SEARCH));

        } else if (strcmp(arg, arg_as_str(C_REC)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_REC));

        } else if (strcmp(arg, arg_as_str(C_INFO)) == 0) {
            CMD_PARSE_ERR("id");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_INFO));

        } else if (strcmp(arg, arg_as_str(C_EDIT)) == 0) {
            CMD_PARSE_ERR("id");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_EDIT));

        } else if (strcmp(arg, arg_as_str(C_ADD)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_ADD));

        } else if (strcmp(arg, arg_as_str(C_STATS)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_STATS));

            // ls command types
        } else if (strcmp(arg, arg_as_str(C_LIST)) == 0 ||
                strcmp(arg, arg_as_str(C_LS)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST));
        } else if (strcmp(arg, arg_as_str(C_LIST_COMPL)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST_COMPL));
        } else if (strcmp(arg, arg_as_str(C_LIST_DROPP)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST_DROPP));
        } else if (strcmp(arg, arg_as_str(C_LIST_ONHOL)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST_ONHOL));
        } else if (strcmp(arg, arg_as_str(C_LIST_PLANN)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST_PLANN));
        } else if (strcmp(arg, arg_as_str(C_LIST_WATCH)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_LIST_WATCH));

            // sorting flags
        } else if (strcmp(arg, arg_as_str(F_SORT_NAME)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_NAME));
        } else if (strcmp(arg, arg_as_str(F_SORT_PROG)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_PROG));
        } else if (strcmp(arg, arg_as_str(F_SORT_RELE)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_RELE));
        } else if (strcmp(arg, arg_as_str(F_SORT_REVR)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_REVR));
        } else if (strcmp(arg, arg_as_str(F_SORT_SCOR)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_SCOR));
        } else if (strcmp(arg, arg_as_str(F_SORT_UPDA)) == 0) {
            Args_push_arg(args, Arg_new(NULL, F_SORT_UPDA));

        } else {
            printf("Unknown argument '%s' -- %s\n", arg, MORE_INFO);
            exit(EXIT_FAILURE);
        }
    }

    return args;
}

// returns NULL if arg can't be found
Arg *Args_find_arg(const Args *args, ArgType t1, ArgType t2) {

    for (size_t i = 0; i < args->size; i++) {
        if (args->items[i].t == t1 || args->items[i].t == t2) {
            return &args->items[i];
        }
    }
    return NULL;
}
