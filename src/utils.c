#include "utils.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "main.h"

size_t get_fsize(FILE **f) {
    fseek(*f, 0, SEEK_END);
    long fsize = ftell(*f);
    fseek(*f, 0, SEEK_SET);
    return fsize;
}

void panic(const char *s) {
    printf("Panic: %s\n", s);
    exit(EXIT_FAILURE);
}

char *read_entire_file(FILE *f) {
    size_t fsize = get_fsize(&f);
    char *content = malloc(fsize + 1);
    fread(content, fsize, 1, f);
    int r = fclose(f);
    if (r != 0) {
        panic("failed to close file after read");
    }
    content[fsize] = 0;
    return content;
}

bool str_is_empty(const char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isalpha(s[i])) {
            return false;
        }
    }
    return true;
}

void str_to_lowercase(char *s, size_t len) {
    for (size_t i = 0; i < len; i++)
        s[i] = tolower(s[i]);
}

// takes either "YYYY-MM-DD" or "YYYY"
time_t time_t_from_iso_ymd(const char *iso_str) {
    struct tm tm = {0};
    if (!strstr(iso_str, "-")) { // if no '-', assume "YYYY"
        tm.tm_year = atoi(iso_str) - 1900;
        tm.tm_mon += 6;
        tm.tm_isdst = -1;
        return mktime(&tm);
    } else if (strptime(iso_str, "%Y-%m-%d", &tm) == NULL)
        return (time_t)-1;

    tm.tm_isdst = -1;
    return mktime(&tm);
}

double time_t_to_days(time_t t) { return ((double)t / 86400); }

void trim_str(char *str) {
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        str[0] = '\0';
        return;
    }
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    size_t len = (end - start) + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

void format_time_t_year(char *buff, size_t buff_size, time_t *time,
        bool only_year) {
    if (only_year) {
        strftime(buff, buff_size, "%Y", localtime(time));
    } else {
        strftime(buff, buff_size, "%Y-%m-%d", localtime(time));
    }
}

// expand leading '~' to $HOME
// returns an allocated string to be freed by the caller
char *expand_home_path(const char *path) {
    const char *home = getenv("HOME");
    if (!path) {
        return NULL;
    }
    if (path[0] != '~') {
        return strdup(path);
    }
    if (!home) {
        return strdup(path);
    }
    if (path[1] != '\0' && path[1] != '/') {
        return strdup(path);
    }
    char *expanded = malloc(strlen(home) + (strlen(path) + 1) + 1);
    if (!expanded) {
        return NULL;
    }
    strcpy(expanded, home);
    strcat(expanded, path + 1);

    return expanded;
}
