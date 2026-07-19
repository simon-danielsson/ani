#include "db_format.h"

char *field_icon(enum AniCurrentFieldState acfs, bool devicon) {
    char *icons_dev[10] = {"", "󰷝", "",  "", "",
        "", "",  "󰎛", "", "󰚰"};
    char *icons_tty[10] = {"I", "N", "Y", "T", "R", "E", "E", "N", "S", "U"};

    if (devicon) {
        char *tmp = icons_dev[acfs];
        return tmp;
    } else {
        char *tmp = icons_tty[acfs];
        return tmp;
    }
}

char *AniEntryStatus_to_str(AniEntryStatus aes) {
    char *tmp;
    char *status[5] = {"Watching", "Completed", "On hold", "Dropped",
        "Plan to watch"};
    return status[aes];
}

char *anifield_enum_to_str(enum AniCurrentFieldState a) {
    char *field_name;
    switch (a) {
        case ID:
            field_name = "Id";
            break;
        case NAME:
            field_name = "Name";
            break;
        case TAGS:
            field_name = "Tags";
            break;
        case YEAR:
            field_name = "Year";
            break;
        case SCORE:
            field_name = "Score";
            break;
        case EP_TOT:
            field_name = "Ep total";
            break;
        case EP_WAT:
            field_name = "Ep watched";
            break;
        case NOTE:
            field_name = "Note";
            break;
        case STAT:
            field_name = "Status";
            break;
        case DATEUPD:
            field_name = "Last updated";
            break;
    }
    return field_name;
}
