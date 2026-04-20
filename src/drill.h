//Legend of Saladir - drill.h

//Unit drill: Use of skills.

#ifndef DRILL_H
#define DRILL_H

struct level_type;
struct playerinfo;

void init_skills();
void use_quickskill(playerinfo *plr, level_type *level, int slot);
int skill_listselect(int group, const char *prompt);
bool use_skill(playerinfo *plr, level_type *level, int group, int skill);

#endif
