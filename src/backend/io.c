#include "io.h"

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

void read_anifile(FILE *f) {
    char *content = read_entire_file(f);

    int line = 0;
    char *delim = "\n";
    char *token = strtok(content, delim);

    while (token != NULL) {

        // skip header line (just there to make .ani file readable by humans)
        if (line == 0) {
            token = strtok(NULL, delim);
            line++;
            continue;
        }

        printf("line %d: %s\n", line, token);
        line++;
        token = strtok(NULL, delim);
    }

    free(content);
}

void write_anifile(AniFile *af, FILE *f) { return; };
