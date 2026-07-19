#include "main.h"

size_t get_fsize(FILE **f) {
    fseek(*f, 0, SEEK_END);
    long fsize = ftell(*f);
    fseek(*f, 0, SEEK_SET);
    return fsize;
}

static void panic(const char *s) {
    printf("Panic: %s\n", s);
    exit(1);
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

// takes either "YYYY-MM-DD" or "YYYY"
time_t time_t_from_iso_ymd(const char *iso_str) {
    struct tm tm = {0};

    if (!strstr(iso_str, "-")) { // if no '-', assume "YYYY"
        tm.tm_year = atoi(iso_str) - 1900;
        tm.tm_mon += 6;
        tm.tm_isdst = -1;
        return mktime(&tm);
    }

    if (strptime(iso_str, "%Y-%m-%d", &tm) == NULL)
        return (time_t)-1;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

void format_time_t_year(char *buff, time_t *time, bool only_year) {
    struct tm *t = localtime(time);
    if (only_year) {
        strftime(buff, sizeof(buff), "%Y", t);
    } else {
        strftime(buff, sizeof(buff), "%Y-%m-%d", t);
    }
}
