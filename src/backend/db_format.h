#pragma once

#include "../main.h"

typedef enum {
  SHOW = 0,
  MOVIE = 1,
} AniEntryType;

typedef enum {
  WATCHING = 0,
  COMPLETED = 1,
  ON_HOLD = 2,
  DROPPED = 3,
  PLAN_TO_WATCH = 4
} AniEntryStatus;

typedef struct {
  AniEntryType type;
  char *name;
  int year;
  char **tags;
  int score;
  int ep_total;
  int ep_watched;
  AniEntryStatus status;
  char *date_updated;
  char *date_added;
} AniEntry;

typedef struct {
  AniEntry *entry;
  size_t count;
} AniFile;
