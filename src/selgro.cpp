/* player and monster
	inventory routines

   Legend of Saladir

   (C) Erno Tuomainen 1997/1998

*/

//Refactored 4.9.2021 - 24.6.2025 Paul K. Pekkarinen

#include "gameview.h"
#include "selgro.h"
#include "stash.h"

SelectItemsGround::SelectItemsGround(Stash &srcinv, int flt)
	: SelectItems(flt), inv(srcinv), place(-1, -1), darklevel(false)
{

}

invnode *SelectItemsGround::Get_Handle(const char *preprompt, const Coord &c)
{
	Set_Place(c);
	Set_Header(preprompt); //note: this may not be needed if base Select is called

	return 0;
}

bool SelectItemsGround::Change_Category(int select)
{
	const bool rv=Change_Item_Category(select);

	if (rv)
		inv.builditemarray(mypocket, Get_Filter(), place.x, place.y);

	return rv;
}

void SelectItemsGround::Set_Header(const char *preprompt)
{
	prompt=preprompt;
	prompt.append(" (Ground)");

	if (gameview.Is_Visible(place)==false)
		darklevel=true;
}

bool SelectItemsGround::Select_Items(const char *preprompt, const Coord &c)
{
	Set_Place(c);

	// first build a list of items, if no items to match filter
	// it will return false
	if (inv.builditemarray(mypocket, Get_Filter(), c.x, c.y)==false)
		return false;

	return Select(preprompt);
}
