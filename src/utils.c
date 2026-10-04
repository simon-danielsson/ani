#include "utils.h"
#include "main.h"

size_t get_fsize(FILE **f) {
    fseek(*f, 0, SEEK_END);
    long fsize = ftell(*f);
    fseek(*f, 0, SEEK_SET);
    return (size_t)fsize;
}

void error(const char *s) {
    printf("Error: %s -- %s\n", s, MORE_INFO);
    exit(EXIT_FAILURE);
}

void panic(const char *s) {
    printf("Panic: %s -- %s\n", s, MORE_INFO);
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

size_t char_len_of_float(float f, int precision) {
    return snprintf(NULL, 0, "%.*f", precision, f);
}

size_t char_len_of_int(int i) {
    size_t len = 1;
    if (i != 0) {
        len = floor(log10(abs(i))) + 1;
    }
    return len;
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
    size_t len = (size_t)(end - (size_t)start) + 1;
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

// expands leading '~' to $HOME or folds $HOME to leading '~'
// returns an allocated string to be freed by the caller
// returns NULL if path param NULL or if env $HOME can't be found
char *expand_home_path(const char *path) {
    const char *home = getenv("HOME");

    if (!path || !home)
        return NULL;

    size_t home_len = strlen(home);
    if (strncmp(path, home, home_len) == 0) {
        size_t len = strlen(path) - home_len + 2;
        char *out = malloc(len);
        if (!out)
            return NULL;

        out[0] = '~';
        strcpy(out + 1, path + home_len);
        return out;
    }

    if (path[0] != '~')
        return strdup(path);

    if (path[1] != '\0' && path[1] != '/')
        return strdup(path);

    size_t len = strlen(home) + strlen(path + 1) + 1;
    char *out = malloc(len);
    if (!out)
        return NULL;

    strcpy(out, home);
    strcat(out, path + 1);

    return out;
}

#define FAILED_TO_OPEN                                                         \
    printf("Error: failed to open file -- %s", MORE_INFO);                       \
    exit(EXIT_FAILURE);

FILE *get_anifile_handle(char *filepath) {

    FILE *handle = NULL;

    if (filepath[0]) {
        handle = fopen(filepath, "r+");
    }
    if (!handle) {
        FILE *f_fallback = NULL;
        char tmp[256];
        {
            char *ani_loc = ".ani";
            char *home = getenv("HOME");
            snprintf(tmp, sizeof(tmp), "%s/%s", home, ani_loc);
            f_fallback = fopen(tmp, "r+");
        }

        if (!f_fallback) {
            printf("Missing .ani file: %s\n", tmp);
            printf("This is required to run ani without a file flag...\n");
            FAILED_TO_OPEN
        }

        // retrieve path of fallback file within HOME/.ani
        char *fallback_file = get_set_ani_path(f_fallback);

        handle = fopen(fallback_file, "r+");
        if (!handle) {
            printf("Error: missing or broken fallback '%s' inside '%s'\n",
                    fallback_file, tmp);
            FAILED_TO_OPEN
        }
    }
    return handle;
}

/*
   TODO: right now I have two functions that do the same thing but with
   different return values, this needs to be fixed. i.e get_anifile_path() &
   get_anifile_handle()
   */

void get_anifile_path(char *filepath, char *buf, size_t buf_size) {

    if (filepath[0]) {
        FILE *probe = fopen(filepath, "r+");
        if (probe) {
            fclose(probe);
            strncpy(buf, filepath, buf_size - 1);
            buf[buf_size - 1] = '\0';
            return;
        }
    }

    FILE *f_fallback = NULL;
    char tmp[256];
    {
        char *ani_loc = ".ani";
        char *home = getenv("HOME");
        snprintf(tmp, sizeof(tmp), "%s/%s", home, ani_loc);
        f_fallback = fopen(tmp, "r+");
    }

    if (!f_fallback) {
        printf("Missing .ani file: %s\n", tmp);
        printf("This is required to run ani without a file flag...\n");
        FAILED_TO_OPEN
    }

    char *fallback_file = get_set_ani_path(f_fallback);
    fclose(f_fallback);

    FILE *probe = fopen(fallback_file, "r+");
    if (!probe) {
        printf("Error: missing or broken fallback '%s' inside '%s'\n",
                fallback_file, tmp);
        FAILED_TO_OPEN
    }
    fclose(probe);

    strncpy(buf, fallback_file, buf_size - 1);
    buf[buf_size - 1] = '\0';
    return;
}
