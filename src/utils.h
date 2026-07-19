#ifndef UTILS_H
#define UTILS_H

#include "main.h"

size_t get_fsize(FILE **f);

static void panic(const char *s);

char *read_entire_file(FILE *f);

bool str_is_empty(const char *s);

// takes either "YYYY-MM-DD" or "YYYY"
time_t time_t_from_iso_ymd(const char *iso_str);

void format_time_t_year(char *buff, size_t buff_size, time_t *time,
                        bool only_year);

#endif
