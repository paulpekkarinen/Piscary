//Legend of Saladir - pack.h

//Unit pack: Drop or take stuff.

#ifndef PACK_H
#define PACK_H

struct Coord;
struct being;
class equipment;
class inventory;
struct invnode;
struct level_type;
struct playerinfo;
class Pocket;

bool player_autopickup(level_type *level, const Coord &c);
bool drop_everything(
	inventory *inv, equipment &gear, level_type *level, int x, int y);
void drop_loot(being *mons, level_type *level);
void drop_selected(playerinfo *plr);
void pick_up_item(playerinfo *plr, level_type *level);
bool shopkeeper_drop(level_type *level, being *keeper);
bool shopkeeper_get(level_type *level, being *monster, invnode *itemptr);

#endif
