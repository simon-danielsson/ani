#include "../utils.h"
#include "db_format.h"
#include <stdio.h>
#include <stdlib.h>

// split by ',' and return array of char*
char **get_tags_from_field(char *s) {

    char **output = calloc(TAG_TMP_BUFF, TAG_TMP_BUFF * sizeof(*output));

    if (output == NULL) {
        // TODO: error handling
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    int j = 0;
    char tmp[TAG_TMP_BUFF] = {0};
    int t = 0;

    while (t < TAG_MAX_N) {
        if (s[i] == ',' || s[i] == '\0') {
            trim_str(tmp);
            str_to_lowercase(tmp, TAG_TMP_BUFF);
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
            panic("out of memory\n");
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
                        trim_str(tmp);
                        e.last_updated = time_t_from_iso_ymd(tmp);
                        e.updated_this_cycle = false;
                        break;
                    default:
                        printf("Error: encountered unknown field '%s' while reading file",
                                tmp);
                        exit(EXIT_FAILURE);
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

/*
   id|name|year|tags|score|eptot|epwat|note|stat|dateupd
   1|One Piece|1995|action,adventure,comedy|10|1400|5|this is very
   good|0|2012-01-14 2|Ghost in The
   Shell|1995|action,mecha|9|1|1|ghosts|1|2021-04-02 3|Black
   Lagoon|2006|action|7|12|9| |3|2025-12-31 4|Kaguya-Sama: Love is war|2023|
   |7|13|4|watch to-love-ru after this one |2|2025-12-31
   */

void write_anifile(AniFile *af, FILE *f) {
    fprintf(f, "id|name|year|tags|score|eptot|epwat|note|stat|dateupd\n");

    // printf("---------\n");
    for (size_t i = 0; i < af->size; i++) {

        // id
        fprintf(f, "%d", af->entries[i].id);
        fprintf(f, "|");

        // name
        fprintf(f, "%s", af->entries[i].name);
        fprintf(f, "|");

        // release year
        {
            char tmp[64] = {0};
            format_time_t_year(tmp, sizeof(tmp), &af->entries[i].released, true);
            fprintf(f, "%s", tmp);
        }
        fprintf(f, "|");

        // tags
        if (af->entries[i].tags != NULL) {
            char tmp[256] = {0};
            size_t tmp_pos = 0;
            for (size_t j = 0; j < TAG_MAX_N; j++) {
                if (af->entries[i].tags[j] != NULL) {
                    char tag[64];
                    snprintf(tag, sizeof(tag), "%s,", af->entries[i].tags[j]);
                    strcpy(tmp + tmp_pos, tag);
                    tmp_pos += strlen(tag);
                }
            }
            // trim last ','
            if (tmp_pos > 0 && tmp[tmp_pos - 1] == ',') {
                tmp[tmp_pos - 1] = '\0';
            }
            fprintf(f, "%s", tmp);
        } else {
            fprintf(f, " ");
        }
        fprintf(f, "|");

        // score
        fprintf(f, "%d", af->entries[i].score);
        fprintf(f, "|");

        fprintf(f, "%d", af->entries[i].ep_total);
        fprintf(f, "|");
        fprintf(f, "%d", af->entries[i].ep_watched);
        fprintf(f, "|");

        // note
        if (af->entries[i].note != NULL) {
            fprintf(f, "%s", af->entries[i].note);
        } else {
            fprintf(f, " ");
        }
        fprintf(f, "|");

        // status
        fprintf(f, "%d", af->entries[i].status);
        fprintf(f, "|");

        // date updated
        {
            time_t u = af->entries[i].last_updated;
            if (af->entries[i].updated_this_cycle) {
                u = time(NULL);
            }
            char tmp[64] = {0};
            format_time_t_year(tmp, sizeof(tmp), &u, false);
            fprintf(f, "%s", tmp);
        }
        fprintf(f, "\n");
    }

    return;
}
