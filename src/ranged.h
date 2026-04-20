//Legend of Saladir - ranged.h

//Unit ranged: Ranged attack meat and potato functions.

#ifndef RANGED_H
#define RANGED_H

#include "types.h"

struct playerinfo;

int monster_ranged_attack(being *mptr, level_type *level);
void ranged_attack(playerinfo *plr, level_type *level);
bool ranged_line(level_type *level, int out, int output_delay, bool single,
	int x1, int y1, int x2, int y2,
	bool (*hitfunc)(level_type *, Actor *ranger, int, int, bool, int),
	Actor *ranger, int skill);

#endif
