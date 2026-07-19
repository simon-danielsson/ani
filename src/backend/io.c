#include "../utils.h"
#include "db_format.h"
#include <assert.h>
#include <stdio.h>

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

char *AniEntryStatus_to_str(AniEntryStatus aes) {
    char *status;
    switch (aes) {
        case WATCHING:
            status = "Watching";
            break;
        case COMPLETED:
            status = "Completed";
            break;
        case ON_HOLD:
            status = "On hold";
            break;
        case DROPPED:
            status = "Dropped";
            break;
        case PLAN_TO_WATCH:
            status = "Plan to watch";
            break;
    }
    return status;
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
                        if (!str_is_empty(tmp)) {
                            e.tags = get_tags_from_field(tmp);
                        } else {
                            e.tags = NULL;
                        }
                        break;
                    case YEAR:
                        e.released = time_t_from_iso_ymd(tmp);
                        break;
                    case SCORE:
                        e.score = atoi(tmp);
                        break;
                    case EP_TOT:
                        e.ep_total = atoi(tmp);
                        break;
                    case EP_WAT:
                        e.ep_watched = atoi(tmp);
                        break;
                    case NOTE:
                        if (!str_is_empty(tmp)) {
                            e.note = malloc((strlen(tmp) + 1) * sizeof(char));
                            strcpy(e.note, tmp);
                        } else {
                            e.note = NULL;
                        }

                        break;
                    case STAT:
                        e.status = atoi(tmp);
                        break;
                    case DATEUPD:
                        e.last_updated = atoi(tmp);
                        break;
                    default:
                        // printf("field: unknown\n");
                        assert(false);
                        break;
                }

                // printf("%-15s%-5s%s\n", anifield_enum_to_str(acfs), "--", tmp);
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

        // printf("\n");
        line++;
        acfs = 0;

        line_tok = strtok(NULL, LINE_DELIM);
    }

    free(content);
    return *af;
}

void write_anifile(AniFile *af, FILE *f) { return; }
