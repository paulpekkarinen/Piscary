/**************************************************************************
 * pack.cpp --                                                            *
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

//Refactored 31.10.2023 - 26.3.2026 Paul K. Pekkarinen

#include <cstring>
#include "avatar.h"
#include "caves.h"
#include "being.h"
#include "dice.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "invnode.h"
#include "input.h"
#include "itemdata.h"
#include "itempack.h"
#include "lexicon.h"
#include "material.h"
#include "message.h"
#include "names.h"
#include "options.h"
#include "pack.h"
#include "pocket.h"
#include "script.h"
#include "selgro.h"
#include "selpack.h"
#include "trade.h"
#include "world.h"

using std::string;

int getitem_inner(playerinfo *plr, level_type *level, invnode *lptr);
bool player_did_drop(invnode *dropit);

bool player_autopickup(level_type *level, const Coord &c)
{
	if (strlen(CONFIGVARS.pickuptypes)==0)
		return false;

	//get topmost item here
	invnode *lptr=gameview.Get_Item(c);
	if (lptr==0) //check anyway
		return false;

	/* check capasity first */
	if (player.Can_Carry(lptr->Get_Weight())==false)
	{
		msg.newmsg("You tried to take an item, but it weights too much.",
			C_WHITE);
		return false;
	}

	/* don't get UNPAID items */
	if (lptr->i.status & ITEM_UNPAID)
		return false;

	if (my_stricmp(CONFIGVARS.pickuptypes, "all")!=0)
	{
		int j=0;

		/* check if the item exists in the pickup list */
		for (int i=0; i<(int)strlen(CONFIGVARS.pickuptypes); i++)
		{
			if (CONFIGVARS.pickuptypes[i] == gategories[lptr->i.type].out)
				j=1;
		}

		if (j==0)
			return false;
	}

	//get item for player
	lptr = level->inv.remove_n_items(lptr, lptr->count);
	if (lptr)
	{
		display->Item_Info(lptr, "You took");

		player.inv.Add_Item(lptr);

		gameview.Refresh_Item_Map(c);
	}
	else
	{
		msg.newmsg("Autopickup: Can't get item, no memory!", CHB_RED);
		return false;
	}

	return true;
}

//drop monster's items and corpse when it dies
void drop_loot(being *mons, level_type *level)
{
	int prob = 100;

	/* decide whether to generate a corpse or not */
	if (mons->m.weight < (4 * WEIGHT_KILO))
		prob = 5;
	else
		if (mons->m.weight < (10 * WEIGHT_KILO))
			prob = 20;
		else
			if (mons->m.weight < (30 * WEIGHT_KILO))
				prob = 40;
			else
				prob = 31+RANDU(70);

	if (throwdice(1, 100, 0) > prob)
		return;

	invnode *item=level->inv.Create_Item(
		Itempack(IS_FOOD, FOOD_CORPSE, 1, MAT_ORGANIC));

	if (!item)
	{
		msg.vnewmsg(CHB_RED, "Error: cannot generate a corpse.");
		return;
	}

	/* generate a corpse to the given coords */

	if (mons->m.desc.size()+7 <= ITEM_NAMEMAX)
	{
		item->i.name=mons->m.desc;
		item->i.name.append(" corpse");
	}
	else
		item->i.name="corpse";

	/* pmod2 will have the monster race */
	item->i.pmod2=mons->m.race;
	item->i.pmod4=0;
	item->i.icond=COND_NEW;

	/* I want to retain the special monster type for nice effects :-) */
	const int spmon=mons->Get_Special_Id();
	if (spmon!=0) item->i.pmod3=spmon;

	/* food items have their nutrition modifier in items 'pmod' field */
	item->i.pmod1=90;
	item->i.weight=mons->m.weight;

	const Coord pos=mons->Get_Location();

	/* drop everything from monster inventory */
	bool dropped=false;
	for (;;)
	{
		invnode *n=mons->inv.Get_Last_Item();
		if (n==0) break; //no more items to drop

		mons->Drop_Item(n, n->count, pos);

		dropped=true; //at least one item dropped
	}

	if (dropped)
		gameview.Refresh_Item_Map(pos);

	/* clean the room owners, shopkeepers mainly */
	/* go trough the room array and remove owner pointer 'mons' */
	if (mons->Is_Shopkeeper())
	{
		mons->inv.clean_shop_items(level, mons);
		level->remove_room_owner(mons);
	}
}

int getitem_inner(playerinfo *plr, level_type *level, invnode *lptr)
{
	int count=1;

	if (lptr->count > 1)
	{
		Game.noticeevents(level);

		if (lptr->i.type == IS_MONEY && CONFIGVARS.getallmoney)
		{
			count=lptr->count;
		}
		else
		{
			msg.vadd(C_GREEN, "How many %s's to get [0..%ld, ENTER for all]?",
				lptr->i.name.c_str(), lptr->count);
			count=get_amount_of_items(lptr->count);
		}
	}

	const char *matname=lptr->Get_Material_Name();

	if (count==0)
	{
		msg.vnewmsg(C_WHITE, "Item %s %s was not taken.",
			matname, lptr->i.name.c_str());
		return 0;
	}

	if (count > lptr->count)
	{
		msg.vnewmsg(C_WHITE, "There are only %ld %s's!",
			lptr->count, lptr->i.name.c_str());

		count=lptr->count;
	}

	if (plr->Can_Carry(count * lptr->Get_Weight_Of_One())==false)
	{
		if (count==1)
		{
			msg.vnewmsg(C_WHITE, "The %s %s weighs too much for you.",
				matname, lptr->i.name.c_str());
		}
		else
			msg.vnewmsg(C_WHITE, "Those %d %s %s's weight too much for you.",
				count, matname, lptr->i.name.c_str());
		return 0;
	}

	lptr = level->inv.remove_n_items(lptr, lptr->count);

	if (lptr)
	{
		if (lptr->i.status & ITEM_UNPAID)
			plr->bill += count * lptr->i.price;

		display->Item_Info(lptr, count, "You took");

		//this should work after previous checks
		plr->inv.Add_Item(lptr);
	}
	else
	{
		msg.newmsg("Error: Can't get item, item transfer failed!", CHB_RED);
		return -1;
	}

	return count;
}

/* ** FIX **
 * note: THERE'S A BUG here if the actual DROP fails but the shopkeeper agrees
 * to buy the item. This is because shopkeeper will first buy the item and
 * then this routine tries to drop it -> if drop fails, the player has
 * got the money from shopkeeper and the item he/she just sold!
 * note: does the drop ever fail?
 * ** FIX **/
//Returns true if item was ready to drop, but don't move it to level here.
bool player_did_drop(invnode *dropit)
{
	bool sellmode=false;

	level_type *level=world->Get_Current_Level();

	if (dropit->slot >= 0)
	{
		msg.vnewmsg(C_YELLOW, "Item %s %s is in use, unequip it first!",
			dropit->Get_Material_Name(), dropit->i.name.c_str());
		return false;
	}

	int count=dropit->count;

	if (count > 1 && !CONFIGVARS.droppiles)
	{
		Game.noticeevents(level);

		msg.vadd(C_GREEN, "How many %s's to drop [0..%d, ENTER for all]?",
			dropit->i.name.c_str(), count);
		count=get_amount_of_items(count);

		if (count==0)
		{
			msg.newmsg("Nothing dropped.", C_WHITE);
			return false;
		}

		if (count > dropit->count)
		{
			msg.vnewmsg(C_WHITE, "But you only have %d %s's.",
				dropit->count, dropit->i.name.c_str());

			count=dropit->count;
		}
	}

	msg.update();

	if (dropit->i.status & ITEM_UNPAID)
		player.bill -= count * dropit->i.price;

	player.Spend_Time(TIME_DROPITEM);

	const int pr=player.In_Room();

	if (pr!=-1 && !(dropit->i.status & ITEM_UNPAID))
	{
		if (level->rooms[pr].type == ROOM_SHOP)
		{
			Game.noticeevents(level);
			if (shopkeeper_buy(level, level->rooms[pr].owner, dropit))
				sellmode=true;
		}
	}

	const char *stxt;
	if (sellmode)
		stxt="You just sold";
	else
		stxt="You drop";

	display->Item_Info(dropit, count, stxt);

	return true;
}

void drop_selected(playerinfo *plr)
{
	const int v = plr->backpack->Select_Items("Select items to drop!");

	if (v!=Stockpile::Selected)
		return;

	Pocket &tasku=plr->backpack->Get_Pocket();
	const Coord c=plr->Get_Location();

	int amt=0;
	for (;;)
	{
		invnode *i=tasku.Remove_Next_Selected();
		if (i==0)
			break;

		if (player_did_drop(i))
		{
			player.Drop_Item(i, i->count, c);
			amt++;
		}
	}

	if (amt>0)
		gameview.Refresh_Item_Map(c);
}

void pick_up_item(playerinfo *plr, level_type *level)
{
	Coord pc=plr->Get_Location();
	const int i=gameview.Count_Items(pc);

	if (i==0)
	{
		msg.newmsg("You fill your pockets with dust...", C_WHITE);
		return;
	}

	int picked_up=0 ; //how many items picked up

	/* if there're multiple items, call up a inventory lister */
	/* otherwise get the item directly */
	if (i > 1)
	{
		SelectItemsGround level_items(level->inv, -1);

		const int got = level_items.Select_Items("Select items to get!", pc);

		if (got==Stockpile::Selected)
		{
			Pocket &tasku=level_items.Get_Pocket();
			tasku.Transfer(level->inv, plr->inv);
		}
	}
	else
	{
		invnode *ptr=gameview.Get_Item(pc);
		if (ptr)
			picked_up=getitem_inner(plr, level, ptr);
	}

	if (picked_up>0)
		gameview.Refresh_Item_Map(pc);
}

/*
 * Shopkeeper routine which tries to drop unpaid items to the floor
 * Drops only ONE item and returns.
 */
bool shopkeeper_drop(level_type *level, being *keeper)
{
	invnode *i=keeper->inv.Remove_An_Unpaid_Item();

	if (i==0)
		return false;

	const Coord &c=keeper->Get_Location();
	gameview.Land_Item(i, c);
	gameview.Refresh_Item_Map(c);

	string s(keeper->m.name);
	s.append(" just put something new for sale");
	msg.add_dist(level, c.x, c.y, s.c_str(), C_GREEN, NULL, C_CYAN);

	return true;
}

bool shopkeeper_get(level_type *level, being *monster, invnode *itemptr)
{
	invnode *i=level->inv.Pick_Up(itemptr);

	//shopkeeper can carry infinite number of items
	monster->inv.Add_Item(i);

	if (gameview.Is_Visible(monster->Get_Location()))
	{
		string s=monster_sprintf(monster, true, true);
		s.append(" repositions some items.");

		msg.add_dist(level, monster->x, monster->y, s.c_str(), C_WHITE,
			texts->Get_Random(Script::Keeper_Unseen), C_WHITE);
	}
	return true;
}
