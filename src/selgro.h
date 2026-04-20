//Legend of Saladir - selpack.h

//Unit selgro: Inventory selection from ground.

#ifndef SELGRO_H
#define SELGRO_H

#include "inventor.h"

class Stash;

class SelectItemsGround : public SelectItems
{
private:
	Stash &inv; //source inventory
	Coord place; //location of items on level map
	bool darklevel; //if items are in a dark place

public:
	SelectItemsGround(Stash &srcinv, int flt);

	invnode *Get_Handle(const char *preprompt, const Coord &c);

	bool Change_Category(int select) override;
	void Set_Header(const char *preprompt) override;
	bool Select_Items(const char *preprompt, const Coord &c);
	void Set_Place(const Coord &c) { place=c; }
};

#endif
