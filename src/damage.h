/* damage.h */

//Unit damage: Damage types.

#ifndef DAMAGE_H
#define DAMAGE_H

#include "types.h"

/* magic elements */
#define ELEMENT_NOTHING 0
#define ELEMENT_FIRE    1
#define ELEMENT_POISON  2
#define ELEMENT_COLD    3
#define ELEMENT_ELEC    4
#define ELEMENT_WATER   5
#define ELEMENT_ACID    6

void damage_checkbodyparts(level_type *level, Actor *mptr);
int damage_issue(level_type *level,
	Actor *target, Actor *attacker,
	int element, int damage, int bodypart,
	const char *message);

#endif


