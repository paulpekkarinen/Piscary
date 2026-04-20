//Legend of Saladir - specmon.h

//Unit specmon: Functions for special monsters.

#ifndef SPECMON_H
#define SPECMON_H

#include "types.h"

/* special monster types */
#define NPC_BILLGATES	1
#define NPC_SPARHAWK	2
#define NPC_THOMAS      3
#define NPC_NATASHA		4

void act_specialmonster(being *monster, level_type *level);
void eat_specialmonster(level_type *level, int type);
void monster_chatspecials(being *mptr);

#endif
