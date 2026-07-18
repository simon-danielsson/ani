#include "db_format.h"
#include <ctime>
#include <stdio.h>

size_t get_fsize(FILE **f) {
    fseek(*f, 0, SEEK_END);
    long fsize = ftell(*f);
    fseek(*f, 0, SEEK_SET);
    return fsize;
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

int ani_atoi(const char *d) {
    int num = 0;
    for (int i = 0; d[i] != '\0'; i++) {
        num = num * 10 + (d[i] - '0');
    }
    return num;
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

char *anifield_enum_to_str(enum AniCurrentFieldState a) {
    char *field_name;
    switch (a) {
        case ID:
            field_name = "Id";
            break;
        case NAME:
            field_name = "Name";
            break;
        case TAGS:
            field_name = "Tags";
            break;
        case YEAR:
            field_name = "Year";
            break;
        case SCORE:
            field_name = "Score";
            break;
        case EP_TOT:
            field_name = "Ep total";
            break;
        case EP_WAT:
            field_name = "Ep watched";
            break;
        case NOTE:
            field_name = "Note";
            break;
        case STAT:
            field_name = "Status";
            break;
        case DATEUPD:
            field_name = "Last updated";
            break;
        default:
            field_name = "??????";
            break;
    }
    return field_name;
}

AniFile *AniFile_init() {
    AniFile *container = malloc(sizeof(AniFile));
    if (!container) {
        panic("memory allocation failed");
    }

#define INIT_SIZE 8
    container->entries = malloc(INIT_SIZE * sizeof(AniEntry));
    container->size = INIT_SIZE;
    container->capacity = INIT_SIZE;
#undef INIT_SIZE

    if (!container->entries) {
        free(container);
        panic("memory allocation failed");
    }
    return container;
}

void AniFile_push_AniEntry(AniFile *af, AniEntry e) {
    if (af->size == af->capacity) {
        size_t new_capacity = af->capacity << 1;
        AniEntry *new_entries =
            realloc(af->entries, new_capacity * sizeof(AniEntry));
        if (!new_entries) {
            panic("Out of memory\n");
        }
        af->entries = new_entries;
        af->capacity = new_capacity;
    }
    af->entries[af->size++] = e;
}

#define LINE_DELIM "\n"
#define FIELD_DELIM '|'
AniFile read_anifile(FILE *f) {
    char *content = read_entire_file(f);

    AniFile *af = AniFile_init();

    int line = 0;

    char *line_tok = strtok(content, LINE_DELIM);
    while (line_tok != NULL) {

        // skip header line (just there to make .ani file readable)
        if (line == 0 || str_is_empty(line_tok)) {
            line_tok = strtok(NULL, LINE_DELIM);
            line++;
            continue;
        }

        // iterate on fields
        enum AniCurrentFieldState acfs = 0;

        // typedef struct {
        //   char *name;
        //   char **tags;
        //   int score;
        //   int ep_total;
        //   int ep_watched;
        //   AniEntryStatus status;
        //   time_t released;
        //   time_t *last_updated;
        // } AniEntry;
        char e_name[256] = {0};
        char *e_tags[128] = {0};
        int e_score = 0;
        int ep_total = 0;
        int ep_watched = 0;
        AniEntryStatus e_status = PLAN_TO_WATCH;
        time_t e_released =

            char tmp[256] = {0};
        int field_c = 0;
        int i = 0;
        while (true) {
            if (line_tok[i] == FIELD_DELIM || !line_tok[i]) {
                // TODO: capture fields here into an AniFile variable that will
                // then get returned by the function

                switch (acfs) {
                    case ID:
                        field_name = "Id";
                        break;
                    case NAME:
                        field_name = "Name";
                        break;
                    case TAGS:
                        field_name = "Tags";
                        break;
                    case YEAR:
                        field_name = "Year";
                        break;
                    case SCORE:
                        field_name = "Score";
                        break;
                    case EP_TOT:
                        field_name = "Ep total";
                        break;
                    case EP_WAT:
                        field_name = "Ep watched";
                        break;
                    case NOTE:
                        field_name = "Note";
                        break;
                    case STAT:
                        field_name = "Status";
                        break;
                    case DATEUPD:
                        field_name = "Last updated";
                        break;
                    default:
                        field_name = "??????";
                        break;
                }

                printf("%-15s%-5s%s\n", anifield_enum_to_str(acfs), "--", tmp);
                acfs++;
                field_c = 0;
                if (!line_tok[i]) {
                    break;
                }
                i++; // skip delim
                memset(tmp, 0, sizeof(tmp));
            }
            tmp[field_c] = line_tok[i];
            field_c++;
            i++;
        }

        printf("\n");
        line++;
        acfs = 0;

        line_tok = strtok(NULL, LINE_DELIM);
    }

    free(content);
    return *af;
}

void write_anifile(AniFile *af, FILE *f) { return; };
