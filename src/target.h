//target.h

//Unit target: Target type.

#ifndef TARGET_H
#define TARGET_H

#include "geometry.h"

class Actor;
struct invnode;
struct level_type;
class Tar_Ball;

/* this is used when something is targetted by player */
struct Target
{
	enum Target_Type
	{
		None,
		Creature,
		Level_Item,
		Inventory_Item, //doesn't have location
		Terrain
	};

	int type; //which type of target it is
	Actor *olento; //monster targetted
	invnode *invitem; //item targetted
	Coord pos; //coordinates of the target

	Target() : type(None), olento(0), invitem(0) { }

	Coord Get_Location();

	bool Is_Active();

	void Clear();
	void Set(const Coord &c);
	void Set(Actor *o);
	void Set(invnode *i);
	void Set(invnode *i, const Coord &c);

	void Show_Data();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, level_type *level);
};

#endif
