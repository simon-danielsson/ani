#include "main.h"
#include "backend/io.h"

int main(void) {
    FILE *f = fopen("test.ani", "rw");
    // AniFile af = read_anifile(f);
    read_anifile(f);

    // do stuff with the intermediate representation using api functions

    // write_anifile(f);

    return 0;
}
