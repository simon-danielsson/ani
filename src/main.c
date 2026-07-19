#include "main.h"
#include "backend/db_format.h"
#include "backend/io.h"

void AniFile_debug_print(AniFile af) {
    printf("---------\n");
    for (size_t i = 0; i < af.size; i++) {

        printf("\x1b[30;42m%-70s \x1b[0m\n", af.entries[i].name);

        printf("id: %d \n", af.entries[i].id);

        for (size_t j = 0; j < TAG_MAX_N; j++) {
            if (af.entries[i].tags[j] != NULL) {
                printf("tags: #%s ", af.entries[i].tags[j]);
            }
        }
        printf("\n\n");
    }
}

int main(void) {
    FILE *f = fopen("test.ani", "rw");
    AniFile af = read_anifile(f);

    AniFile_debug_print(af);

    // do stuff with the intermediate representation using api functions

    write_anifile(&af, f);

    return 0;
}
