#include "../static/guide.h"
#include "../static/help.h"
#include <libc.h>
#include <stdio.h>
#include <stdlib.h>

#define MORE_INFO "run 'ani -h' for more information"

typedef enum {
    F_HELP,
    F_HELP_LONG,
    F_GUIDE,
    F_GUIDE_LONG,
    F_FILE,
    F_FILE_LONG,
    _ARGS_N
} ArgType;

char *arg_as_str(ArgType at) {
    char *args[_ARGS_N] = {"-h", "--help", "-g", "--guide", "-f", "--file"};
    return args[at];
}

void process_args(int argc, char **argv) {
    if (argc < 2) {
        printf("No arguments were provided -- %s\n", MORE_INFO);
        exit(EXIT_FAILURE);
    }

    // help
    if (strcmp(argv[1], arg_as_str(F_HELP)) == 0 ||
            strcmp(argv[1], arg_as_str(F_HELP_LONG)) == 0) {
        for (size_t i = 0; i < help_txt_len; i++) {
            printf("%c", help_txt[i]);
        }
        exit(EXIT_SUCCESS);
    }

    // guide
    // TODO: write guide in static/guide.txt
    if (strcmp(argv[1], arg_as_str(F_GUIDE)) == 0 ||
            strcmp(argv[1], arg_as_str(F_GUIDE_LONG)) == 0) {
        for (size_t i = 0; i < guide_txt_len; i++) {
            printf("%c", guide_txt[i]);
        }
        exit(EXIT_SUCCESS);
    }

    // if this point of the function is reached, the argument is unknown
    printf("Unknown argument '%s' -- %s\n", argv[1], MORE_INFO);
    exit(EXIT_FAILURE);

    return;
}

/*

   USAGE
   ani <option|command>

   META OPTIONS
   -h, --help            Displays this help message.
   -g, --guide           Display practical guide on typical usage.

   OPTIONS
   -f, --file            Source file to read and/or modify, can be omitted if
   a file has been set as default with the 'set' command.
   -n, --no-icons        Replace nerdfont icons with more portable fallbacks.

   COMMANDS
   add                   Add a new entry in an interactive prompt.
   ep <id>               Change episode progress in an interactive prompt.
   If episode number chosen is more or equal to the total
   number of episodes of the entry, its status will
   change to 'completed' by default.
   edit <id>             Edit an entry in an interactive prompt.
   info <id>             Print info of given entry.
   rm <id>               Remove an entry (non-reversible).
   stats                 Display global statistics.

   set [command]
   new <src-file>    Set a default/fallback path to a src-file, will be
   used if the '--file' flag has not been provided.
   rm                Remove default path (does not remove the file itself).
   ls                Print location of default path (if there is one).

   search <word>         Search entries by name, note, and tags.

   ls [command, flags]   List all entries
   watching          List watching
   completed         List completed
   on-hold           List on-hold
   dropped           List dropped
   planned           List plan-to-watch
   -r                Reverse sorting (combined as a modifier with other
   sorting flags) -n                Sort by name -s                Sort by user
   score/rating -u                Sort by last updated -d                Sort by
   entry release year -p                Sort by entry episode progress

*/
