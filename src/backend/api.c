#include "db_format.h"

char *AniEntryStatus_to_str(AniEntryStatus aes) {
    char *status;
    switch (aes) {
        case WATCHING:
            status = "Watching";
            break;
        case COMPLETED:
            status = "Completed";
            break;
        case ON_HOLD:
            status = "On hold";
            break;
        case DROPPED:
            status = "Dropped";
            break;
        case PLAN_TO_WATCH:
            status = "Plan to watch";
            break;
    }
    return status;
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
        default:
            field_name = "??????";
            break;
    }
    return field_name;
}
