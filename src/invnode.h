//Legend of Saladir - invnode.h

//Unit invnode: Inventory item with location etc. data.

#ifndef INVNODE_H
#define INVNODE_H

#include "items.h"

//One item in an inventory, either container or level.
struct invnode
{
	int x; // position in the level
	int y;

	int count; // number of this kind of items in inventory

	int slot; // if item is equipped, slot number 
	item_def i;	// item definition structure

	invnode();
	
	Coord Get_Location();
	int Get_Weight(); //includes amount of items (count)
	real Get_Material_Mod(); //in shop pricing
	const char *Get_Name();

	void Set_Location(const Coord &c);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
