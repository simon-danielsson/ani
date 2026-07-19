#include "main.h"
#include "backend/db_format.h"
#include "backend/io.h"
#include "cli/arg.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

void AniFile_debug_print(AniFile af) {
    // printf("---------\n");
    for (size_t i = 0; i < af.size; i++) {

        // name & id
        printf("\x1b[1;30;42m%-3d %-70s \x1b[0m\n", af.entries[i].id,
                af.entries[i].name);

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

bool AniFile_has_changed(const AniFile *snapshot, const AniFile *current) {
    return memcmp(snapshot, current, sizeof *current) != 0;
}

int main(int argc, char **argv) {
    // process cli arguments
    FILE *f = fopen("./tests/input.ani", "r+");
    if (!f) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    process_args(argc, argv);

    // read
    // AniFile af = read_anifile(f);

    // snapshot to compare with before writing to file
    // AniFile snapshot = af;

    // AniFile_debug_print(af);

    // {
    //     FILE *f = fopen("./tests/output.ani", "w");
    //     // if (AniFile_has_changed(&snapshot, &af)) {
    //     write_anifile(&af, f);
    //     // };
    // }

    return 0;
}
