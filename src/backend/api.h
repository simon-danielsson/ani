#include "db_format.h"

/*
   Oh one command I missed is a "stats" command that compiles the list into some
   global statistics. Stuff like an approximated the combined amount of days of
   watch time, episodes watched, number of shows completed, dropped, currently
   watching, total number of entries, average score and so on

   - combined appr. amount of watch time (in days)
   - n of ep watched
   - shows (i.e entries) completed, dropped, currently watching, on hold and
   plan-to-watch
   - total number of entries
   - average score
*/

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

typedef struct {
  int entry_id;
  char *text_fields_concat;
} AniEntrySearchResult;

void AniFile_get_stats(AniFile *af, struct AniFileStats *a);

char *AniEntryStatus_to_str(AniEntryStatus aes);
AniEntryStatus str_to_AniEntryStatus(const char *s);

char *field_icon(enum AniCurrentFieldState acfs, bool devicon);

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
