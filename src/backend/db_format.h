#pragma once

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

typedef struct {
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

typedef struct {
  AniEntry *entries;
  size_t size;
  size_t capacity;
} AniFile;
