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

bool drop_everything(
	inventory *inv, equipment &gear, level_type *level, int x, int y);
void drop_item(playerinfo *plr, level_type *level);
invnode *drop_item(inventory *inv, invnode *in_src, equipment &gear,
	level_type *level, int count, const Coord &c);
void drop_loot(being *mons, level_type *level);
void drop_selected(Pocket &tasku, level_type *level, const Coord &c);
void pick_up_item(playerinfo *plr, level_type *level);
bool shopkeeper_drop(level_type *level, being *keeper);
bool shopkeeper_get(level_type *level, being *monster, invnode *itemptr);

#endif
