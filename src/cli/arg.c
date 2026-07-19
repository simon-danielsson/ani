#include "arg.h"
#include "../static/guide.h"
#include "../static/help.h"
#include "../utils.h"
#include <libc.h>
#include <stdio.h>
#include <stdlib.h>

char *arg_as_str(ArgType at) {
    static char *args[_ARGS_N] = {[F_HELP] = "-h",
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
        [C_SEARCH] = "search"

    };
    return args[at];
}

Args *Args_init() {
    Args *container = malloc(sizeof(Args));
    if (!container) {
        panic("memory allocation failed");
    }

#define INIT_SIZE 8
    container->items = malloc(INIT_SIZE * sizeof(Arg));
    container->size = 0;
    container->capacity = INIT_SIZE;
#undef INIT_SIZE

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
char *ArgIter_peek(const ArgIter *it) {
    return ArgIter_has_next(it) ? *it->current : NULL;
}
char *ArgIter_next(ArgIter *it) {
    return ArgIter_has_next(it) ? *it->current++ : NULL;
}
bool ArgIter_has_prev(const ArgIter *it) { return it->current > it->begin; }
char *ArgIter_peek_prev(const ArgIter *it) {
    return ArgIter_has_prev(it) ? it->current[-1] : NULL;
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

            // TODO: write guide in static/guide.txt
        } else if (strcmp(arg, arg_as_str(F_GUIDE)) == 0) {
            for (size_t i = 0; i < guide_txt_len; i++) {
                printf("%c", guide_txt[i]);
            }
            exit(EXIT_SUCCESS);

        } else if (strcmp(arg, arg_as_str(F_ICONS)) == 0 ||
                (strcmp(arg, arg_as_str(F_ICONS_LONG)) == 0)) {
            Args_push_arg(args, Arg_new(NULL, F_ICONS));

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

        } else if (strcmp(arg, arg_as_str(C_STATS)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_STATS));

        } else if (strcmp(arg, arg_as_str(C_EDIT)) == 0) {
            CMD_PARSE_ERR("id");
            Args_push_arg(args, Arg_new(ArgIter_next(&it), C_EDIT));

        } else if (strcmp(arg, arg_as_str(C_ADD)) == 0) {
            Args_push_arg(args, Arg_new(NULL, C_ADD));

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
