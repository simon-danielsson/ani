#pragma once

#include "db_format.h"

AniFile read_anifile(FILE *f);
void write_anifile(AniFile *af, FILE *f);
char *AniEntryStatus_to_str(AniEntryStatus aes);
