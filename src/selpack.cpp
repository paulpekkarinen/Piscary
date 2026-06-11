/* player and monster
	inventory routines

   Legend of Saladir

   (C) Erno Tuomainen 1997/1998

*/

//Refactored 4.9.2021 - 24.6.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "selpack.h"

SelectItemsPack::SelectItemsPack(inventory &srcinv, int flt)
	: Stockpile(flt), inv(srcinv)
{

}

//Get handle functions don't remove the item, only returns the handle.
invnode *SelectItemsPack::Get_Handle(const char *preprompt)
{
	invnode *rv=0;

	if (inv.builditemarray(mypocket, Get_Filter(), -1, -1)==false)
		return 0;
			
	switch (Select(preprompt, true))
	{
		case Stockpile::Selected:
		case Stockpile::Container_Selected:
			rv=mypocket.Remove_Next_Selected();
		break;
		default: break; 
	}

	return rv;
}

bool SelectItemsPack::Change_Category(int select)
{
	bool rv=Change_Item_Category(select);

	//using -1, -1 is from inventory, not on ground location
	if (rv)
		rv=inv.builditemarray(mypocket, Get_Filter(), -1, -1);

	return rv;
}

int SelectItemsPack::Select_Items(const char *preprompt)
{
	// first build a list of items, if no items to match filter
	// it will return false
	if (inv.builditemarray(mypocket, Get_Filter(), -1, -1)==false)
		return Stockpile::Empty;

	return Select(preprompt, false);
}

void SelectItemsPack::Set_Header(const char *preprompt)
{
	if (&inv==&player.inv)
	{
		prompt=preprompt;
		prompt.append(" (Backpack)");
	}
	else
		prompt="inventory of a ";
}
