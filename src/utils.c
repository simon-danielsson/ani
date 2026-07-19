#include "utils.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "main.h"
#include <stddef.h>

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
    }

    if (strptime(iso_str, "%Y-%m-%d", &tm) == NULL)
        return (time_t)-1;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

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
    struct tm *t = localtime(time);

    if (only_year) {
        strftime(buff, buff_size, "%Y", t);
    } else {
        strftime(buff, buff_size, "%Y-%m-%d", t);
    }
}

void AniFile_debug_print(AniFile af) {
    // printf("---------\n");
    for (size_t i = 0; i < af.size; i++) {

        // name & id
        printf("\x1b[1;30;42m%-3d %-70s \x1b[0m\n", af.entries[i].id,
                af.entries[i].name);
        printf("idx: %zu\n", i);

        // released
        {
            char tmp[64] = {0};
            format_time_t_year(tmp, sizeof(tmp), &af.entries[i].released, true);
            printf("released: %s \n", tmp);
        }

        printf("score: %d \n", af.entries[i].score);
        printf("total ep: %d \n", af.entries[i].ep_total);
        printf("watched ep: %d \n", af.entries[i].ep_watched);
        if (af.entries[i].note != NULL) {
            printf("note: %s \n", af.entries[i].note);
        }

        printf("status: %s \n", AniEntryStatus_to_str(af.entries[i].status));

        // tags
        if (af.entries[i].tags != NULL) {
            printf("tags: ");
            for (size_t j = 0; j < TAG_MAX_N; j++) {
                if (af.entries[i].tags[j] != NULL) {
                    printf("#%s ", af.entries[i].tags[j]);
                }
            }
        }

        printf("\n\n");
    }
}
