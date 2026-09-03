//Legend of Saladir - drill.h

//Unit drill: Use of skills.

#ifndef DRILL_H
#define DRILL_H

struct level_type;
struct playerinfo;

void use_quickskill(playerinfo *plr, level_type *level, int slot);
bool use_skill(playerinfo *plr, level_type *level, int group, int skill);

#endif
