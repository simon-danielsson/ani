#include "api.h"
#include "db_format.h"

char *field_icon(enum AniCurrentFieldState acfs, bool devicon) {
    char *icons_dev[10] = {"", "󰷝", "",  "", "",
        "", "",  "󰎛", "", "󰚰"};
    char *icons_tty[10] = {"I", "N", "Y", "T", "R", "E", "E", "N", "S", "U"};

    if (devicon) {
        char *tmp = icons_dev[acfs];
        return tmp;
    } else {
        char *tmp = icons_tty[acfs];
        return tmp;
    }
}

AniEntry *AniFile_find_entry_by_id(AniFile *af, int id) {
    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].id == id) {
            return &af->entries[i];
        }
    }
    return NULL;
}

bool AniFile_remove_entry_by_id(AniFile *af, int id) {
    if (af == NULL) {
        return false;
    }

    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].id == id) {

            free(af->entries[i].name);

            if (af->entries[i].tags != NULL) {
                for (size_t j = 0; j < TAG_MAX_N; j++) {
                    free(af->entries[i].tags[j]);
                }
                free(af->entries[i].tags);
            }

            free(af->entries[i].note);

            // shift entries left
            for (size_t j = i + 1; j < af->size; j++) {
                af->entries[j - 1] = af->entries[j];
            }

            af->size--;
            return true;
        }
    }

    return false;
}

int AniFile_get_most_recent_id(AniFile *af) {
    int most_recent = 0;
    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].id > most_recent) {
            most_recent = af->entries[i].id;
        }
    }
    return most_recent;
}

bool AniFile_has_changed(const AniFile *snapshot, const AniFile *current) {
    for (size_t i = 0; i < current->size; i++) {
        if (current->entries[i].last_updated) {
            return true;
        }
    }
    return memcmp(snapshot, current, sizeof *current) != 0;
}

AniEntryStatus str_to_AniEntryStatus(const char *s) {
    switch (s[0]) {
        case 'w' | 'W':
            return WATCHING;
        case 'c' | 'C':
            return COMPLETED;
        case 'o' | 'O':
            return ON_HOLD;
        case 'd' | 'D':
            return DROPPED;
        default:
            return PLAN_TO_WATCH;
    }
}

char *AniEntryStatus_to_str(AniEntryStatus aes) {
    char *tmp;
    char *status[5] = {"Watching", "Completed", "On hold", "Dropped",
        "Plan to watch"};
    return status[aes];
}

char *anifield_enum_to_str(enum AniCurrentFieldState a) {
    char *field_name;
    switch (a) {
        case ID:
            field_name = "Id";
            break;
        case NAME:
            field_name = "Name";
            break;
        case TAGS:
            field_name = "Tags";
            break;
        case YEAR:
            field_name = "Year";
            break;
        case SCORE:
            field_name = "Score";
            break;
        case EP_TOT:
            field_name = "Ep total";
            break;
        case EP_WAT:
            field_name = "Ep watched";
            break;
        case NOTE:
            field_name = "Note";
            break;
        case STAT:
            field_name = "Status";
            break;
        default:
            field_name = "Last updated";
            break;
    }
    return field_name;
}

void AniFile_get_stats(AniFile *af, struct AniFileStats *a) {

#define M25_IN_SECS 1500
#define ENTRY af->entries[i]

    int score_total = 0;

    a->total_n_entries = af->size;

    for (size_t i = 0; i < af->size; i++) {
        a->combined_watch_time += ENTRY.ep_watched * M25_IN_SECS;
        a->total_n_ep_watched += ENTRY.ep_watched;
        if (ENTRY.status != PLAN_TO_WATCH) {
            score_total += ENTRY.score;
        }
        switch (ENTRY.status) {
            case COMPLETED:
                a->total_completed++;
                break;
            case DROPPED:
                a->total_dropped++;
                break;
            case WATCHING:
                a->total_watching++;
                break;
            case ON_HOLD:
                a->total_hold++;
                break;
            case PLAN_TO_WATCH:
                a->total_planned++;
                break;
        }
    }

    assert(a->total_completed + a->total_dropped + a->total_hold +
            a->total_planned + a->total_watching ==
            a->total_n_entries);

    a->average_score = (double)score_total / af->size;

#undef ENTRY

    return;
}
