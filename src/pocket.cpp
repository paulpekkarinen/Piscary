//Legend of Saladir - pocket.cpp

//Refactored 14.11.2021 - 26.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "currency.h"
#include "display.h"
#include "inventor.h"
#include "invnode.h"
#include "itemdata.h"
#include "items.h"
#include "message.h"
#include "output.h"
#include "pocket.h"
#include "purse.h"
#include "rig.h"

void Pocket::Add_Item(invnode *ptr)
{
	//don't add the item if it's already in the list
	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		if ((*ii).ptr==ptr)
			return;
	}

	items.push_back(itemlistptr(ptr));
}

//If this item is a container, return the container handle.
inventory *Pocket::Get_Container(int index)
{
	invnode *item=Get_Item_Handle(index);

	if (item!=0)
	{
		return item->inv;
	}

	return 0;
}

invnode *Pocket::Get_Item_Handle(int index)
{
	int ci=0;

	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		if (ci==index)
			return (*ii).ptr;

		ci++;
	}

	return 0;
}

//Add without checking if the item already exists.
void Pocket::Push_Item(invnode *ptr)
{
	items.push_back(itemlistptr(ptr));
}

bool Pocket::collectmoneyptr(inventory &from_inv)
{
	const int itemcount = from_inv.Count_Items(IS_MONEY, -1, -1, true);

	if (!itemcount)
		return false;

	from_inv.collectmoneypointers_recurse(this);

	return true;
}

void Pocket::Clear_Items()
{
	items.clear();
}

void Pocket::Show(int starting_index, int &lasttype, bool darklevel)
{
	if (Is_Empty())
	{
		print_text("<Empty>");
		return;
	}

	int itemcount=0; //keeps track of number of items displayed

	//move ahead in the location of the index and start displaying from that
	pitr ii = items.begin();
	for (int t=0; t<starting_index; t++)
	{
		if (ii==items.end()) return; //can't start from the end!
		++ii;
	}

	if (starting_index>0)
		display->More_Inventory(2);

	gotoxy(0, 3);

	//display items until the end of list or screen reached
	while (ii != items.end())
	{
		itemlistptr &iptr=(*ii);
		invnode *dispitem=iptr.ptr;

		if (dispitem->i.type != lasttype && !darklevel)
		{
			lasttype=dispitem->i.type;

			//show item category
			set_color(C_YELLOW);
			my_printf("%s ('%c')\n",
				gategories[lasttype].name, gategories[lasttype].out);
		}
		if (get_cursor_y() < SCREEN_LINES-3)
		{
			my_setcolor(CH_MAGENTA);
			if (iptr.sel)
				my_printf(" +");
			else
				my_printf("  ");

			my_setcolor(C_RED);
			my_printf(" %c", 'a'+itemcount);
			my_setcolor(C_WHITE);
			my_printf(") ");
			if (dispitem->slot >= 0)
			{
				my_setcolor(C_CYAN);
				equipment::display_description(dispitem->slot);
				my_setcolor(C_WHITE);
			}
			if (darklevel)
				my_printf("something");
			else
				display->Item_Info(dispitem, 0);

			my_printf("\n");
			itemcount++;
		}
		else
		{
			//end of screen reached
			display->More_Inventory(SCREEN_LINES-3);
			break;
		}

		++ii; //next item in the list
	}
}

int Pocket::Get_Selected_Amount()
{
	int total=0;

	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		itemlistptr &iptr=(*ii);
		if (iptr.sel) total++;
	}

	return total;
}

int Pocket::get_weight_of_items()
{
	int total=0;

	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		itemlistptr &iptr=(*ii);
		total+=iptr.ptr->Get_Weight();
	}

	return total;
}

bool Pocket::Is_Empty()
{
	return items.empty();
}

void Pocket::Add_Coins(Currency &c, inventory &to_inv)
{
	if (c.gold>0)
		to_inv.Add_Gold(c.gold);
	if (c.silver>0)
		to_inv.Add_Silver(c.silver);
	if (c.copper>0)
		to_inv.Add_Copper(c.copper);
}

void Pocket::money_transaction(inventory &from_inv, inventory &to_inv,
	equipment &gear, int copperneed, int copperamt)
{
	/* now we have one array with all money items (coin piles)
	 * The array must now be scanned and correct amount
	 * of money removed from the inventory.
	 *
	 */

	Currency c;

	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		if (copperneed<=0) break;

		itemlistptr &iptr=(*ii);
		invnode *change=iptr.ptr;

		if (change->i.group < mucho.num_valuables)
		{
			copperamt = valuables[change->i.group].value *
				change->count;

			/* always remove the source first */
			from_inv.Destroy_Item(gear, change, -1);

			if (copperamt == copperneed)
			{
				c.Coppervalue(copperamt);
				Add_Coins(c, to_inv);

				copperneed-=copperamt;
				copperamt=0;
			}
			else if (copperamt < copperneed)
			{
				c.Coppervalue(copperamt);
				Add_Coins(c, to_inv);

				copperneed-=copperamt;
				copperamt=0;
			}
			else
			{
				c.Coppervalue(copperneed);
				Add_Coins(c, to_inv);

				copperamt-=copperneed;
				copperneed=0;
			}

			/* if money still left, add it back to source inventory */
			if (copperamt > 0)
			{
				c.Coppervalue(copperamt);
				Add_Coins(c, from_inv);
			}
		}
	}
}

void Pocket::Transfer(inventory &from_inv, inventory &to_inv)
{
	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=from_inv.detach((*ii).ptr);
		if (n==0)
		{
			//this shouldn't happen if we have selected items from inventory
			msg.newmsg("Pocket source is empty.");
			return;
		}
		if ((*ii).sel)
			to_inv.Add_Item(n);
	}
}

void Pocket::Toggle(int index)
{
	int ci=0;
	
	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		if (ci==index)
		{
			bool s=(*ii).sel;

			if (s)
				(*ii).sel=false;
			else
				(*ii).sel=true;

			break;
		}

		ci++;
	}
}

invnode *Pocket::Remove_Next_Selected()
{
	if (Is_Empty())
		return 0;

	for (pitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		if ((*ii).sel)
		{
			invnode *n=(*ii).ptr;
			items.erase(ii);
			return n;
		}
	}

	return 0;
}
