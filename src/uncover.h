//Legend of Saladir - uncover.h

//Unit uncover: Show debug data.

#ifndef UNCOVER_H
#define UNCOVER_H

struct invnode;
struct item_def;

class Uncover
{
public:
	void Gameview_Data();
	void Inventory_Item(invnode *iptr, int x, int y);
	void Item_Data(item_def &item);
	void Monsterdef_Info(int race);
	void Program_Data();
};

extern Uncover uncover;

#endif
