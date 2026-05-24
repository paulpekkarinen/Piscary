/**************************************************************************
 * trade.cpp --                                                           *
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

//Refactored 14.7.2022 - 22.9.2025 by Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "currency.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "material.h"
#include "message.h"
#include "move.h"
#include "pocket.h"
#include "quote.h"
#include "trade.h"

using std::string;

bool inv_paymoney(inventory &from_inv, inventory &to_inv, equipment &gear,
	int copperneed, bool checkonly)
{
	const int copperamt = from_inv.Get_Wealth();

	/* if not enough money, return false */
	if (copperamt < copperneed)
		return false;

	/* check only if money is enough */
	if (checkonly)
		return true;

	/* build pointer list from money items */
	Pocket dough;
	dough.collectmoneyptr(from_inv);

	if (dough.Is_Empty())
		return false;

	//make the transaction
	dough.money_transaction(from_inv, to_inv, gear, copperneed, copperamt);

	return true;
}

void paybill(playerinfo *plr, level_type *level)
{
	if (!plr->bill)
	{
		msg.newmsg("You don't have any debts.", C_WHITE);
		return;
	}

	const int pinroom=plr->In_Room();

	if (pinroom<0)
	{
		msg.newmsg("You're not in a shop.", C_WHITE);
		return;
	}

	if (level->rooms[pinroom].type != ROOM_SHOP)
	{
		msg.newmsg("You are in a room, but this is not a shop.", C_WHITE);
		return;
	}

	if (level->rooms[pinroom].owner == 0)
	{
		msg.newmsg("There's no one to pay your bill to.", C_WHITE);
		return;
	}

	being *mptr=0;
	Coord pc;

	for (int i=1; i<10; i++)
	{
		pc=move_to_direction(i, plr);

		mptr=gameview.Get_Monster(pc);
		if (mptr!=0)
		{
			if (mptr->m.status & MST_SHOPKEEPER)
				break;
			else
				mptr=0;
		}
	}

	if (!mptr)
	{
		msg.newmsg("Shopkeeper says: \"Would you please come closer first!\".", C_YELLOW);
		return;
	}

	Currency rupees(plr->bill);
	string s(do_bay_bill_to(mptr, rupees));

	if (!confirm_yn(s.c_str(), false, true))
		return;

	/* transfer the money */
	if (inv_paymoney(plr->inv, mptr->inv, plr->equips, plr->bill, false))
	{
		msg.newmsg("You've paid your bill!", C_RED);
	}
	else
	{
		msg.newmsg("But you don't have enough money!", C_MAGENTA);
		return;
	}

	/* mark items as normal */
	plr->inv.mark_normal();
	plr->bill=0;
}

bool shopkeeper_buy(level_type *level, being *keeper, invnode *item)
{
	Shoppe &myshop=level->rooms[keeper->roomnum].kauppa;

	if (!keeper)
	{
		msg.newmsg("There's nobody to pay you for that!", C_WHITE);
		return false;
	}
	/*
		 for(i=1; i<10; i++)
		 {
		  Coord pc=move_to_direction(i, player);
		  mptr=level->Get_Monster(pc);
		  if(mptr) {
			   if(mptr->m.status & MST_SHOPKEEPER) {
				break;
			   }
			   else
				mptr=NULL;
		  }
		 }

		 if(!mptr) {
		  msg.newmsg("Shopkeeper says: \"Please come closer so I can see the stuff you're selling!\".",
			C_YELLOW);
		  return false;
		 }
	*/

	const char *keepername=keeper->m.name.c_str();

	if (item->i.type == IS_MONEY)
	{
		msg.newmsg(C_MAGENTA,
			"%s laughs: \"Like you want to sell your money right?\".",
			keepername);
		return false;
	}
	if (item->i.type == IS_FOOD && item->i.group == FOOD_CORPSE)
	{
		msg.newmsg(C_MAGENTA,
			"%s grins: \"Yuck! I don't have so sick needs...\".",
			keepername);
		msg.newmsg(C_WHITE, "%s vomits.", keepername);
		return false;
	}

	if (myshop.Accept_Item(item)==false)
	{
		msg.newmsg(C_MAGENTA,
			"\"Take your stuff elsewhere. We don't buy that.\", %s says.",
			keepername);
		return false;
	}

	real copper=myshop.Get_Selling_Price(item);

	if (!inv_paymoney(keeper->inv, player.inv, keeper->equips,
		(int)copper, true))
	{
		string s("\"Sorry, I can't afford to pay for ");
		if (item->count==1)
			s.append("that\"");
		else
			s.append("those\"");

		//note: is itemname missing here?
		
		s.append(keeper->m.name);
		s.append(" says.");

		msg.newmsg(s, C_MAGENTA);
		return false;
	}

	Currency rupees((int)copper);
	string valuestr=get_coppervalue_string(rupees);

	string s(keeper->m.name);
	s.append(" asks: \"Do you want to sell ");

	if (item->count==1)
		s.append("that ");
	else
		s.append("those ");

	s.append(item->i.name);
	if (item->count>1) s.append("s");
	s.append(" for ");
	s.append(valuestr);
	s.append("?\"");

	if (confirm_yn(s.c_str(), false, true))
	{
		if (inv_paymoney(keeper->inv, player.inv, keeper->equips, (int32u)copper, false))
			msg.newmsg("Sold!", C_GREEN);

		myshop.Item_Sold(item);

		return true;
	}

	return false;
}
