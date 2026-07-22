#ifndef UTILS_H
#define UTILS_H

#include "backend/db_format.h"
#include "main.h"

#define MORE_INFO "run 'ani -h' for more information"

size_t get_fsize(FILE **f);

void panic(const char *s);

char *read_entire_file(FILE *f);

bool str_is_empty(const char *s);

// takes either "YYYY-MM-DD" or "YYYY"
time_t time_t_from_iso_ymd(const char *iso_str);

double time_t_to_days(time_t t);

void format_time_t_year(char *buff, size_t buff_size, time_t *time,
                        bool only_year);

void AniFile_debug_print(AniFile af);

void trim_str(char *str);

void str_to_lowercase(char *s, size_t len);

char *expand_home_path(const char *path);
#endif
