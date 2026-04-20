/* player and monster
	inventory routines

   Legend of Saladir

   (C) Erno Tuomainen 1997/1998

*/

//Refactored 4.9.2021 - 24.6.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "selpack.h"

SelectItemsPack::SelectItemsPack(inventory &srcinv, int flt)
	: SelectItems(flt), inv(srcinv)
{

}

//Get handle functions don't remove the item, only returns the handle.
invnode *SelectItemsPack::Get_Handle(const char *preprompt)
{
	Set_Header(preprompt); //note: this may not be needed if base Select is called

	return 0;
}

bool SelectItemsPack::Change_Category(int select)
{
	const bool rv=Change_Item_Category(select);

	//using -1, -1 is from inventory, not on ground location
	if (rv)
		inv.builditemarray(mypocket, Get_Filter(), -1, -1);

	return rv;
}

bool SelectItemsPack::Select_Items(const char *preprompt)
{
	// first build a list of items, if no items to match filter
	// it will return false
	if (inv.builditemarray(mypocket, Get_Filter(), -1, -1)==false)
		return false;

	return Select(preprompt);
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
