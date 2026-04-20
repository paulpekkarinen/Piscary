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

#include "avatar.h"
#include "caves.h"
#include "being.h"
#include "dice.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "invnode.h"
#include "input.h"
#include "itempack.h"
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

using std::string;

void drop_here(invnode *src, level_type *level, int x, int y);
int getitem_inner(playerinfo *plr, level_type *level, invnode *lptr);
bool player_dropitem_inner(invnode *dropit);

bool drop_everything(
	inventory *inv, equipment &gear, level_type *level, int x, int y)
{
	bool rv=false;

	Coord c(x, y);

	for (;;)
	{
		invnode *n=inv->Get_Last_Item();
		if (n==0) break; //no more items to drop

		if (drop_item(inv, n, gear, level, -1, c)==0)
			break;

		rv=true; //at least one item dropped
	}

	return rv;
}

void drop_here(invnode *src, level_type *level, int x, int y)
{
	src->x=x;
	src->y=y;
	src->slot = -1;

	level->inv.Add_Item(src);
}

void drop_item(playerinfo *plr, level_type *level)
{
	const bool dropped = plr->backpack->Select_Items("Select items to drop!");

	if (dropped)
		drop_selected(plr->backpack->Get_Pocket(), level, plr->Get_Location());
}

invnode *drop_item(inventory *inv, invnode *in_src, equipment &gear,
	level_type *level, int count, const Coord &c)
{
	/* if item is equipped */
	if (in_src->slot!=-1)
		gear.clear_slot(in_src->slot);

	invnode *src=inv->remove_n_items(in_src, count);

	if (src == 0)
	{
		msg.newmsg("Error: drop_item() - item not found from this inventory!", CHB_RED);
		return 0;
	}

	drop_here(src, level, c.x, c.y);
	return src;
}

//drop monster's items and corpse etc. when it dies
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

	/* set coordinates */
	item->x=pos.x;
	item->y=pos.y;

	/* drop everything from monster inventory */
	drop_everything(&mons->inv, mons->equips, level, pos.x, pos.y);

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

	if (count==0)
	{
		msg.vnewmsg(C_WHITE, "Item %s %s was not taken.",
			materials[lptr->i.material].name, lptr->i.name.c_str());
		return 0;
	}

	if (count > lptr->count)
	{
		msg.vnewmsg(C_WHITE, "There are only %ld %s's!",
			lptr->count, lptr->i.name.c_str());

		count=lptr->count;
	}

	if (plr->Can_Carry(count * lptr->i.Get_Weight())==false)
	{
		if (count==1)
		{
			msg.vnewmsg(C_WHITE, "The %s %s weighs too much for you.",
				materials[lptr->i.material].name, lptr->i.name.c_str());
		}
		else
			msg.vnewmsg(C_WHITE, "Those %d %s %s's weight too much for you.",
				count, materials[lptr->i.material].name, lptr->i.name.c_str());
		return 0;
	}

	lptr = level->inv.remove_n_items(lptr, lptr->count);

	if (lptr)
	{
		if (lptr->i.status & ITEM_UNPAID)
			plr->bill += count * lptr->i.price;

		//this should work after previous checks
		plr->inv.Add_Item(lptr);

		display->Item_Info(&lptr->i, lptr->i.weight, count, "You took");
	}
	else
	{
		msg.newmsg("Error: Can't get item, item transfer failed!", CHB_RED);
		return -1;
	}

	return count;
}

//note: only called in drop_selected()

/* ** FIX **
 * note: THERE'S A BUG here if the actual DROP fails but the shopkeeper agrees
 * to buy the item. This is because shopkeeper will first buy the item and
 * then this routine tries to drop it -> if drop fails, the player has
 * got the money from shopkeeper and the item he/she just sold!
 * ** FIX **/
bool player_dropitem_inner(invnode *dropit)
{
	//   Tinvpointer dropit;
	//   Tinvpointer newitem;
	//   int32u oldw;

	bool sellmode=false;

	level_type *level=c_level;
	//   oldw=player.inv.weight;

	//   dropit=inv_listitems(&player.inv, "Drop what?", -1,
	//			  true, -1, -1);

	if (dropit)
	{
		if (dropit->slot >= 0)
		{
			msg.vnewmsg(C_YELLOW, "Item %s %s is in use, unequip it first!",
				materials[dropit->i.material].name, dropit->i.name.c_str());
			return true;
		}

		int count=dropit->count;
		if (dropit->count > 1 && !CONFIGVARS.droppiles)
		{
			Game.noticeevents(level);

			msg.vadd(C_GREEN, "How many %s's to drop [0..%ld, ENTER for all]?",
				dropit->i.name.c_str(), dropit->count);
			count=get_amount_of_items(dropit->count);

			if (count==0)
			{
				msg.newmsg("Nothing dropped.", C_WHITE);
				return true;
			}
			if (count > dropit->count)
			{
				msg.vnewmsg(C_WHITE, "But you only have %ld %s's.",
					dropit->count, dropit->i.name.c_str());

				count=dropit->count;
			}
		}

		msg.update();

		dropit = drop_item(&player.inv, dropit, player.equips, level,
			count, player.Get_Location());

		if (dropit)
		{
			if (dropit->i.status & ITEM_UNPAID)
				player.bill -= count * dropit->i.price;

			player.Spend_Time(TIME_DROPITEM);

			if (player.inroom>=0 && !(dropit->i.status & ITEM_UNPAID))
			{
				if (level->rooms[player.inroom].type == ROOM_SHOP)
				{
					Game.noticeevents(level);
					if (shopkeeper_buy(level, level->rooms[player.inroom].owner,
						dropit))
						sellmode=true;
				}
			}

			const char *stxt;
			if (sellmode)
				stxt="You just sold";
			else
				stxt="You drop";

			display->Item_Info(&dropit->i, dropit->i.weight, count, stxt);
		}
		else
			msg.newmsg("You kept the item.", C_WHITE);

		return true;
	}

	return false;
}

void drop_selected(Pocket &tasku, level_type *level, const Coord &c)
{
	//note: code routine to drop selected items (note: this could be in player class)
	//player_dropitem_inner();
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

	/* if there're multiple items, call up a inventory lister */
	/* otherwise get the item directly */
	if (i > 1)
	{
		SelectItemsGround level_items(level->inv, -1);

		const bool got = level_items.Select_Items("Select items to get!", pc);

		if (got)
		{
			Pocket &tasku=level_items.Get_Pocket();
			tasku.Transfer(level->inv, plr->inv);
		}
	}
	else
	{
		invnode *ptr=gameview.Get_Item(pc);
		if (ptr)
			getitem_inner(plr, level, ptr);
	}
}

/*
 * Shopkeeper routine which tries to drop unpaid items to the floor
 * Drops only ONE item and returns.
 */
bool shopkeeper_drop(level_type *level, being *keeper)
{
	invnode *i=keeper->inv.Remove_An_Unpaid_Item();

	if (!i)
		return false;

	drop_here(i, level, keeper->x, keeper->y);

	string s(keeper->m.name);
	s.append(" just put something new for sale");
	msg.add_dist(level, keeper->x, keeper->y, s.c_str(), C_GREEN, NULL, C_CYAN);

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
