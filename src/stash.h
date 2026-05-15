//Legend of Saladir - stash.h

//Unit stash: List of items on ground (level).

#ifndef STASH_H
#define STASH_H

#include "purse.h"

struct being;
struct level_type;
struct Target;

//Derives from inventory and adds location information for items on a level.
class Stash : public inventory
{
public:
	int Count_Items_Flat(int x, int y); //this counts without recursion
	invnode *Find_Largest_Item(const Coord &c);
	invnode *Top_Item(const Coord &c); //get topmost item at this location

	void crush_items(int x, int y, invnode *nothis);
	invnode *Pick_Up(invnode *src);
	void Refresh_Item_Map(); //put items on gameview map
	bool teleport_items(level_type *level, Target *target);
};

#endif
