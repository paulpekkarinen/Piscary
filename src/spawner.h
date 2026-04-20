//Legend of Saladir - spawner.h

//Unit spawner: Create objects to a level.

#ifndef SPAWNER_H
#define SPAWNER_H

struct Coord;
struct invnode;
struct item_def;
struct level_type;

class Spawner
{
private:
	level_type *lvl; //points to level where the object is created
	
	int Get_Random_Item(int type);

	void Set_Random_Condition(item_def *item);

public:
	explicit Spawner(level_type *luola);

	invnode *Create_Item(const Coord &c, int type, int subtype, int count, int material);
	void Create_Shop_Item(const Coord &c, int shoptype, int sellp);
};

#endif
