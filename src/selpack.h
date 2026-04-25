//Legend of Saladir - selpack.h

//Unit selpack: Inventory selection from backpack.

#ifndef SELPACK_H
#define SELPACK_H

#include "inventor.h"

class SelectItemsPack : public Stockpile
{
private:
	inventory &inv; //source inventory

public:
	SelectItemsPack(inventory &srcinv, int flt);

	invnode *Get_Handle(const char *preprompt);

	bool Change_Category(int select) override;
	int Select_Items(const char *preprompt);
	void Set_Header(const char *preprompt) override;
};

#endif
