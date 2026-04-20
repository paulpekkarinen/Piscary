/**************************************************************************
 * consume.cpp --                                                         *
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
#include "codex.h"
#include "consume.h"
#include "foodstat.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "inventor.h"
#include "invnode.h"
#include "material.h"
#include "message.h"
#include "script.h"
#include "selgro.h"
#include "selpack.h"
#include "specmon.h"

using std::string;

void eat_food(playerinfo *plr, level_type *level)
{
	/* check if player can eat first */
	if (plr->nutr > FOOD_SATIATED)
	{
		msg.newmsg("You're too full to eat now.", C_WHITE);
		return;
	}

	const Coord pc=plr->Get_Location();

	const int sinv=inv_selectsource(pc.x, pc.y, IS_FOOD);
	if (sinv==inventory::Nothing)
	{
		msg.newmsg("Sorry, you've no food on the ground or in your backpack!",
			C_WHITE);
		return;
	}

	invnode *eatit=0;

	if (sinv == inventory::Ground)
	{
		//check level for food
		const int count=gameview.Count_Items(pc);

		if (count==1)
		{
			eatit=gameview.Get_Item(pc);

			string s;
			if (eatit->i.group==FOOD_CORPSE)
				s="Do you really want to eat this dead ";
			else
				s="Do you want to eat this ";

			s.append(eatit->i.name);

			if (!confirm_yn(s.c_str(), true, true))
				return;
		}
		else
		{
			if (count>1)
			{
				SelectItemsGround level_items(level->inv, IS_FOOD);

				eatit=level_items.Get_Handle("What do you want to eat?", pc);
			}
		}
	}
	else
	{
		/* browse the inventory normally */
		plr->backpack->Set_Filter(IS_FOOD); //get only food
		eatit=plr->backpack->Get_Handle("What do you want to eat?");
	}

	/* if eatit has a pointer, then we're gonna eat the bloody pointer :) */
	if (eatit)
	{
		if (eatit->slot>=0)
		{
			msg.newmsg("Please, unequip it first.", C_WHITE);
			return;
		}

		if (eatit->i.type == IS_FOOD)
		{
			/* eat */
			if (eatit->i.status & ITEM_UNPAID)
			{
				msg.newmsg(texts->Get_Random(Script::Unpaid_Item), C_MAGENTA);
				return;
			}

			Game.Eat_Passturns(level, &eatit->i);
			if (eatit->i.pmod3)
				eat_specialmonster(level, eatit->i.pmod3);

			//where to destroy the item that was eaten
			if (sinv == inventory::Ground)
				level->inv.destroy_one_stacked_item(plr->equips, eatit);
			else
				plr->inv.destroy_one_stacked_item(plr->equips, eatit);
		}
		else
			msg.vnewmsg(CH_RED, "You can't eat that, for gods sake "
				"it's made out of %s.",
				materials[eatit->i.material].name);
	}
}
