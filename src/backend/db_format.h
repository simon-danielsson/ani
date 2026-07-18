#pragma once

#include "../main.h"

typedef enum {
  WATCHING = 0,
  COMPLETED = 1,
  ON_HOLD = 2,
  DROPPED = 3,
  PLAN_TO_WATCH = 4
} AniEntryStatus;

typedef struct {
  char *name;
  char **tags;
  int score;
  int ep_total;
  int ep_watched;
  AniEntryStatus status;
  time_t released;
  time_t *last_updated;
} AniEntry;

typedef struct {
  AniEntry *entry;
  size_t count;
} AniFile;
