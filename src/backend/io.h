#pragma once

#include "db_format.h"

char **get_tags_from_field(char *s);

void AniFile_push_AniEntry(AniFile *af, AniEntry e);
AniFile read_anifile(FILE *f);
void write_anifile(AniFile *af, FILE *f);
char *AniEntryStatus_to_str(AniEntryStatus aes);
