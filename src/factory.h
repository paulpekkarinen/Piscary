//Legend of Saladir - factory.h

//Unit factory: Creation of game objects.

#ifndef FACTORY_H
#define FACTORY_H

#include "types.h"

struct Area;
struct Itempack;

//Creates item and other object instances.
class Factory
{
private:
	void Init_Scrollitem(int subtype, item_def *i);
	void Init_Moneyitem(int subtype, item_def *i);
	void Monster_Postgeneration(level_type *level, being *mptr, const Coord &c);
	being *New_Monster(int sp);
	being *New_Npc(int sp, const monsterdef &mon);
	being *New_Shopkeeper(int sp, const monsterdef &mon, const Area &ar);
	void Set_Alignment(item_def *i);
	void Set_Material(item_def *i, int material);

public:
	int Get_Species_From_Char(char ch);
	void Add_Monster(level_type *level, int x, int y, int type);
	void Add_Random_Monster(level_type *level, const Coord &c);
	void Add_Shopkeeper(level_type *level, int roomnum);
	void Add_Special_Monsters(level_type *level);
	invnode *New_Item(const Itempack &ip);
	invnode *New_Item_From(invnode *src, int count);
	invnode *New_Rock(); //creates a rock
};

extern Factory factory;

#endif
