#include "../backend/api.h"
#include "../backend/io.h"
#include "../utils.h"

/*
   commands that will be using the prompt:

   add: Add a new entry in an interactive prompt.

   ep <id>: Change episode progress in an interactive prompt.

   edit <id>: Edit an entry in an interactive prompt.
   */

int prompt_ep(int watched_episodes, int total_episodes) {
    printf("Current progress: %d out of %d\n", watched_episodes, total_episodes);
    printf("=> ");
    int r = 0;
    char tmp[128] = {0};
    int c_count = 0;
    for (int ch; (ch = getchar()) != EOF;) {
        if (ch == '\n') {
            r = atoi(tmp);
            break;
        }
        tmp[c_count] = ch;
        c_count++;
    }
    return r;
}

// takes char[n] questions and n of array
// returns char[n] of answers with the same order/length
char **prompt_add(const char **q, int n_q) {

    char **answers = calloc(n_q, sizeof(*answers));

    for (int current_q = 0; current_q < n_q; current_q++) {
        printf("%s\n", q[current_q]);
        printf("=> ");
        char tmp[128] = {0};
        int c_count = 0;
        for (int ch; (ch = getchar()) != EOF;) {
            if (ch == '\n') {
                answers[current_q] = malloc((strlen(tmp) + 1) * sizeof(char));
                strcpy(answers[current_q], tmp);
                break;
            }
            tmp[c_count] = ch;
            c_count++;
        }
    }

    return answers;
}

const char *prompt_field(enum AniCurrentFieldState at) {
    static char *args[_FIELDS_N] = {
        [NAME] = "Name",
        [YEAR] = "Release year (YYYY)",
        [EP_TOT] = "Total number of episodes",
        [EP_WAT] = "Episodes watched",
        [TAGS] = "Tags (separated by ',')",
        [SCORE] = "User score (from 1 to 10)",
        [NOTE] = "Personal note",
        [STAT] = "Current watch status\n\
                  [w]atching, [c]ompleted, [o]n hold, \
                      [d]ropped, [p]lan to watch ",
    };
    return args[at];
}

void cmd_add(AniFile *af) {

#define Q 8
    const char *q[Q] = {
        prompt_field(NAME),   prompt_field(YEAR), prompt_field(EP_TOT),
        prompt_field(EP_WAT), prompt_field(TAGS), prompt_field(SCORE),
        prompt_field(NOTE),   prompt_field(STAT),
    };

    char **a = prompt_add(q, Q);

    printf("------\n");

    AniEntry e = {0};

    // id
    e.id = AniFile_get_most_recent_id(af) + 1;

    // name
    trim_str(a[0]);
    e.name = malloc((strlen(a[0]) + 1) * sizeof(char));
    strcpy(e.name, a[0]);
    free(a[0]);

    e.released = time_t_from_iso_ymd(a[1]);
    free(a[1]);

    e.ep_total = atoi(a[2]);
    free(a[2]);

    e.ep_watched = atoi(a[3]);
    free(a[3]);

    if (!str_is_empty(a[4])) {
        e.tags = get_tags_from_field(a[4]);
    } else {
        e.tags = NULL;
    }
    free(a[4]);

    e.score = atoi(a[5]);
    free(a[5]);

    trim_str(a[6]);
    if (!str_is_empty(a[6])) {
        e.note = malloc((strlen(a[6]) + 1) * sizeof(char));
        strcpy(e.note, a[6]);
    } else {
        e.note = NULL;
    }
    free(a[6]);

    e.status = str_to_AniEntryStatus(a[7]);
    free(a[7]);

    e.updated_this_cycle = true;

    e.last_updated = time(NULL);

    free(a);

    AniFile_push_AniEntry(af, e);

#undef Q
}
