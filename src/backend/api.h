#include "db_format.h"

char *AniEntryStatus_to_str(AniEntryStatus aes);
AniEntryStatus str_to_AniEntryStatus(const char *s);

int AniFile_get_most_recent_id(AniFile *af);
bool AniFile_has_changed(const AniFile *snapshot, const AniFile *current);
char *anifield_enum_to_str(enum AniCurrentFieldState a);
