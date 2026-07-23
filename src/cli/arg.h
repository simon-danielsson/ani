#ifndef ARG_H
#define ARG_H

#include "../main.h"

typedef enum {
  F_HELP,
  F_HELP_LONG,
  F_GUIDE,
  F_FILE,
  F_FILE_LONG,
  F_ICONS,
  F_ICONS_LONG,
  C_ADD,
  C_EP,
  C_EDIT,
  C_INFO,
  C_RM,
  C_STATS,
  C_REC,
  C_SEARCH,
  C_LIST,
  C_LS,
  C_LIST_WATCH,
  C_LIST_COMPL,
  C_LIST_ONHOL,
  C_LIST_DROPP,
  C_LIST_PLANN,
  F_SORT_REVR,
  F_SORT_NAME,
  F_SORT_SCOR,
  F_SORT_UPDA,
  F_SORT_RELE,
  F_SORT_PROG,
  F_VERS,
  F_VERS_LONG,
  _ARGS_N
} ArgType;

char *arg_as_str(ArgType at);

typedef struct {
  ArgType t;
  char *s;
} Arg;

typedef struct {
  Arg *items;
  size_t size;
  size_t capacity;
} Args;

Args *parse_args(int argc, char **argv);

Arg *Args_find_arg(const Args *args, ArgType t1, ArgType t2);

#endif
