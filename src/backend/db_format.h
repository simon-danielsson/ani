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
  ID = 0,
  NAME = 1,
  YEAR = 2,
  TAGS = 3,
  SCORE = 4,
  EP_TOT = 5,
  EP_WAT = 6,
  NOTE = 7,
  STAT = 8,
  DATEUPD = 9
};

typedef struct {
  char *name;
  char **tags;
  int id;
  int score;
  int ep_total;
  int ep_watched;
  AniEntryStatus status;
  time_t released;
  time_t *last_updated;
} AniEntry;

typedef struct {
  AniEntry *entries;
  size_t size;
  size_t capacity;
} AniFile;
