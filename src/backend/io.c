#include "db_format.h"
#include <assert.h>
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

// split by ',' and return array of char*
char **get_tags_from_field(char *s) {

    char **output = calloc(TAG_TMP_BUFF, TAG_TMP_BUFF * sizeof(*output));

    if (output == NULL) {
        // TODO: error handling
        exit(1);
    }

    size_t i = 0;
    int j = 0;
    char tmp[TAG_TMP_BUFF] = {0};
    int t = 0;

    while (t < TAG_MAX_N) {
        if (s[i] == ',' || s[i] == '\0') {
            output[t] = malloc((strlen(tmp) + 1) * sizeof(char));
            strcpy(output[t], tmp);
            j = 0;
            t++;
            memset(tmp, 0, sizeof(tmp));
            if (s[i] == '\0') {
                break;
            }
            i++;
            continue;
        }
        tmp[j] = s[i];
        i++;
        j++;
    }

    return output;
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
    container->size = 0;
    container->capacity = INIT_SIZE;
#undef INIT_SIZE

    if (!container->entries) {
        free(container);
        panic("memory allocation failed");
    }
    return container;
}

// char **split

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

        // AniEntry default values
        int e_id = -1;
        char *e_tags[128] = {0};
        int e_score = 0;
        int ep_total = 0;
        int ep_watched = 0;
        AniEntryStatus e_status = PLAN_TO_WATCH;
        time_t e_released = time(NULL);
        time_t e_last_upd = time(NULL);

        char tmp[256] = {0};
        int field_c = 0;
        int i = 0;

        AniEntry e = {0};

        // AniEntry e = {0};
        while (true) {
            if (line_tok[i] == FIELD_DELIM || !line_tok[i]) {
                // TODO: capture fields here into an AniFile variable that will
                // then get returned by the function

                switch (acfs) {
                    case ID:
                        e.id = atoi(tmp);
                        break;
                    case NAME:
                        e.name = malloc((strlen(tmp) + 1) * sizeof(char));
                        strcpy(e.name, tmp);
                        break;
                    case TAGS:
                        e.tags = get_tags_from_field(tmp);
                        break;
                    case YEAR:
                        printf("field: year (not implemented)\n");
                        // assert(false);
                        break;
                    case SCORE:
                        printf("field: score (not implemented)\n");
                        // assert(false);
                        break;
                    case EP_TOT:
                        printf("field: ep tot (not implemented)\n");
                        // assert(false);
                        break;
                    case EP_WAT:
                        printf("field: ep wat (not implemented)\n");
                        // assert(false);
                        break;
                    case NOTE:
                        printf("field: ep note (not implemented)\n");
                        // assert(false);
                        break;
                    case STAT:
                        printf("field: ep status (not implemented)\n");
                        // assert(false);
                        break;
                    case DATEUPD:
                        printf("field: date upd (not implemented)\n");
                        // assert(false);
                        break;
                    default:
                        printf("field: unknown\n");
                        // assert(false);
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

        AniFile_push_AniEntry(af, e);

        printf("\n");
        line++;
        acfs = 0;

        line_tok = strtok(NULL, LINE_DELIM);
    }

    free(content);
    return *af;
}

void write_anifile(AniFile *af, FILE *f) { return; };
