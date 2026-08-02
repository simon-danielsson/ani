#include "../utils.h"
#include "backend.h"

char *field_icon(enum AniCurrentFieldState acfs) {
    char *icons_dev[10] = {"", "󰷝", "",  "", "",
        "", "",  "󰎛", "", "󰚰"};
    return icons_dev[acfs];
}

int AniEntry_qsort_by_progress(const void *a, const void *b) {
    const AniEntry *x = (const AniEntry *)a;
    const AniEntry *y = (const AniEntry *)b;

    if (!x || !y)
        return (x == y) ? 0 : (x ? -1 : 1);
    double x_progress = (double)x->ep_watched / (double)x->ep_total;
    double y_progress = (double)y->ep_watched / (double)y->ep_total;
    if (x_progress < y_progress)
        return -1;
    if (x_progress > y_progress)
        return 1;
    return 0;
}

int AniEntry_qsort_by_released(const void *a, const void *b) {
    const AniEntry *x = (const AniEntry *)a;
    const AniEntry *y = (const AniEntry *)b;

    if (!x || !y)
        return 0;

    if (x->released < y->released)
        return -1;
    if (x->released > y->released)
        return 1;
    return 0;
}

int AniEntry_qsort_by_updated(const void *a, const void *b) {
    const AniEntry *x = (const AniEntry *)a;
    const AniEntry *y = (const AniEntry *)b;

    if (!x || !y)
        return (x == y) ? 0 : (x ? -1 : 1);
    if (x->last_updated < y->last_updated)
        return -1;
    if (x->last_updated > y->last_updated)
        return 1;
    return 0;
}

int AniEntry_qsort_by_score(const void *a, const void *b) {
    const AniEntry *x = (const AniEntry *)a;
    const AniEntry *y = (const AniEntry *)b;

    if (!x || !y)
        return (x == y) ? 0 : (x ? -1 : 1);
    return x->score - y->score;
}

int AniEntry_qsort_by_name(const void *a, const void *b) {
    const AniEntry *x = (const AniEntry *)a;
    const AniEntry *y = (const AniEntry *)b;

    if (!x || !y)
        return (x == y) ? 0 : (x ? -1 : 1);
    return strcmp(x->name, y->name);
}

// void AniEntry_reverse_array(AniEntry **entries, size_t n) {
//     int l = 0, r = n - 1;
//     while (l < r) {
//         AniEntry *temp = entries[l];
//         entries[l] = entries[r];
//         entries[r] = temp;
//         l++;
//         r--;
//     }
// }
void AniEntry_reverse_array(AniEntry *entries, size_t n) {
    if (n <= 1)
        return;

    size_t l = 0, r = n - 1;
    while (l < r) {
        AniEntry temp = entries[l]; // Swap whole structs
        entries[l] = entries[r];
        entries[r] = temp;
        l++;
        r--;
    }
}

void AniFile_collect_entries_with_certain_status(AniFile *af, AniEntry *entries,
        int *found_count,
        AniEntryStatus s) {

    for (size_t i = 0; i < af->size; i++) {
        if (af->entries[i].status == s) {
            entries[*found_count] = af->entries[i];
            (*found_count)++;
        }
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

// returns an array of id's matching search term
void AniFile_search_for_entries(AniFile *af, AniEntry *entries_buff,
        int *found_count, const char *search_term) {

    if (!af || af->size == 0 || !search_term || !entries_buff)
        return;

    // lowercase copy of search term
    char *term = strdup(search_term);

    if (!term) {
        return;
    }

    str_to_lowercase(term, strlen(term) + 1);

    for (size_t i = 0; i < af->size; i++) {
        char buffer[2048] = {0};

        strcat(buffer, af->entries[i].name);

        if (af->entries[i].note) {
            strcat(buffer, " ");
            strcat(buffer, af->entries[i].note);
        }

        if (af->entries[i].tags) {
            for (size_t j = 0; af->entries[i].tags[j]; j++) {
                strcat(buffer, " ");
                strcat(buffer, af->entries[i].tags[j]);
            }
        }

        str_to_lowercase(buffer, strlen(buffer) + 1);

        if (strstr(buffer, term)) {
            entries_buff[*found_count] = af->entries[i];
            (*found_count)++;
        }
    }

    free(term);
}

bool AniEntry_is_probably_a_new_entry(const AniEntry *new_entry,
        const AniFile *af) {
    for (size_t i = 0; i < af->size; i++) {
        const AniEntry *old = &af->entries[i];

        int matches = 0;

        if (new_entry->status == old->status || new_entry->score == old->score ||
                new_entry->released == old->released ||
                new_entry->ep_total == old->ep_total ||
                new_entry->ep_watched == old->ep_watched) {

            matches++;
        }

        char name_copy[256];
        strncpy(name_copy, new_entry->name, sizeof(name_copy) - 1);
        name_copy[sizeof(name_copy) - 1] = '\0';

        size_t word_count = 0;
        size_t name_matches = 0;

        for (char *token = strtok(name_copy, " "); token != NULL;
                token = strtok(NULL, " ")) {

            word_count++;

            if (strcasestr(old->name, token) != NULL) {
                name_matches++;
            }
        }

        if (word_count >= 2 && name_matches * 5 >= word_count * 4) {
            matches++;
        }

        if (matches >= 5) {
            return false;
        }
    }

    return true;
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

int AniFile_get_stats_KV_find_key(const char *str_key, int kv_size,
        AniFileStats_KV *kv) {
    for (int i = 0; i < kv_size; i++) {
        if (strcmp(kv[i].key, str_key) == 0)
            return i;
    }
    return -1;
}

void AniFile_get_stats_KV_add_key(const char *key, size_t *kv_size,
        AniFileStats_KV *kv) {
    if (*kv_size >= AniFileStats_KV_MAX_VAL) {
        panic("kv array capacity exceeded");
    }
    AniFileStats_KV *entry = &kv[*kv_size];

    strncpy(entry->key, key, AniFileStats_KV_MAX_KEY_LEN - 1);
    entry->key[AniFileStats_KV_MAX_KEY_LEN - 1] = '\0';
    entry->value = 0;

    (*kv_size)++;
}

void AniFile_get_stats_KV_count(char *key, size_t *kv_size,
        AniFileStats_KV *kv) {
    int idx = AniFile_get_stats_KV_find_key(key, *kv_size, kv);
    if (idx == -1) {
        AniFile_get_stats_KV_add_key(key, kv_size, kv);
        idx = *kv_size - 1;
    }
    kv[idx].value++;
}

#define M25_IN_SECS 1500
#define ENTRY af->entries[i]
void AniFile_get_stats(AniFile *af, AniFileStats *a) {
    a->entries_n = af->size;
    a->last_update = 0;

    int score_total = 0;
    int scored_entries = af->size - a->plan_to_watch_n;

    for (size_t i = 0; i < af->size; i++) {
        if (ENTRY.tags) {
            for (size_t j = 0; ENTRY.tags[j] != NULL; j++) {
                AniFile_get_stats_KV_count(ENTRY.tags[j], &a->tags_kv_size, a->tags_kv);
            }
        }

        a->combined_watch_time += ENTRY.ep_watched * M25_IN_SECS;
        a->ep_watched_n += ENTRY.ep_watched;
        if (ENTRY.last_updated > a->last_update) {
            a->last_update = ENTRY.last_updated;
        }
        if (ENTRY.status != PLAN_TO_WATCH) {
            score_total += ENTRY.score;
        }
        switch (ENTRY.status) {
            case COMPLETED:
                a->completed_n++;
                break;
            case DROPPED:
                a->dropped_n++;
                break;
            case WATCHING:
                a->watching_n++;
                break;
            case ON_HOLD:
                a->on_hold_n++;
                break;
            case PLAN_TO_WATCH:
                a->plan_to_watch_n++;
                break;
        }
    }

    assert(a->completed_n + a->dropped_n + a->on_hold_n + a->plan_to_watch_n +
            a->watching_n ==
            a->entries_n);

    a->average_score =
        scored_entries ? (double)score_total / scored_entries : 0.0;

    int biggest_value_yet = 0;
    for (size_t i = 0; i < a->tags_kv_size; i++) {
        if (a->tags_kv[i].value > biggest_value_yet) {
            biggest_value_yet = a->tags_kv[i].value;
            a->tag_fav = a->tags_kv[i].key;
        }
    }
}

char *get_set_ani_path(FILE *f) {
    char *content = read_entire_file(f);
    if (!content) {
        return NULL;
    }

    char *path = malloc(257);
    if (!path) {
        free(content);
        return NULL;
    }

    int i = 0;
    while (i < 256 && content[i] && content[i] != '\n') {
        path[i] = content[i];
        i++;
    }

    path[i] = '\0';

    trim_str(path);

    char *expanded = expand_home_path(path);

    free(path);
    free(content);

    return expanded;
    return path;
}
