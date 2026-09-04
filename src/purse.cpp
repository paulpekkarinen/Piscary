/**************************************************************************
 * purse.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 4.9.2021 - 27.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "factory.h"
#include "game.h"
#include "input.h"
#include "invnode.h"
#include "itemdata.h"
#include "itempack.h"
#include "log.h"
#include "pocket.h"
#include "purse.h"
#include "storage.h"
#include "uncover.h"

using std::string;

//Recursive task operations.
enum Rec_Ops
{
	Delete_Items,
	Mark_Unpaid
};

inventory::inventory()
	: capasity(0), copper(0), weight(0)
{

}

inventory::~inventory()
{
	Clear();
}

void inventory::Calc_Recursive()
{
	weight=0;
	copper=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		//if item is a container, recurse and calculate it
		if (iptr->inv!=0)
		{
			iptr->inv->Calc_Recursive();
			weight += iptr->inv->weight;
			copper += iptr->inv->copper;
		}
		else weight += iptr->i.weight * iptr->count;

		if (iptr->i.type == IS_MONEY && iptr->i.group < mucho.num_valuables)
			copper += valuables[iptr->i.group].value * iptr->count;
	}
}

/*
** Build a list of item pointers which match to the filter. If filter -1
** then build a list of all items.
*/
bool inventory::builditemarray(Pocket &ptrlist, int filter, int x, int y)
{
	const int itemcount = Count_Items(filter, x, y, false);

	if (itemcount==0)
		return false;

	ptrlist.Clear_Items(); //clear before adding items

	/* collect pointers to items, now knows about filtered item type too :) */
	int amt=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *invptr=(*ii);

		if (invptr->i.type != IS_CONTAINER)
		{
			if (invptr->i.type != filter && filter>=0)
				continue;
		}

		//if location is -1, -1 it's from backpack, otherwise
		//check location match on the map
		if (x>=0 && y>=0)
		{
			if (!(x==invptr->x && y==invptr->y))
				continue;
		}

		ptrlist.Push_Item(invptr);
		amt++;
	}

	/* if no filter matches, return false */
	if (amt==0)
		return false;

	return true;
}

void inventory::clean_shop_items(level_type *level, being *owner)
{
	int32u mask=0xffffffff ^ ITEM_UNPAID;

	/* mark all items inside the owners room PAID */
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);
		Coord ic=iptr->Get_Location();

		if (level->rooms[owner->roomnum].Encloses(ic))
			iptr->i.status &= mask;
	}
}

void inventory::Clear()
{
	recursive_task(Delete_Items);

	items.clear();
	weight=0;
	copper=0;
}

int inventory::Get_Linear_Size() const
{
	return (int)items.size();
}

void inventory::Add_Copper(int amount)
{
	Create_Item(Itempack(IS_MONEY, MONEY_COPPER, amount, -1));
}

void inventory::Add_Gold(int amount)
{
	Create_Item(Itempack(IS_MONEY, MONEY_GOLD, amount, -1));
}

void inventory::Add_Item(invnode *new_item)
{
	items.push_back(new_item);
	recalculate();
}

void inventory::Add_Silver(int amount)
{
	Create_Item(Itempack(IS_MONEY, MONEY_SILVER, amount, -1));
}

//note: this is unused, probably because it would be slow (if called per turn)
void inventory::age_food_items()
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=(*ii);

		n->i.Age_Food(player.stat[STAT_LUC].Get());
		if (n->inv!=0)
			n->inv->age_food_items();
	}
}

invnode *inventory::Create_Item(const Itempack &ip)
{
	invnode *inode=factory.New_Item(ip);
	if (inode==0)
	{
		diary.Write("Failed to create an item in inventory.");
		return 0;
	}

	Add_Item(inode);
	return inode;
}

void inventory::collectmoneypointers_recurse(Pocket *ptrlist)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		if (iptr->i.type == IS_MONEY)
		{
			ptrlist->Push_Item(iptr);
		}
		else if (iptr->inv!=0)
		{
			iptr->inv->collectmoneypointers_recurse(ptrlist);
		}
	}
}

//Even though this has the location, it has to be in the base class because
//items can have inventory component.
int inventory::Count_Items(int type, int x, int y, bool recursive)
{
	int total=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *invptr=(*ii);

		if (invptr->i.type != IS_CONTAINER)
		{
			if (invptr->i.type != type && type>=0)
				continue;
		}

		if (x>=0 && y>=0)
		{
			if (!(x==invptr->x && y==invptr->y))
				continue;
		}

		total++;

		/* do we want the count from all inventory levels ?*/
		if (invptr->inv!=0 && recursive)
			total += invptr->inv->Count_Items(type, -1, -1, true);
	}

	return total;
}

int inventory::Destroy_Item(equipment &gear, invnode *remthis, int count)
{
	if (count==0)
		return 0;

	if (remthis==0) return -1; //if trying to remove NULL node

	/* if item is equipped */
	if (remthis->slot!=-1)
		gear.clear_slot(remthis->slot);

	invnode *ptr=remove_n_items(remthis, count);
	if (ptr==0) return -1;

	delete ptr;

	recalculate();

	return count;
}

void inventory::destroy_item(equipment &gear, invnode *src)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=(*ii);
		if (n==src)
		{
			Destroy_Item(gear, n, n->count);
			break;
		}

		//go to sub-inventory recursively
		if (n->inv!=0)
		{
			n->inv->destroy_item(gear, src);
		}
	}
}

void inventory::destroy_one_stacked_item(equipment &gear, invnode *src)
{
	src->count--;
	if (src->count<=0)
		destroy_item(gear, src);
}

invnode *inventory::detach(invnode *src)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=(*ii);
		if (n==src)
		{
			items.erase(ii);
			recalculate();
			return n;
		}
		//go to sub-inventory recursively
		if (n->inv!=0)
		{
			invnode *sub=n->inv->detach(src);
			if (sub!=0)
				return sub;
		}
	}

	return 0;
}

/* Routine to find a best item type of "type" contained in inventory */
invnode *inventory::Find_Best_Item(int type, int group)
{
	bool better;
	invnode *best=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *invptr=(*ii);

		better=false;
		if (invptr->i.type == type)
		{
			if (group>=0)
			{
				if (invptr->i.group == group)
					return invptr;
			}
			else
			{
				if (best==0)
					better=true;
				else
				{
					if (invptr->Rate() > best->Rate())
						better=true;
				}
				if (better)
					best=invptr;
			}
		}
	}

	return best;
}

invnode *inventory::Find_By_Slot_Id(int id)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		if (iptr->slot==id) return iptr; //found it!

		//if item is a container, recursive jump to it
		if (iptr->inv!=0)
		{
			iptr->inv->Find_By_Slot_Id(id);
		}
	}

	return 0;
}

invnode *inventory::Find_Item(invnode *src)
{
	//check if 'src' is in this inventory
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=(*ii);
		if (n==src)
			return n;

		//if item is a container, recursive jump to it
		if (n->inv!=0)
		{
			invnode *sub=n->inv->Find_Item(src);
			if (sub!=0)
				return sub;
		}
	}

	return 0;
}

invnode *inventory::Get_Last_Item()
{
	if (items.empty()) return 0;
	return items.back();
}

void inventory::mark_normal()
{
	//this flips unpaid items to normal items...
	mark_unpaid();
}

void inventory::mark_unpaid()
{
	recursive_task(Mark_Unpaid);
}

bool inventory::ready_newmissile(equipment &e, int type, int group)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		if (type==iptr->i.type && group==iptr->i.group)
		{
			/* split a piles */
			iptr = remove_one_item(iptr);
			iptr->slot = EQUIP_MISSILE;

			e.equip[EQUIP_MISSILE].in_use=1;
			e.equip[EQUIP_MISSILE].item=iptr;

			return true;
		}
	}

	return false;
}

void inventory::recalculate()
{
	Calc_Recursive();

	if(this == &player.inv)
		GAME_NOTIFYFLAGS |= GAME_MONEYCHG;
}

void inventory::recursive_task(int op)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		if (iptr->inv!=0)
			iptr->inv->recursive_task(op);

		switch (op)
		{
			case Delete_Items: delete iptr; break;
			case Mark_Unpaid:
				if (iptr->i.status & ITEM_UNPAID)
					iptr->i.status ^= ITEM_UNPAID;
			break;
			default: break;
		}
	}
}

invnode *inventory::remove_n_items(invnode *src, int count)
{
	invnode *i=Find_Item(src);
	if (i==0) return 0;

	if (count==-1 || count>src->count)
		count=src->count;

	const int result=src->count-count;

	//everything moved, return the original item
	if (result==0)
		return detach(src);

	//create clone of item with count that was taken
	invnode *ni=factory.New_Item_From(src, count);
	if (ni!=0)
	{
		src->count-=count;
		recalculate();
	}

	return ni;
}

invnode *inventory::remove_one_item(invnode *src)
{
	return remove_n_items(src, 1);
}

invnode *inventory::Remove_An_Unpaid_Item()
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);
		if (iptr->i.status & ITEM_UNPAID) //if unpaid, return it
		{
			return detach(iptr);
		}
	}
	return 0;
}

void inventory::List_Items()
{
	static int xpos=0;
	static int ypos=0;
	static int rec=0;
	static int amt=0;

	if (rec==0) //reset if at first recursion level
	{
		xpos=0;
		ypos=0;
		amt=0;
	}

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);

		uncover.Inventory_Item(iptr, amt, xpos, ypos);
		amt++;

		if (list_more(ypos)==false)
			break;

		//if item is a container, recursively list items in it
		if (iptr->inv!=0)
		{
			rec++;
			xpos+=5;
			iptr->inv->List_Items();
			xpos-=5;
		}
	}

	rec=0;
}

void inventory::save(Tar_Ball &tb)
{
	tb.Put(Get_Linear_Size());

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);
		iptr->Save(tb);

		/* if item is a container, recursively save it */
		if (iptr->inv!=0)
			iptr->inv->save(tb);
	}
}

void inventory::load(Tar_Ball &tb)
{
	const int linear_amt=tb.Get_Next_Value();

	for (int i=0; i<linear_amt; i++)
	{
		invnode *iptr=new invnode;
		iptr->Load(tb);
		items.push_back(iptr);

		/* if item is a container, recursively load it */
		if (iptr->inv!=0)
			iptr->inv->load(tb);
	}

	recalculate();
}
