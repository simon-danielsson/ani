#ifndef ARG_H
#define ARG_H

#include "../main.h"

typedef enum {
  F_HELP,
  F_HELP_LONG,
  F_GUIDE,
  F_GUIDE_LONG,
  F_FILE,
  F_FILE_LONG,
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

Args parse_args(int argc, char **argv);

#endif
