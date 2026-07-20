#ifndef CLI_H
#define CLI_H

#include "../backend/db_format.h"

void cmd_add(AniFile *af);
void cmd_ep(AniFile *af, int id);
void cmd_edit(AniFile *af, int id);
void cmd_info(AniFile *af, int id, bool devicons);

#endif
