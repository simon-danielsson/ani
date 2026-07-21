#ifndef CLI_H
#define CLI_H

#include "../backend/db_format.h"

typedef enum {
  RED,
  BLUE,
  GREEN,
  MAGENTA,
  YELLOW,
  RESET,
} Color;

char *ansi_from_color(Color c);

typedef struct {
  AniEntryStatus stat;
  int total;
  int scaled_total;
  Color color;
} StatsBarField;

#define COULD_NOT_FIND_ENTRY_BY_ID                                             \
  if (!e) {                                                                    \
    printf("Error: an entry with id '%d' could not be found -- %s", id,        \
           MORE_INFO);                                                         \
    exit(EXIT_FAILURE);                                                        \
  }

void cmd_add(AniFile *af);
void cmd_ep(AniFile *af, int id);
void cmd_edit(AniFile *af, int id);
void cmd_info(AniFile *af, int id, bool devicons);
void cmd_rm(AniFile *af, int id);
void cmd_stats(AniFile *af);
void cmd_rec(AniFile *af, bool devicons);

#endif
