//Legend of Saladir - melee.h

//Unit melee: Close combat.

#ifndef MELEE_H
#define MELEE_H

struct being;
struct level_type;
struct playerinfo;

void meleeattack(playerinfo *plr, level_type *level, being *mptr);
void monster_meleeattack(being *mptr, level_type *level, Actor *target);

#endif
