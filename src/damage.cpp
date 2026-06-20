/**************************************************************************
 * damage.cpp --                                                          *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored in 13.7.2021 - 19.10.2025 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include "avatar.h"
#include "being.h"
#include "body.h"
#include "caves.h"
#include "condit.h"
#include "damage.h"
#include "game.h"
#include "gameview.h"
#include "invnode.h"
#include "lexicon.h"
#include "message.h"
#include "names.h"
#include "pack.h"

using std::string;

void damage_checkbodyparts(level_type *level, Actor *mptr)
{
	int slot;
	const bool is_player=mptr->Is_Player();

	string monsname;
	if (is_player==false)
		monsname=monster_sprintf(mptr, true, true);

	Equipslot *equip=mptr->equips.equip;
	hpslot *hpptr=mptr->hpp;
	int race=mptr->Get_Race();
	Coord pc=mptr->Get_Location();
	Gender gen(mptr->m.gender);

	/* when head falls below or to zero, the creature SHOULD die! */
	if (hpptr[HPSLOT_HEAD].cur <= 0)
		mptr->Death();

	/* when body falls below or to zero, the creature SHOULD die! */
	if (hpptr[HPSLOT_BODY].cur <= 0)
		mptr->Death();

	/* lefthand and righthand should be disabled and weapons dropped */
	if ((hpptr[HPSLOT_LEFTHAND].cur <= 0))
	{
		hpptr[HPSLOT_LEFTHAND].cur=0;

		if (equip[EQUIP_LHAND].status==EQSTAT_OK)
		{
			/* here must drop item from left hand */
			/* and disable that hand */
			equip[EQUIP_LHAND].status=EQSTAT_BROKEN;

			/* if damaged hand is reserved by another item drop the item
			   from the hand which reserved this slot */
			if (equip[EQUIP_LHAND].reserv)
				slot=equip[EQUIP_LHAND].reserv;
			else
				slot=EQUIP_LHAND;

			if (is_player)
			{
				msg.newmsg("Your left hand is disabled!",C_RED);
				player.conditions.add(CONDIT_BADLARM, 1);

				if (equip[slot].item)
				{
					string s("Your ");
					s.append(equip[slot].item->i.name);
					s.append(" fell down.");

					/* drop the item */
					mptr->Drop_Single_Item(equip[slot].item, pc);
					msg.newmsg(s, C_RED);
				}
			}
			else
			{
				mptr->conditions.add(CONDIT_BADLARM, 1);

				if (npc_races[mptr->m.race].behave & BEHV_FLYING)
				{
					string s=monsname;
					s.append(" drops to the ground.");

					msg.add_dist(level, pc.x, pc.y, s.c_str(), C_WHITE,
						 "You hear a \"splat.\".",C_WHITE);
					mptr->m.status|=MST_CANTMOVE;
				}
				if (equip[slot].item)
				{
					string s=monsname;
					s.append(" lost ");
					append_string_with(s, gen.Get_Art(3), ' ');
					append_string_with(s, equip[slot].item->i.name, '.');

					/* drop the item */
					mptr->Drop_Single_Item(equip[slot].item, pc);
					msg.add_dist(level, pc.x, pc.y, s.c_str(), C_RED, 0, CHB_CYAN);
				}
			}
		}
	}

	if ((hpptr[HPSLOT_RIGHTHAND].cur <= 0))
	{
		hpptr[HPSLOT_RIGHTHAND].cur=0;

		if (equip[EQUIP_RHAND].status==EQSTAT_OK)
		{
			/* here must drop item from left hand */
			/* and disable that hand */
			equip[EQUIP_RHAND].status=EQSTAT_BROKEN;

			/* if damaged hand is reserved by another item drop the item
			   from the hand which reserved this slot */
			if(equip[EQUIP_RHAND].reserv)
				slot=equip[EQUIP_RHAND].reserv;
			else
				slot=EQUIP_RHAND;

			if (is_player)
			{
				player.conditions.add(CONDIT_BADRARM, 1);
				msg.newmsg("Your right hand is disabled!",C_RED);
				if (equip[slot].item)
				{
					string s("Your ");
					s.append(equip[slot].item->i.name);
					s.append(" fell down.");

					/* drop it */
					mptr->Drop_Single_Item(equip[slot].item, pc);
					msg.newmsg(s, C_RED);
				}
			}
			else
			{
				mptr->conditions.add(CONDIT_BADRARM, 1);

				if(npc_races[mptr->m.race].behave & BEHV_FLYING)
				{
					string s=monsname;
					s.append(" drops to the ground.");
					msg.add_dist(level, pc.x, pc.y, s.c_str(), C_WHITE,
						 "You hear a distant \"splat.\".",C_WHITE);
					mptr->m.status|=MST_CANTMOVE;
				}

				if(equip[slot].item)
				{
					string s=monsname;
					s.append(" lost ");
					append_string_with(s, gen.Get_Art(3), ' ');
					append_string_with(s, equip[slot].item->i.name, '.');

					/* drop the item */
					mptr->Drop_Single_Item(equip[slot].item, pc);
					msg.add_dist(level, pc.x, pc.y, s.c_str(), C_RED, 0, CHB_CYAN);
				}
			}
		}
	}

	/* legs should stop working, no walking, fall to the ground etc. */
	if(hpptr[HPSLOT_LEGS].cur <= 0)
	{
		hpptr[HPSLOT_LEGS].cur=0;

		if(npc_races[race].bodyparts[HPSLOT_LEGS] < 0)
			return;

		if(equip[EQUIP_LEGS].status==EQSTAT_OK)
		{

			equip[EQUIP_LEGS].status=EQSTAT_BROKEN;

			if(!mptr)
			{
				player.conditions.add(CONDIT_BADLEGS, 1);
				msg.newmsg("Your legs are disabled!",C_RED);
				msg.newmsg("You fall to the ground.",CH_RED);
			}
			else
			{
				mptr->conditions.add(CONDIT_BADLEGS, 1);

				/* check for flying creatures */
				if(! (npc_races[mptr->m.race].behave & BEHV_FLYING))
				{
					string s=monsname;
					s.append(" falls to the ground.");
					msg.add_dist(level, pc.x, pc.y, s.c_str(), C_RED, NULL, CHB_CYAN);

					mptr->m.status|=MST_CANTMOVE;
				}
			}
		}
	}
}

int damage_issue(level_type *level, //note: need damage routine without attacker..
	Actor *target, Actor *attacker,
	int element, int damage, int bodypart,
	const char *message)
{
	//..so this check can be removed
	if (target==0)
		return 0;

	int i, st=0, ed=0;
	bool bodydam=false;
	int idam;

	if(bodypart<0 || bodypart>=HPSLOT_MAX)
	{
		st=0;
		ed=HPSLOT_MAX;
		bodydam=true;
	}
	else
	{
		bodydam=false;
		st=bodypart;
		ed=bodypart+1;
	}

	const bool is_plr=target->Is_Player();

	/* show the hit message */
	if(message!=0)
	{
		if (is_plr)
			msg.newmsg(message, C_RED);
		else
		{
			if (gameview.Is_Visible(target->x, target->y))
				msg.newmsg(message, C_RED);
		}
	}

	target->Damage_Message(damage, bodypart);

	/* issue damage */
	for(i=st; i<ed; i++)
	{
		if(bodydam)
		{
			idam=(int)(hp_bpmod[i] * (real)damage);
		}
		else
			idam=damage;

		target->Damage_Issue(element, idam, i);
	}

	/* return remaining hitpoints */
	if (is_plr==false)
	{
		target->Calculate_Totalhp();

		damage_checkbodyparts(level, target);

		if (target->Is_Alive())
		{
			/* lets make the monster really angry */
			if (attacker->Is_Player())
			{
				/* make angry only if sees the player */
				if (gameview.Is_Visible(target->x, target->y))
					target->Getangry(level, attacker, true);
			}
			else
				target->Getangry(level, attacker, false);

			target->Shouldflee(level);
		}

		return target->Get_Hit_Points();
	}
	else
	{
		GAME_NOTIFYFLAGS|=GAME_HPSPCHG;
		player.Checkstat(level, true, true);
		return player.Get_Hit_Points();
	}
}
