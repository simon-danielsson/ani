#include "main.h"
#include "backend/backend.h"
#include "frontend/frontend.h"
#include "utils.h"

// must be located in env $HOME
#define SETFILE_NAME ".ani"

int main(int argc, char **argv) {
    _run_test(TEST);
    srand((unsigned)time(NULL));

    Arg_init_all();
    if (!Arg_parse(argc, argv)) {
        return 1;
    };

    PrgVars pv = PrgVars_init();
    PrgVars_setup(&pv);
    // PrgVars_debug_print(&pv);

    // path from arg or else fallback file
    FILE *f = get_anifile_handle(pv.filepath);

    AniFile af = read_anifile(get_anifile_handle(pv.filepath));
    // AniFile *snapshot = &af;

    if (pv.cmd) {
        pv.cmd(&af, &pv);
    }

    fclose(f);

    /*
       TODO: find a reliable way to check if the anifile has changed. the old
       version doesn't work reliably anymore after the rewrite of the arg
       handling. Perhaps I could do this using a hash but that would require a
       deep copy of the anifile which is probably best avoided until I've
       refactored the memory model
       */

    // if (AniFile_has_changed(snapshot, &af)) {
    // printf("file changed");
    write_anifile(&af, get_anifile_handle(pv.filepath));
    // };

    return 0;
}
