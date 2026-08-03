#include "../backend/backend.h"
#include "../static/guide.h"
#include "../static/help.h"
#include "../utils.h"
#include "frontend.h"
#include <stdio.h>

#define HEADER_MAX_WIDTH 44
#define WIDTH 51
#define COL_WIDTH WIDTH / 2 - 3
#define COL_SHOW_HEADER "\033[4;1m"

char *ansi_clr(Color c) {
    switch (c) {
        case MAGENTA:
            return "\033[35m";
            break;
        case YELLOW:
            return "\033[33m";
            break;
        case GREEN:
            return "\033[32m";
            break;
        case RED:
            return "\033[31m";
            break;
        case BLUE:
            return "\033[34m";
            break;
        default:
            return "\033[0m";
    }
    return "\033[0m";
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
        [STAT] = "[w]atching, [c]ompleted, [o]n hold, [d]ropped, [p]lan to watch",
    };
    return args[at];
}

void AniEntry_pretty_print(AniEntry *e) {
    /*
       TODO: the color
       */

    char *col_field = ansi_clr(BLUE);
    char *col_tags_note = ansi_clr(YELLOW);
    char *col_reset = ansi_clr(RESET);

    // header
    {
        size_t header_len = strlen(e->name);

        int padding;
        if (header_len > HEADER_MAX_WIDTH) {
            padding = (WIDTH / 2) - (HEADER_MAX_WIDTH / 2);
        } else {
            padding = (WIDTH / 2) - (header_len / 2);
        }
        while (padding > 0) {
            printf(" ");
            padding--;
        }

        for (size_t i = 0; i < header_len; i++) {
            if (i > HEADER_MAX_WIDTH - 2) {
                printf("…");
                break;
            }
            printf("%s%c%s", COL_SHOW_HEADER, e->name[i], col_reset);
        }
        printf("\n");
    }

    printf("┌────────────────────────┬────────────────────────┐\n");

    // id
    {
        size_t len = 0;
        printf("│Id");

        size_t id_len = char_len_of_int(e->id);
        size_t padding = COL_WIDTH - len - id_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, e->id, col_reset);
    }

    printf("│");

    // score
    {
        size_t len = 0;
        printf("Score");

        size_t scr_len = 1;
        if (e->status != PLAN_TO_WATCH) {
            scr_len = char_len_of_int(e->score);
        }

        size_t padding = COL_WIDTH - len - scr_len - 3;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s", col_field);
        if (e->status != PLAN_TO_WATCH) {
            printf("%d", e->score);
        } else {
            printf("-");
        }
        printf("%s", col_reset);
    }

    printf("│\n");

    // status
    {
        printf("│Status");

        char *status = AniEntryStatus_to_str(e->status);
        size_t status_len = strlen(status);

        size_t padding = COL_WIDTH - status_len - 4;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%s%s", col_field, status, col_reset);
    }
    printf("│");

    // released
    {
        printf("Released");

        char tmp[64] = {0};
        format_time_t_year(tmp, 64, &e->released, true);

        size_t len = strlen(tmp);

        size_t padding = COL_WIDTH - len - 6;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%s%s", col_field, tmp, col_reset);
    }

    printf("│\n");

    // progress
    {
        printf("│Progress");

        size_t ep_w_len = 1;
        if (e->status != PLAN_TO_WATCH) {
            ep_w_len = char_len_of_int(e->ep_watched);
        }

        size_t ep_t_len = char_len_of_int(e->ep_total);
        size_t padding = COL_WIDTH - 7 - ep_t_len - ep_w_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }

        printf("%s", col_field);
        if (e->status != PLAN_TO_WATCH) {
            printf("%d/%d", e->ep_watched, e->ep_total);
        } else {
            printf("-/%d", e->ep_total);
        }
        printf("%s", col_reset);
    }
    printf("│");

    // released
    {
        printf("Updated");

        char tmp[64] = {0};
        format_time_t_year(tmp, 64, &e->last_updated, false);

        size_t len = strlen(tmp);

        size_t padding = COL_WIDTH - len - 5;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%s%s", col_field, tmp, col_reset);
    }
    printf("│\n");

    printf("├────────────────────────┴────────────────────────┤\n");

    printf("│");
    // tags
    if (e->tags) {
        int i = 0;
        size_t tags_len = 0;
        char tags_str[2048] = {0};
        while (e->tags[i]) {
            tags_len += strlen(e->tags[i]) + 1;
            char tmp[256] = {0};
            if (i > 0) {
                snprintf(tmp, 256, " #%s%s%s", col_tags_note, e->tags[i], col_reset);
            } else {
                snprintf(tmp, 256, "#%s%s%s", col_tags_note, e->tags[i], col_reset);
            }
            strcat(tags_str, tmp);
            tags_len += 1;
            i++;
        }
        tags_len += 1;

        size_t start_pad = (WIDTH / 2) - (tags_len / 2);
        size_t end_pad = WIDTH - start_pad - tags_len;
        while (start_pad > 0) {
            printf(" ");
            start_pad--;
        }
        printf("%s", tags_str);
        while (end_pad > 0) {
            printf(" ");
            end_pad--;
        }
    }
    printf("│\n");

    // note
    if (e->note) {
        printf("│");
        size_t len = strlen(e->note);

        size_t start_pad = (WIDTH / 2) - (len / 2);
        size_t end_pad = WIDTH - start_pad - len - 2;
        while (start_pad > 0) {
            printf(" ");
            start_pad--;
        }
        printf("%s%s%s", col_tags_note, e->note, col_reset);
        while (end_pad > 0) {
            printf(" ");
            end_pad--;
        }
        printf("│\n");
    }

    printf("└─────────────────────────────────────────────────┘\n");
    printf("\n");
}

void AniEntry_prompt_edit_print(AniEntry *e) {
    char released_tmp[32] = {0};
    format_time_t_year(released_tmp, 32, &e->released, true);

    printf("%-24s%s\n", "1 Name", e->name);
    printf("%-24s%s\n", "2 Release year", released_tmp);
    printf("%-24s%s\n", "3 Status", AniEntryStatus_to_str(e->status));
    printf("%-24s%d\n", "4 Episodes total", e->ep_total);
    printf("%-24s%d\n", "5 Episodes watched", e->ep_watched);
    printf("%-24s%d\n", "6 Score", e->score);

    printf("%-24s", "7 Note");
    if (e->note != NULL) {
        printf("%s\n", e->note);
    } else {
        printf("(none)\n");
    }

    printf("%-24s", "8 Tags");
    if (e->tags != NULL) {
        for (size_t j = 0; j < TAG_MAX_N; j++) {
            if (e->tags[j] != NULL) {
                printf("#%s ", e->tags[j]);
            }
        }
    } else {
        printf("(none)");
    }
    printf("\n\n");
}

// returns false if the user wants to keep editing
bool prompt_edit(AniEntry *e) {
    AniEntry_prompt_edit_print(e);

    printf("Enter number corresponding to field:\n");
    printf(PROMPT);
    int r = 0;
    {
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
    }

    switch (r) {
        case 1:
            printf("Enter new name:\n");
            printf(PROMPT);
            char tmp[128] = {0};
            int c_count = 0;
            for (int ch; (ch = getchar()) != EOF;) {
                if (ch == '\n') {
                    tmp[c_count] = '\0';
                    trim_str(tmp);
                    char *new_name = strdup(tmp);
                    if (!new_name) {
                        perror("strdup");
                        return false;
                    }
                    free(e->name);
                    e->name = new_name;
                    printf("%sName was updated to '%s'%s\n", ansi_clr(YELLOW), e->name,
                            ansi_clr(RESET));
                    e->last_updated = time(NULL);
                    break;
                }
                tmp[c_count] = ch;
                c_count++;
            }
            break;

        case 2:
            printf("Enter new release year:\n");
            printf(PROMPT);
            {
                int r = 0;
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        trim_str(tmp);
                        e->released = time_t_from_iso_ymd(tmp);
                        printf("%sYear was updated to '%s'%s\n", ansi_clr(YELLOW), tmp,
                                ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }
            break;

        case 3:
            printf("%s\n", prompt_field(STAT));
            printf("Enter character corresponding to status:\n");
            printf(PROMPT);
            {
                int r = 0;
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        trim_str(tmp);
                        e->status = str_to_AniEntryStatus(tmp);
                        if (e->status == COMPLETED) {
                            e->ep_watched = e->ep_total;
                        }
                        printf("%sStatus was updated to '%s'%s\n", ansi_clr(YELLOW),
                                AniEntryStatus_to_str(e->status), ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }
            break;

        case 4:
            printf("Enter new total episodes:\n");
            printf(PROMPT);
            {
                int r = 0;
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        trim_str(tmp);
                        e->ep_total = atoi(tmp);
                        printf("%sTotal episodes was updated to '%s'%s\n", ansi_clr(YELLOW),
                                tmp, ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }

            break;

        case 5:
            printf("Enter new episodes watched:\n");
            printf(PROMPT);
            {
                int r = 0;
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        trim_str(tmp);
                        int new_ep = atoi(tmp);
                        if (new_ep >= e->ep_total) {
                            e->ep_watched = e->ep_total;
                            e->status = COMPLETED;
                        } else if (new_ep <= 0) {
                            e->ep_watched = 0;
                        } else {
                            e->ep_watched = new_ep;
                        }
                        printf("%sEpisodes watched was updated to '%d'%s\n", ansi_clr(YELLOW),
                                e->ep_watched, ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }
            break;

        case 6:
            printf("Enter new score (1-10):\n");
            printf(PROMPT);
            {
                int r = 0;
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        trim_str(tmp);
                        e->score = atoi(tmp);
                        printf("%sScore was updated to '%d'%s\n", ansi_clr(YELLOW), e->score,
                                ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }
            break;

        case 7:
            printf("Enter new note:\n");
            printf(PROMPT);
            {
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        tmp[c_count] = '\0';
                        trim_str(tmp);
                        char *new_name = strdup(tmp);
                        if (str_is_empty(tmp)) {
                            new_name = strdup(" ");
                        } else {
                            new_name = strdup(tmp);
                        }
                        if (!new_name) {
                            perror("strdup");
                            return false;
                        }
                        free(e->note);
                        e->note = new_name;
                        printf("%sNote was updated to '%s'%s\n", ansi_clr(YELLOW), e->note,
                                ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }
            break;

        default:
            printf("Enter new tags (separated by ','):\n");
            printf(PROMPT);
            {
                char tmp[128] = {0};
                int c_count = 0;
                for (int ch; (ch = getchar()) != EOF;) {
                    if (ch == '\n') {
                        tmp[c_count] = '\0';
                        trim_str(tmp);

                        if (!str_is_empty(tmp)) {
                            char *tags_raw = strdup(tmp);
                            free(e->tags);
                            e->tags = get_tags_from_field(tags_raw);
                        } else {
                            e->tags = NULL;
                        }
                        printf("%sTags were updated to '", ansi_clr(YELLOW));
                        if (!str_is_empty(tmp)) {
                            for (size_t j = 0; j < TAG_MAX_N; j++) {
                                if (e->tags[j] != NULL) {
                                    printf("#%s ", e->tags[j]);
                                }
                            }
                        }
                        printf("'%s\n", ansi_clr(RESET));
                        e->last_updated = time(NULL);
                        break;
                    }
                    tmp[c_count] = ch;
                    c_count++;
                }
            }

            break;
    }

    printf("\nDo you want to keep editing?\n");
    printf("(y) Keep editing!\n");
    printf("(n) Save and exit\n");
    printf("(Ctrl-C) Cancel changes and exit\n");
    printf(PROMPT);
    {
        char tmp[32] = {0};
        int j = 0;
        for (int ch; (ch = getchar()) != EOF;) {
            if (ch == '\n' || ch == '\r') {
                break;
            }
            tmp[j] = ch;
            j++;
        }
        if (strcmp(tmp, "y")) {
            return true;
        }
        if (strcmp(tmp, "n")) {
            return false;
        }
    }
    return false;
}

void cmd_list(AniFile *af, PrgVars *pv) {
    AniEntry *entries = calloc(af->size, sizeof *entries);
    int found_count = 0;

    if (pv->params[0] != NULL) {
        switch (pv->params[0][0]) {
            case 'w':
                AniFile_collect_entries_with_certain_status(af, entries, &found_count,
                        WATCHING);
                break;
            case 'c':
                AniFile_collect_entries_with_certain_status(af, entries, &found_count,
                        COMPLETED);
                break;
            case 'o':
                AniFile_collect_entries_with_certain_status(af, entries, &found_count,
                        ON_HOLD);
                break;
            case 'd':
                AniFile_collect_entries_with_certain_status(af, entries, &found_count,
                        DROPPED);
                break;
            case 'p':
                AniFile_collect_entries_with_certain_status(af, entries, &found_count,
                        PLAN_TO_WATCH);
                break;
        }
    } else {
        found_count = (int)af->size;
        for (int i = 0; i < found_count; ++i) {
            entries[i] = af->entries[i];
        }
    }

    if (!entries) {
        printf("No entries with could be found.");
        return;
    }

    bool reverse = false;
    if (pv->sort_flags_count != 0) {
        for (size_t i = 0; i < pv->sort_flags_count; i++) {
            if (*pv->sort_flags[i] == (ArgType)SF_REVERSE) {
                reverse = true;
            }

            switch (*pv->sort_flags[i]) {
                case (ArgType)SF_NAME:
                    qsort(entries, found_count, sizeof(entries[0]), AniEntry_qsort_by_name);
                    break;
                case (ArgType)SF_SCORE:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_score);
                    break;
                case (ArgType)SF_UPDATED:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_updated);
                    break;
                case (ArgType)SF_RELEASED:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_released);
                    break;
                case (ArgType)SF_PROGRESS:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_progress);
                    break;
                default:
                    break;
            }
        }
    }

    if (reverse) {
        AniEntry_reverse_array(entries, found_count);
    }

    for (int i = 0; i < found_count; i++) {
        if (&entries[i])
            AniEntry_pretty_print(&entries[i]);
    }

    free(entries);
}

void cmd_edit(AniFile *af, PrgVars *pv) {
    AniEntry *e = AniFile_find_entry_by_id(af, atoi(pv->params[0]));
    COULD_NOT_FIND_ENTRY_BY_ID;
    int prompt_should_quit = false;
    while (!prompt_should_quit) {
        prompt_should_quit = prompt_edit(e);
    }
}

bool prompt_confirm(void) {
    printf(PROMPT);
    bool yes = false;
    char tmp[128] = {0};
    int c_count = 0;
    for (int ch; (ch = getchar()) != EOF;) {
        if (ch == '\n') {
            if (tmp[0] == 'y' || tmp[0] == 'Y') {
                yes = true;
            }
            break;
        }
        tmp[c_count] = ch;
        c_count++;
    }
    return yes;
}

int prompt_ep(char *name, int watched_episodes, int total_episodes) {
    printf("'%s'\n", name);
    printf("Progress: %d of %d\n", watched_episodes, total_episodes);
    printf(PROMPT);
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
        printf(PROMPT);
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

void cmd_rm(AniFile *af, PrgVars *pv) {
    int id = atoi(pv->params[0]);
    AniEntry *e = AniFile_find_entry_by_id(af, id);
    COULD_NOT_FIND_ENTRY_BY_ID;

    char name[128] = {0};
    memcpy(name, e->name, strlen(e->name));

    bool success = AniFile_remove_entry_by_id(af, id);
    if (success) {
        printf("Entry '%s%s%s' with id '%s%d%s' was successfully removed.\n",
                ansi_clr(YELLOW), name, ansi_clr(RESET), ansi_clr(YELLOW), id,
                ansi_clr(RESET));
    } else {
        printf("Error: removal of entry failed -- %s", MORE_INFO);
    }
}

void cmd_ep(AniFile *af, PrgVars *pv) {
    AniEntry *e = AniFile_find_entry_by_id(af, atoi(pv->params[0]));
    COULD_NOT_FIND_ENTRY_BY_ID;

    int new_ep = prompt_ep(e->name, e->ep_watched, e->ep_total);

    if (new_ep >= e->ep_total) {
        e->ep_watched = e->ep_total;
        e->status = COMPLETED;
    } else if (new_ep < e->ep_total && e->status == COMPLETED) {
        e->ep_watched = new_ep;
        e->status = WATCHING;
    } else if (new_ep <= 0) {
        e->ep_watched = 0;
    } else {
        e->ep_watched = new_ep;
    }

    e->last_updated = time(NULL);

    printf("Updated: %d of %d\n", e->ep_watched, e->ep_total);
}

void cmd_search(AniFile *af, PrgVars *pv) {
    if (!pv->params[0]) {
        error("no search argument was provided");
        return;
    }
    AniEntry *entries = calloc(af->size, sizeof(*entries));
    int found_count = 0;
    AniFile_search_for_entries(af, entries, &found_count, pv->params[0]);

    if (found_count == 0) {
        printf("Could not find anything matching '%s%s%s'!\n", ansi_clr(YELLOW),
                pv->params[0], ansi_clr(YELLOW));
        free(entries);
        return;
    }

    bool reverse = false;
    if (pv->sort_flags_count != 0) {
        for (size_t i = 0; i < pv->sort_flags_count; i++) {
            if (*pv->sort_flags[i] == (ArgType)SF_REVERSE) {
                reverse = true;
            }

            switch (*pv->sort_flags[i]) {
                case (ArgType)SF_NAME:
                    qsort(entries, found_count, sizeof(entries[0]), AniEntry_qsort_by_name);
                    break;
                case (ArgType)SF_SCORE:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_score);
                    break;
                case (ArgType)SF_UPDATED:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_updated);
                    break;
                case (ArgType)SF_RELEASED:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_released);
                    break;
                case (ArgType)SF_PROGRESS:
                    qsort(entries, found_count, sizeof(entries[0]),
                            AniEntry_qsort_by_progress);
                    break;
                default:
                    break;
            }
        }
    }

    if (reverse) {
        AniEntry_reverse_array(entries, found_count);
    }

    for (int i = 0; i < found_count; i++) {
        if (&entries[i] != NULL)
            AniEntry_pretty_print(&entries[i]);
    }

    free(entries);
}

void cmd_info(AniFile *af, PrgVars *pv) {
    AniEntry *e = AniFile_find_entry_by_id(af, atoi(pv->params[0]));
    COULD_NOT_FIND_ENTRY_BY_ID;
    AniEntry_pretty_print(e);
}

void cmd_rec(AniFile *af, PrgVars *_) {
    AniEntry *e = AniFile_find_random_plan_to_watch_entry(af);
    if (!e) {
        printf("Could not find any recommendation for you!\n");
        return;
    }
    AniEntry_pretty_print(e);
}

void cmd_add(AniFile *af, PrgVars *_) {
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

    bool probably_new = AniEntry_is_probably_a_new_entry(&e, af);

    if (probably_new) {
        AniFile_push_AniEntry(af, e);
        printf("'%s%s%s' was successfully added with id '%s%d%s'\n",
                ansi_clr(YELLOW), e.name, ansi_clr(RESET), ansi_clr(YELLOW), e.id,
                ansi_clr(RESET));
    } else {
        printf("'%s%s%s' might already exist in your library.\n", ansi_clr(YELLOW),
                e.name, ansi_clr(RESET));
        printf("Do you want to add it anyway? (y/n)\n");
        bool yes = prompt_confirm();
        if (yes) {
            AniFile_push_AniEntry(af, e);
            printf("'%s%s%s' was successfully added with id '%s%d%s'\n",
                    ansi_clr(YELLOW), e.name, ansi_clr(RESET), ansi_clr(YELLOW), e.id,
                    ansi_clr(RESET));
        }
    }
#undef Q
}

// #define STATS_N_OF_STATUSES 5
// #define STATS_STATSBAR_LEN 56
// #define STATS_STATSBAR_C "█"
//
// void bar_repeat(const char *c, int count, Color col) {
//     for (int i = 0; i < count; i++) {
//         printf("%s%s%s", ansi_clr(col), c, ansi_clr(RESET));
//     }
// }
//
// void stats_print_statsbar(StatsBarField *fields) {
//
//     int total = 0;
//     for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
//         total += fields[i].total;
//     }
//     for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
//         fields[i].scaled_total =
//             round(fields[i].total * ((double)STATS_STATSBAR_LEN / total));
//     }
//     for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
//         bar_repeat(STATS_STATSBAR_C, fields[i].scaled_total,
//         fields[i].color);
//     }
// }

void cmd_stats(AniFile *af, PrgVars *pv) {

    AniFileStats stats = {0};
    AniFile_get_stats(af, &stats);

    char *col_field = ansi_clr(BLUE);
    char *col_lib = ansi_clr(YELLOW);
    char *col_reset = ansi_clr(RESET);

    // library location
    {
        // TODO: this can be consolidated
        char tmp[256] = {0};
        get_anifile_path(pv->filepath, tmp, 256);
        char *path = expand_home_path(tmp);
        printf("Library: %s%s%s\n", col_lib, path, col_reset);
        free(path);
    }

    char *div = "┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈\n";
    char *header = "┌──────Entries──────┐┌────────────Info────────────┐\n";
    printf("%s%s", div, header);

#define ENTRIES_SECTION_W 21

    // total
    {
        size_t len = 0;
        printf("│Total");
        len += 7;

        size_t field_len = char_len_of_int(stats.entries_n);
        size_t padding = ENTRIES_SECTION_W - len - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.entries_n, col_reset);
        printf("│");
    }

    {
        printf("│Ep. watched");
        size_t ep_w_len = char_len_of_int(stats.ep_watched_n);
        size_t ep_t_len = char_len_of_int(stats.ep_total_n);
        size_t padding = COL_WIDTH - 6 - ep_t_len - ep_w_len;
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d/%d%s", col_field, stats.ep_watched_n, stats.ep_total_n,
                col_reset);
    }
    printf("│\n");

    {
        printf("│Watching");

        size_t field_len = char_len_of_int(stats.watching_n);
        size_t padding = ENTRIES_SECTION_W - 10 - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.watching_n, col_reset);
        printf("│");
    }

    {
        printf("│Days watched");

        float days = time_t_to_days(stats.combined_watch_time);
        size_t days_len = char_len_of_float(days, 1);
        size_t padding = COL_WIDTH - 6 - days_len;
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%.1f%s", col_field, days, col_reset);
    }
    printf("│\n");

    {
        printf("│Completed");

        size_t field_len = char_len_of_int(stats.completed_n);
        size_t padding = ENTRIES_SECTION_W - 11 - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.completed_n, col_reset);
        printf("│");
    }

    {
        printf("│Avg. score");
        size_t days_len = char_len_of_float(stats.average_score, 2);
        size_t padding = COL_WIDTH - 4 - days_len;
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%.2f%s", col_field, stats.average_score, col_reset);
    }
    printf("│\n");

    {
        printf("│On hold");

        size_t field_len = char_len_of_int(stats.on_hold_n);
        size_t padding = ENTRIES_SECTION_W - 9 - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.on_hold_n, col_reset);
        printf("│");
    }

    {
        printf("│Fav. tag");
        size_t len = strlen(stats.tag_fav);
        size_t padding = COL_WIDTH - 3 - len;
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s#%s%s", col_field, stats.tag_fav, col_reset);
    }
    printf("│\n");

    {
        printf("│Dropped");

        size_t field_len = char_len_of_int(stats.dropped_n);
        size_t padding = ENTRIES_SECTION_W - 9 - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.dropped_n, col_reset);
        printf("│");
    }

    {
        printf("│Unique tags");
        size_t days_len = char_len_of_float(stats.tags_kv_size, 1);
        size_t padding = COL_WIDTH - 3 - days_len;
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, (int)stats.tags_kv_size, col_reset);
    }
    printf("│\n");

    {
        printf("│Plan to watch");

        size_t field_len = char_len_of_int(stats.plan_to_watch_n);
        size_t padding = ENTRIES_SECTION_W - 15 - field_len;

        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%d%s", col_field, stats.plan_to_watch_n, col_reset);
        printf("│");
    }

    {
        printf("│Last updated");
        char tmp[256] = {0};
        format_time_t_year(tmp, 256, &stats.last_update, false);
        size_t padding = COL_WIDTH - 6 - strlen(tmp);
        while (padding > 0) {
            printf(" ");
            padding--;
        }
        printf("%s%s%s", col_field, tmp, col_reset);
    }
    printf("│\n");
    char *footer = "└───────────────────┘└────────────────────────────┘\n";
    printf("%s\n", footer);

    // StatsBarField fields[STATS_N_OF_STATUSES] = {
    //     (StatsBarField){.color = GREEN,
    //         .scaled_total = 0,
    //         .total = stats.watching_n,
    //         .stat = WATCHING},
    //
    //     (StatsBarField){.color = BLUE,
    //         .scaled_total = 0,
    //         .total = stats.completed_n,
    //         .stat = COMPLETED},
    //
    //     (StatsBarField){.color = YELLOW,
    //         .scaled_total = 0,
    //         .total = stats.on_hold_n,
    //         .stat = ON_HOLD},
    //
    //     (StatsBarField){.color = RED,
    //         .scaled_total = 0,
    //         .total = stats.dropped_n,
    //         .stat = DROPPED},
    //
    //     (StatsBarField){.color = RESET,
    //         .scaled_total = 0,
    //         .total = stats.plan_to_watch_n,
    //         .stat = PLAN_TO_WATCH},
    // };
    //
    // printf("\n");
    //
    // int row = 0;
    // printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
    //         AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
    //         fields[row].total);
    //
    // printf("%-15s%d", "Ep. watched", stats.ep_watched_n);
    //
    // printf("\n");
    // row++;
    //
    // printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
    //         AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
    //         fields[row].total);
    //
    // printf("%-15s%.1f", "Days", time_t_to_days(stats.combined_watch_time));
    //
    // printf("\n");
    // row++;
    //
    // printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
    //         AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
    //         fields[row].total);
    //
    // printf("%-15s%d", "Tot. entries", stats.entries_n);
    //
    // printf("\n");
    // row++;
    //
    // printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
    //         AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
    //         fields[row].total);
    //
    // printf("%-15s%.2f", "Avg. score", stats.average_score);
    //
    // printf("\n");
    // row++;
    //
    // printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
    //         AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
    //         fields[row].total);
    //
    // printf("%-15s#%s", "Fav. tag", stats.tag_fav);
    //
    // printf("\n");
}

void flag_guide(AniFile *af, PrgVars *pv) {
    for (size_t i = 0; i < guide_txt_len; i++) {
        printf("%c", guide_txt[i]);
    }
}

void flag_help(AniFile *af, PrgVars *pv) {
    for (size_t i = 0; i < help_txt_len; i++) {
        printf("%c", help_txt[i]);
    }
}

void flag_version(AniFile *af, PrgVars *pv) {
    printf("========================================\n");
    printf("%s %s (%.8s)\n", ENV_NAME, ENV_GITTAG, ENV_GITHASH);
    printf("Anime progress tracker for the CLI.\n");
    printf("%s\n", ENV_REPO);
    printf("----------------------------------------\n");
    printf("© 2026 %s - MIT License\n", ENV_AUTHOR);
    printf("Contact: %s\n", ENV_CONTACT);
    printf("========================================\n");
    exit(EXIT_SUCCESS);
}
