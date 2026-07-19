#include "main.h"
#include "backend/db_format.h"
#include "backend/io.h"

void format_time_t_year(char *buff, time_t *time, bool only_year) {
    struct tm *t = localtime(time);
    if (only_year) {
        strftime(buff, sizeof(buff), "%Y", t);
    } else {
        strftime(buff, sizeof(buff), "%Y-%m-%d", t);
    }
}

void AniFile_debug_print(AniFile af) {
    printf("---------\n");
    for (size_t i = 0; i < af.size; i++) {

        // name & id
        printf("\x1b[1;30;42m%-3d %-70s \x1b[0m\n", af.entries[i].id,
                af.entries[i].name);

        // released
        {
            char released_tmp_buff[64] = {0};
            format_time_t_year(released_tmp_buff, &af.entries[i].released, true);
            printf("released: %s \n", released_tmp_buff);
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
            for (size_t j = 0; j < TAG_MAX_N; j++) {
                if (af.entries[i].tags[j] != NULL) {
                    printf("tags: #%s ", af.entries[i].tags[j]);
                }
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
