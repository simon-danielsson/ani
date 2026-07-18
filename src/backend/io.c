#include "io.h"
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
    fclose(f);
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

int get_int_from_char_arr(const char *d) {
    int num = 0;
    for (int i = 0; i < 3; i++) {
        num = num * 10 + (d[i] - '0');
    }
    return num;
}

enum AniCurrentFieldState {
    ID = 0,
    TYPE = 1,
    NAME = 2,
    YEAR = 3,
    TAGS = 4,
    SCORE = 5,
    EP_TOT = 6,
    EP_WAT = 7,
    NOTE = 8,
    STAT = 9,
    DATEUP = 10,
    DATEADD = 11
};

char *anifield_enum_to_str(enum AniCurrentFieldState a) {
    char *field_name;
    switch (a) {
        case ID: // 0
            field_name = "Id";
            break;
        case TYPE: // 1
            field_name = "Type";
            break;
        case NAME: // 2
            field_name = "Name";
            break;
        case TAGS: // 2
            field_name = "Tags";
            break;
        case YEAR: // 3
            field_name = "Year";
            break;
        case SCORE: // 4
            field_name = "Score";
            break;
        case EP_TOT: // 5
            field_name = "Ep total";
            break;
        case EP_WAT: // 6
            field_name = "Ep watched";
            break;
        case NOTE: // 7
            field_name = "Note";
            break;
        case STAT: // 8
            field_name = "Status";
            break;
        case DATEUP: // 9
            field_name = "Last updated";
            break;
        case DATEADD: // 10
            field_name = "Added";
            break;
        default:
            field_name = "??????";
            break;
    }
    return field_name;
}

#define LINE_DELIM "\n"
#define FIELD_DELIM '|'
void read_anifile(FILE *f) {
    char *content = read_entire_file(f);

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
        while (true) {
            if (line_tok[i] == FIELD_DELIM || !line_tok[i]) {
                // TODO: capture fields here into an AniFile variable that will
                // then get returned by the function
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
}

void write_anifile(AniFile *af, FILE *f) { return; };
