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

AniEntry *AniFile_find_random_plan_to_watch_entry(AniFile *af) {
    if (af == NULL || af->size == 0)
        return NULL;

    size_t count = 0;
    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].status == PLAN_TO_WATCH ||
                af->entries[i].status == ON_HOLD) {

            count++;
        }
    }

    if (count == 0)
        return NULL;

    size_t target = rand() % count;

    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].status == PLAN_TO_WATCH ||
                af->entries[i].status == ON_HOLD) {
            if (target-- == 0)
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

int count_tag_occ(AniFile *af, const char *tag) {
    int count = 0;

    for (size_t i = 0; i < af->size; i++) {
        AniEntry *e = &af->entries[i];

        if (!e->tags)
            continue;

        for (size_t j = 0; e->tags[j] != NULL; j++) {
            if (strcmp(e->tags[j], tag) == 0) {
                count++;
            }
        }
    }

    return count;
}

const char *find_most_common_tag(AniFile *af) {
    const char *best_tag = NULL;
    int best_count = 0;

    for (size_t i = 0; i < af->size; i++) {
        AniEntry *e = &af->entries[i];

        if (!e->tags)
            continue;

        for (size_t j = 0; e->tags[j] != NULL; j++) {
            const char *tag = e->tags[j];

            bool seen_prev = false;

            for (size_t ii = 0; ii <= i && !seen_prev; ii++) {
                AniEntry *prev = &af->entries[ii];

                if (!prev->tags)
                    continue;

                size_t limit = (ii == i) ? j : (size_t)-1;

                for (size_t jj = 0; prev->tags[jj] != NULL && jj < limit; jj++) {
                    if (strcmp(prev->tags[jj], tag) == 0) {
                        seen_prev = true;
                        break;
                    }
                }
            }

            if (seen_prev)
                continue;

            int count = count_tag_occ(af, tag);

            if (count > best_count) {
                best_count = count;
                best_tag = tag;
            }
        }
    }

    return best_tag;
}

void AniFile_get_stats(AniFile *af, struct AniFileStats *a) {
#define M25_IN_SECS 1500
#define ENTRY af->entries[i]

    int score_total = 0;
    a->total_n_entries = af->size;
    int scored_entries = af->size - a->total_planned;

    for (size_t i = 0; i < af->size; i++) {
        a->combined_watch_time += ENTRY.ep_watched * M25_IN_SECS;
        a->total_n_ep_watched += ENTRY.ep_watched;
        if (ENTRY.status != PLAN_TO_WATCH) {
            score_total += ENTRY.score;
        }
        const char *fav = find_most_common_tag(af);
        a->fav_tag = fav ? strdup(fav) : NULL;
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

    a->average_score =
        scored_entries ? (double)score_total / scored_entries : 0.0;

#undef ENTRY

    return;
}
