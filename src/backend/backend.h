#ifndef BACKEND_H
#define BACKEND_H

#include "../main.h"

#define TAG_TMP_BUFF 96
#define TAG_MAX_N 4

typedef enum {
  WATCHING = 0,
  COMPLETED = 1,
  ON_HOLD = 2,
  DROPPED = 3,
  PLAN_TO_WATCH = 4
} AniEntryStatus;

enum AniCurrentFieldState {
  ID,
  NAME,
  YEAR,
  TAGS,
  SCORE,
  EP_TOT,
  EP_WAT,
  NOTE,
  STAT,
  DATEUPD,
  _FIELDS_N
};

typedef struct AniEntry {
  char *name;
  char *note;  // NOTE: can also be NULL
  char **tags; // NOTE: can also be NULL
  int id;
  int score;
  int ep_total;
  int ep_watched;
  AniEntryStatus status;
  time_t released;
  time_t last_updated;
  bool updated_this_cycle;
} AniEntry;

typedef struct AniFile {
  AniEntry *entries;
  size_t size;
  size_t capacity;
} AniFile;

struct AniFileStats {
  int total_completed;
  int total_dropped;
  int total_watching;
  int total_hold;
  int total_planned;
  int total_n_ep_watched;
  int total_n_entries;
  time_t combined_watch_time;
  double average_score;
  char *fav_tag;
};

typedef struct AniEntrySearchResult {
  int entry_id;
  char *text_fields_concat;
} AniEntrySearchResult;

// io.h

char **get_tags_from_field(char *s);

void AniFile_push_AniEntry(AniFile *af, AniEntry e);
AniFile read_anifile(FILE *f);
void write_anifile(AniFile *af, FILE *f);
char *AniEntryStatus_to_str(AniEntryStatus aes);

// api.h

void AniFile_get_stats(AniFile *af, struct AniFileStats *a);

char *AniEntryStatus_to_str(AniEntryStatus aes);
AniEntryStatus str_to_AniEntryStatus(const char *s);

char *field_icon(enum AniCurrentFieldState acfs);

int AniFile_get_most_recent_id(AniFile *af);
bool AniFile_has_changed(const AniFile *snapshot, const AniFile *current);

AniEntry *AniFile_find_random_plan_to_watch_entry(AniFile *af);
AniEntry *AniFile_find_entry_by_id(AniFile *af, int id);

// returns an array of id's matching search term
int *AniFile_search_for_entries(AniFile *af, const char *search_term,
                                size_t *out_count);

bool AniFile_remove_entry_by_id(AniFile *af, int id);

void AniFile_collect_entries_with_certain_status(AniFile *af,
                                                 AniEntry **entries,
                                                 AniEntryStatus s);

bool AniEntry_is_probably_a_new_entry(const AniEntry *new_entry,
                                      const AniFile *af);

// takes the path of the .ani file in home directory and gives back a path for
// the fallback .ani file
char *get_set_ani_path(FILE *f);

// qsort related functions ( src/cli/cli.c : cmd_list() )

int AniEntry_qsort_by_name(const void *x_void, const void *y_void);
int AniEntry_qsort_by_score(const void *x_void, const void *y_void);
int AniEntry_qsort_by_updated(const void *a, const void *b);
int AniEntry_qsort_by_released(const void *a, const void *b);
int AniEntry_qsort_by_progress(const void *a, const void *b);
void AniEntry_reverse_array(AniEntry **entries, size_t n);
#endif
