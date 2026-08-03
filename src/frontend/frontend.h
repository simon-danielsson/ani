#ifndef FRONTEND_H
#define FRONTEND_H

#include "../backend/backend.h"
#include "../main.h"

typedef enum {
  // flags
  F_HELP,
  F_VERSION,
  F_GUIDE,
  F_FILE,

  // commands
  C_SEARCH,
  C_LIST,
  C_ADD,
  C_EP,
  C_EDIT,
  C_INFO,
  C_REMOVE,
  C_STATS,
  C_REC,

  // sorting flags
  SF_REVERSE,
  SF_NAME,
  SF_SCORE,
  SF_UPDATED,
  SF_RELEASED,
  SF_PROGRESS,
} ArgType;

#define ARG_COUNT 19
#define ARG_MAX_PARAM 6

typedef struct PrgVars PrgVars;

typedef struct PrgVars {
  char filepath[128];
  void (*cmd)(AniFile *af, PrgVars *pv);
  char **params;
  ArgType *sort_flags[2];
  size_t sort_flags_count;
} PrgVars;

typedef struct Arg {
  ArgType t;
  bool provided;
  const char *names[2];
  bool has_sub; // true if arg needs subcommands/flags
  bool is_sort_flag;
  char *param[ARG_MAX_PARAM];
  void (*cmd)(AniFile *af, PrgVars *pv);
} Arg;

char *arg_as_str(ArgType at);

#define PROMPT ">>> "

typedef enum {
  RED,
  BLUE,
  GREEN,
  MAGENTA,
  YELLOW,
  RESET,
} Color;

#define COULD_NOT_FIND_ENTRY_BY_ID                                             \
  if (!e) {                                                                    \
    printf("Error: an entry with id '%d' not found -- %s\n",                   \
           atoi(pv->params[0]), MORE_INFO);                                    \
    exit(EXIT_FAILURE);                                                        \
  }

void Arg_init_all(void);
bool Arg_parse(uint argc, char **argv);
PrgVars PrgVars_init();
void PrgVars_setup(PrgVars *pv);
void PrgVars_debug_print(PrgVars *pv);

void cmd_add(AniFile *af, PrgVars *pv);
void cmd_ep(AniFile *af, PrgVars *pv);
void cmd_edit(AniFile *af, PrgVars *pv);
void cmd_info(AniFile *af, PrgVars *pv);
void cmd_rm(AniFile *af, PrgVars *pv);
void cmd_stats(AniFile *af, PrgVars *pv);
void cmd_rec(AniFile *af, PrgVars *pv);
void cmd_search(AniFile *af, PrgVars *pv);
void cmd_list(AniFile *af, PrgVars *pv);

void flag_guide(AniFile *af, PrgVars *pv);
void flag_version(AniFile *af, PrgVars *pv);
void flag_help(AniFile *af, PrgVars *pv);

#endif
