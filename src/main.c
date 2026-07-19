#include "main.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "cli/arg.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // process cli arguments
    // FILE *f = fopen("./tests/input.ani", "r+");
    // if (!f) {
    //     perror("Failed to open file");
    //     exit(EXIT_FAILURE);
    // }

    Args args = parse_args(argc, argv);

    FILE *f;
    for (size_t i = 0; i < args.size; i++) {
        if (args.items[i].t == F_FILE) {
            f = fopen(args.items[i].s, "r+");
            if (!f) {
                printf("Error: failed to open '%s'", args.items[i].s);
                exit(EXIT_FAILURE);
            }
        }
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
