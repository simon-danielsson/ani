#include "main.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "cli/arg.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    Args *args = parse_args(argc, argv);

    FILE *f = NULL;
    char f_name[128] = {0};
    for (size_t i = 0; i < args->size; i++) {
        if (args->items[i].t == F_FILE || args->items[i].t == F_FILE_LONG) {
            f = fopen(args->items[i].s, "r+");
        }
    }
    if (!f) {
        printf("Error: failed to open file -- %s", MORE_INFO);
        exit(EXIT_FAILURE);
    }

    // read
    AniFile af = read_anifile(f);

    // snapshot to compare with before writing to file
    // AniFile snapshot = af;

    AniFile_debug_print(af);

    // {
    //     FILE *f = fopen("./tests/output.ani", "w");
    //     // if (AniFile_has_changed(&snapshot, &af)) {
    //     write_anifile(&af, f);
    //     // };
    // }

    // TODO: free memory of Args and AniFile properly at end of program

    return 0;
}
