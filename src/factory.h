//Legend of Saladir - factory.h

//Unit factory: Creation of game objects.

#ifndef FACTORY_H
#define FACTORY_H

#include "types.h"

struct Itempack;

//Creates item and other object instances.
class Factory
{
private:
	void Init_Scrollitem(int subtype, item_def *i);
	void Init_Moneyitem(int subtype, item_def *i);
	void Monster_Initrandom(being *newptr, int type);
	void Monster_Postgeneration(level_type *level, being *mptr);
	being *New_Empty_Monster();
	void Plant_Monster(being *b, const Coord &c);
	void Set_Alignment(item_def *i);
	void Set_Material(item_def *i, int material);

public:
	void Add_Monster(level_type *level, int x, int y, int type);
	void Add_Shopkeeper(level_type *level, int roomnum);
	void Add_Special_Monsters(level_type *level);
	invnode *New_Item(const Itempack &ip);
	invnode *New_Item_From(invnode *src, int count);
	invnode *New_Rock(); //creates a rock
};

extern Factory factory;

#endif
