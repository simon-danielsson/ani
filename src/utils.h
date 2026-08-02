#ifndef UTILS_H
#define UTILS_H

#include "backend/backend.h"
#include "main.h"

#define MORE_INFO "run 'ani -h' for more information"

size_t get_fsize(FILE **f);

void panic(const char *s);

size_t char_len_of_int(int i);
size_t char_len_of_float(float f, int precision);

char *read_entire_file(FILE *f);

bool str_is_empty(const char *s);

// takes either "YYYY-MM-DD" or "YYYY"
time_t time_t_from_iso_ymd(const char *iso_str);

double time_t_to_days(time_t t);

void format_time_t_year(char *buff, size_t buff_size, time_t *time,
                        bool only_year);

void trim_str(char *str);

void str_to_lowercase(char *s, size_t len);

char *expand_home_path(const char *path);

FILE *get_anifile_handle(char *filepath);
void get_anifile_path(char *filepath, char *buf, size_t buf_size);

#endif
