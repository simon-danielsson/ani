#include "cli.h"
#include "../backend/backend.h"
#include "../utils.h"
#include "arg.h"

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
#define PRETTY_PRN_LINE                                                        \
    do {                                                                         \
        printf("\n%-7s", "┊");                                                     \
    } while (0)
    // row
    printf("%-5d", e->id);
    printf("%s%-79s%s", COL_SHOW_HEADER, e->name, ansi_clr(RESET));

    // row
    PRETTY_PRN_LINE;
    printf("%s%s%s %-30s", ansi_clr(BLUE), field_icon(STAT), ansi_clr(RESET),
            AniEntryStatus_to_str(e->status));
    {
        char tmp[64] = {0};
        snprintf(tmp, sizeof(tmp), "%d of %d", e->ep_watched, e->ep_total);
        printf("%s%s %s%-27s", ansi_clr(BLUE), field_icon(EP_TOT), ansi_clr(RESET),
                tmp);
    }
    printf("%s%s%s %d", ansi_clr(BLUE), field_icon(SCORE), ansi_clr(RESET),
            e->score);

    // row
    PRETTY_PRN_LINE;
    printf("%s%s%s", ansi_clr(BLUE), field_icon(TAGS), ansi_clr(RESET));
    if (e->tags != NULL) {
        char tmp[128] = {0};
        for (size_t j = 0; j < TAG_MAX_N; j++) {
            if (e->tags[j] != NULL) {
                strncat(tmp, " #", 2);
                strncat(tmp, e->tags[j], strlen(e->tags[j]) + 1);
            }
        }
        printf("%-31s", tmp);
    } else {
        printf(" %-30s", "(none)");
    }
    {
        char tmp[32] = {0};
        format_time_t_year(tmp, 32, &e->released, true);
        printf("%s%s%s %-27s", ansi_clr(BLUE), field_icon(YEAR), ansi_clr(RESET),
                tmp);
    }
    {
        char tmp[32] = {0};
        format_time_t_year(tmp, 32, &e->last_updated, false);
        printf("%s%s%s %s", ansi_clr(BLUE), field_icon(DATEUPD), ansi_clr(RESET),
                tmp);
    }
    if (e->note) {
        // row
        PRETTY_PRN_LINE;
        printf("%s%s%s %s", ansi_clr(BLUE), field_icon(NOTE), ansi_clr(RESET),
                e->note);
    }
    printf("\n");

#undef PRETTY_PRN_LINE
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

void cmd_list(AniFile *af, ArgType list_type, bool reverse_sort,
        ArgType sort_type) {

    AniEntry **entries = calloc(af->size, sizeof *entries);
    // AniEntry *entries[af->size];
    // memset(entries, 0, sizeof(entries));

    AniEntryStatus status;
    switch (list_type) {
        case C_LIST_WATCH:
            AniFile_collect_entries_with_certain_status(af, entries, WATCHING);
            status = WATCHING;
            break;
        case C_LIST_COMPL:
            AniFile_collect_entries_with_certain_status(af, entries, COMPLETED);
            status = COMPLETED;
            break;
        case C_LIST_ONHOL:
            AniFile_collect_entries_with_certain_status(af, entries, ON_HOLD);
            status = ON_HOLD;
            break;
        case C_LIST_DROPP:
            AniFile_collect_entries_with_certain_status(af, entries, DROPPED);
            status = DROPPED;
            break;
        case C_LIST_PLANN:
            AniFile_collect_entries_with_certain_status(af, entries, PLAN_TO_WATCH);
            status = PLAN_TO_WATCH;
            break;
        default:
            for (size_t i = 0; i < af->size; ++i) {
                entries[i] = &af->entries[i];
            }
            break;
    }

    if (!entries) {
        printf("No entries with could be found.");
        return;
    }
    if (sort_type) {
        switch (sort_type) {
            case F_SORT_NAME:
                qsort(entries, af->size, sizeof(entries[0]), AniEntry_qsort_by_name);
                break;
            case F_SORT_SCOR:
                qsort(entries, af->size, sizeof(entries[0]), AniEntry_qsort_by_score);
                break;
            case F_SORT_UPDA:
                qsort(entries, af->size, sizeof(entries[0]), AniEntry_qsort_by_updated);
                break;
            case F_SORT_RELE:
                qsort(entries, af->size, sizeof(entries[0]), AniEntry_qsort_by_released);
                break;
            case F_SORT_PROG:
                qsort(entries, af->size, sizeof(entries[0]), AniEntry_qsort_by_progress);
                break;
            default:
                break;
        }
    }

    if (reverse_sort) {
        AniEntry_reverse_array(entries, af->size);
    }

    for (size_t i = 0; i < af->size; i++) {
        if (entries[i])
            AniEntry_pretty_print(entries[i]);
    }

    free(entries);
}

void cmd_edit(AniFile *af, int id) {
    AniEntry *e = AniFile_find_entry_by_id(af, id);
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

void cmd_rm(AniFile *af, int id) {
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

void cmd_ep(AniFile *af, int id) {
    AniEntry *e = AniFile_find_entry_by_id(af, id);
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

void cmd_search(AniFile *af, char *search_term) {
    size_t count;
    int *ids = AniFile_search_for_entries(af, search_term, &count);

    if (!ids) {
        printf("Could not find anything matching '%s%s%s'!\n", ansi_clr(YELLOW),
                search_term, ansi_clr(YELLOW));
        return;
    }

    for (size_t i = 0; i < count; i++) {
        int id = ids[i];
        AniEntry *e = AniFile_find_entry_by_id(af, id);
        COULD_NOT_FIND_ENTRY_BY_ID;
        AniEntry_pretty_print(e);
    }
    free(ids);
}

void cmd_info(AniFile *af, int id) {
    AniEntry *e = AniFile_find_entry_by_id(af, id);
    COULD_NOT_FIND_ENTRY_BY_ID;
    AniEntry_pretty_print(e);
}

void cmd_rec(AniFile *af) {
    AniEntry *e = AniFile_find_random_plan_to_watch_entry(af);
    if (!e) {
        printf("Could not find any recommendation for you, sorry!\n");
    }
    AniEntry_pretty_print(e);
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

#define STATS_N_OF_STATUSES 5
#define STATS_STATSBAR_LEN 56
#define STATS_STATSBAR_C "█"

void bar_repeat(const char *c, int count, Color col) {
    for (int i = 0; i < count; i++) {
        printf("%s%s%s", ansi_clr(col), c, ansi_clr(RESET));
    }
}

void stats_print_statsbar(StatsBarField *fields) {

    int total = 0;
    for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
        total += fields[i].total;
    }
    for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
        fields[i].scaled_total =
            round(fields[i].total * ((double)STATS_STATSBAR_LEN / total));
    }
    for (int i = 0; i < STATS_N_OF_STATUSES; i++) {
        bar_repeat(STATS_STATSBAR_C, fields[i].scaled_total, fields[i].color);
    }
}

void cmd_stats(AniFile *af) {

    struct AniFileStats stats = {0};
    AniFile_get_stats(af, &stats);

    StatsBarField fields[STATS_N_OF_STATUSES] = {

        (StatsBarField){.color = GREEN,
            .scaled_total = 0,
            .total = stats.total_watching,
            .stat = WATCHING},

        (StatsBarField){.color = BLUE,
            .scaled_total = 0,
            .total = stats.total_completed,
            .stat = COMPLETED},

        (StatsBarField){.color = YELLOW,
            .scaled_total = 0,
            .total = stats.total_hold,
            .stat = ON_HOLD},

        (StatsBarField){.color = RED,
            .scaled_total = 0,
            .total = stats.total_dropped,
            .stat = DROPPED},

        (StatsBarField){.color = RESET,
            .scaled_total = 0,
            .total = stats.total_planned,
            .stat = PLAN_TO_WATCH},

    };
    stats_print_statsbar(fields);

    printf("\n");

    int row = 0;
    printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
            AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
            fields[row].total);

    printf("%-15s%d", "Ep. watched", stats.total_n_ep_watched);

    printf("\n");
    row++;

    printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
            AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
            fields[row].total);

    printf("%-15s%.1f", "Days", time_t_to_days(stats.combined_watch_time));

    printf("\n");
    row++;

    printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
            AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
            fields[row].total);

    printf("%-15s%d", "Tot. entries", stats.total_n_entries);

    printf("\n");
    row++;

    printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
            AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
            fields[row].total);

    printf("%-15s%.2f", "Avg. score", stats.average_score);

    printf("\n");
    row++;

    printf("%s%-15s%s %-15d", ansi_clr(fields[row].color),
            AniEntryStatus_to_str(fields[row].stat), ansi_clr(RESET),
            fields[row].total);

    printf("%-15s#%s", "Fav. tag", stats.fav_tag);

    printf("\n");
}
