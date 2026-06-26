/**************************************************************************
 * melee.cpp --                                                           *
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

//Refactored 14.7.2022 - 11.4.2026 by Paul K. Pekkarinen

#include <format>
#include "avatar.h"
#include "being.h"
#include "body.h"
#include "caves.h"
#include "dice.h"
#include "damage.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "items.h"
#include "melee.h"
#include "message.h"
#include "names.h"
#include "tactics.h"

using std::format;
using std::string;

bool meleeinner(
	playerinfo *plr, level_type *level, invnode *invptr, int damage, being *mptr);
bool monster_meleeinner(
	being *mptr, level_type *level, invnode *invptr, Actor *target);

void meleeattack(playerinfo *plr, level_type *level, being *mptr)
{
	bool doattack=false;

	item_def *iptr;

	int damage=0, dmod=0;
	int dices=0, dicet=0;

	bool lefthand, righthand;

	if (!(mptr->m.status & MST_ATTACKMODE) &&
		gameview.Is_Visible(mptr->Get_Location()))
	{
		string s("Really attack ");
		s.append(monster_sprintf(mptr, false, false));

		doattack=confirm_yn(s.c_str(), false, true);
	}
	else doattack=true;

	if (doattack)
	{
		/* decide weapon to use in attack */
		plr->equips.decide_meleeweapon(&lefthand, &righthand);

		if (lefthand)
		{
			/* subtract time taken by attack */
			plr->Spend_Time(TIME_MELEEATTACK);

			invnode *invptr=plr->equips.get_inventory_item(EQUIP_LHAND);
			iptr=&invptr->i;

			dmod=iptr->meldam_mod;
			dices=iptr->melee_ds;
			dicet=iptr->melee_dt;

			damage=throwdice(dicet, dices, dmod);

			if (meleeinner(plr, level, invptr, damage, mptr))
				return;
		}

		if (righthand)
		{
			if (plr->Has_Time_For(TIME_MELEEATTACK)==false)
				return;

			plr->Spend_Time(TIME_MELEEATTACK);

			invnode *invptr=plr->equips.get_inventory_item(EQUIP_RHAND);
			iptr=&invptr->i;

			dmod=iptr->meldam_mod;
			dices=iptr->melee_ds;
			dicet=iptr->melee_dt;

			damage=throwdice(dicet, dices, dmod);
			if (meleeinner(plr, level, invptr, damage, mptr))
				return;
		}

		/* do hand damage if no items */
		if (!righthand && !lefthand)
		{
			plr->Spend_Time(TIME_MELEEATTACK);
			dmod=0;
			dices=2;
			dicet=2;

			damage=throwdice(dicet, dices, dmod);
			msg.newmsg("Hand attack!", C_RED);

			if (meleeinner(plr, level, 0, damage, mptr))
				return;
		}
	}
}

/* inner routine for player melee attack
here decide what bodypart to hit, if hit at all etc*/
bool meleeinner(
	playerinfo *plr, level_type *level, invnode *invptr, int damage, being *mptr)
{
	item_def *eqptr;

	//check if has equipment in hand
	if (invptr!=0)
		eqptr=&invptr->i;
	else
		eqptr=0;

	if (eqptr)
	{
		/* check if the item broke completely */
		if (eqptr->Age_Weapon(plr->stat[STAT_LUC].Get(), true)==COND_SMASHED)
		{
			if (eqptr->type==IS_LIGHT)
				plr->light=1;

			plr->inv.Destroy_Item(plr->equips, invptr, -1);

			return false;
		}
	}

	/* collect target bodyparts which the race has */
	int bparts[HPSLOT_MAX+1]={0}; //array for target bodyparts
	int i, j;
	for (j=0, i=0; i<HPSLOT_MAX; i++)
	{
		if (npc_races[mptr->m.race].bodyparts[i]>=0)
			bparts[j++]=i;
	}

	/* get a random target bodypart */
	int tslot=bparts[RANDU(j)];

	//   inttotal=calculate_meleehit(iptr, player.skills, get_stat( &player.stat[STAT_LUC]),
	//npc_races[mptr->m.race].bodyparts[tslot], player.tactic );

	int inttotal=plr->Calculate_Meleehit(eqptr, mptr, tslot);

	/* 1..100 */
	int hitresult=1+RANDU(100);

	damage+=tactics_data[plr->tactic].dam;
	if (damage<0)
		damage=0;

	if (hitresult > inttotal)
	{
		damage=0;
		msg.newmsg("You miss.", C_WHITE);
	}
	else
	{
		/* normal hits give 1 learning mark */
		i=1;

		int crit=plr->skills.check(SKILLGRP_GENERIC, SKILL_FINDWEAKNESS);
		crit=crit/4;

		if (throwdice(1, 100, 0) <= crit)
		{
			i=4;
			msg.newmsg("That was a critical hit(2xDAM)!", CH_WHITE);
			damage+=damage;
		}

		/* now increase melee weaponskills learning counter */
		plr->skills.melee_learnskills(eqptr, i);
	}

	if (damage==0) return false;

	if (damage_issue(level, mptr, plr, ELEMENT_NOTHING, damage, tslot, NULL)<=0)
	{
		plr->Killedmonster(mptr);
		return true;
	}

	return false;
}

void monster_meleeattack(being *mptr, level_type *level, Actor *target)
{
	bool lefthand, righthand;

	/* decide weapon to use in attack */
	mptr->equips.decide_meleeweapon(&lefthand, &righthand);

	if (lefthand)
	{
		/* subtract time taken by attack */
		mptr->Spend_Time(TIME_MELEEATTACK);

		invnode *iptr=mptr->equips.get_inventory_item(EQUIP_LHAND);
		if (monster_meleeinner(mptr, level, iptr, target))
			return;
	}

	if (righthand)
	{
		if (mptr->Has_Time_For(TIME_MELEEATTACK)==false)
			return;

		mptr->Spend_Time(TIME_MELEEATTACK);

		invnode *iptr=mptr->equips.get_inventory_item(EQUIP_RHAND);
		if (monster_meleeinner(mptr, level, iptr, target))
			return;
	}

	/* do hand damage if no items */
	if (!righthand && !lefthand)
	{
		mptr->Spend_Time(TIME_MELEEATTACK);

		if (monster_meleeinner(mptr, level, 0, target))
			return;
	}
}

bool monster_meleeinner(
	being *mptr, level_type *level, invnode *invptr, Actor *target)
{
	int i, j;

	/* array for target bodyparts */
	int bparts[HPSLOT_MAX+1]={0};

	item_def *eqptr;

	if (invptr!=0)
		eqptr=&invptr->i;
	else
		eqptr=0;

	const int monrace=mptr->m.race;

	int damage;
	if (eqptr)
	{
		/* check if the item broke */ //note: are we checking it?
		damage = throwdice(eqptr->melee_dt,
			eqptr->melee_ds,
			eqptr->meldam_mod);
	}
	else
	{
		damage = throwdice(npc_races[monrace].dam_dt,
			npc_races[monrace].dam_ds,
			npc_races[monrace].dam_mod);
	}

	/* targetting monster or player */
	string targetname;
	int trace=target->Get_Race();
	const bool plr=target->Is_Player();

	if (plr)
		targetname="you";
	else
		targetname=monster_sprintf(target, false, true);

	/* collect target bodyparts which the race has */
	for (j=0, i=0; i<HPSLOT_MAX; i++)
	{
		if (npc_races[trace].bodyparts[i]>=0)
		{
			bparts[j++]=i;
		}
	}

	/* get a random target bodypart */
	int tslot=bparts[RANDU(j)];
	Bodypart part(tslot);
	const char *partname=part.Get_Name();

	int inttotal=mptr->Calculate_Meleehit(eqptr, target, tslot);

	real hittotal=(real)inttotal;

	/* throw the dice */
	int hitresult=throwdice(1, 100, 0);

	/* add tactic effect damage */
	damage+=tactics_data[mptr->tactic].dam;
	if (damage<0)
		damage=0;

	string hitbonustxt;
	const int x=mptr->x;
	const int y=mptr->y;

	/* get attacker name */
	string attackername;
	if (gameview.Is_Visible(x, y))
		attackername=monster_sprintf(mptr, true, true);
	else
		attackername="It";

	if (hitresult <= hittotal)
	{
		if (hitresult > (hittotal*0.95))
		{
			hitbonustxt=" hard";
			damage+=throwdice(2, 3, 0);
		}
		else if (hitresult == inttotal)
		{
			hitbonustxt=" critically";
			damage+=damage;
		}
	}
	else
	{
		if (plr)
		{
			msg.newmsg(C_WHITE, "%s misses your %s.",
				attackername.c_str(), partname);
		}
		else
		{
			string s(attackername);
			s.append(" misses ");
			s.append(targetname);
			msg.add_dist(level, x, y, s.c_str(), C_WHITE,
				NULL, C_WHITE);
		}
		return false;
	}

	string actiontxt;

	if (npc_races[monrace].behave & BEHV_ANIMAL)
	{
		if (npc_races[monrace].attacktypes & ATTACK_BITE)
			actiontxt="bites";
		else if (npc_races[monrace].attacktypes & ATTACK_KICK)
			actiontxt="kicks";
		else
			actiontxt="hits";
	}
	else
		actiontxt="hits";

	string dmgmess;
	if (!damage)
	{
		if (plr)
		{
			msg.newmsg(C_WHITE, "%s %s your %s with no damage.",
				attackername.c_str(), actiontxt.c_str(), partname);
		}
		else
		{
			string s=format("{}{} the {} of {} with no damage.",
				attackername, actiontxt, partname, targetname);

			msg.add_dist(level, x, y, s.c_str(), C_WHITE,
				NULL, C_WHITE);
		}
		return false;
	}
	else
	{
		if (plr)
		{
			dmgmess=format("{} {} you{}",
				targetname, actiontxt, hitbonustxt);
		}
		else
		{
			dmgmess=format("{} {} {}{}",
				attackername, actiontxt, targetname, hitbonustxt);
		}
	}

	int thp=0;
	if (plr)
	{
		thp=damage_issue(level, target, mptr,
			ELEMENT_NOTHING, damage, tslot, dmgmess.c_str());
	}
	else
	{
		if (gameview.Is_Visible(x, y)==false)
			dmgmess="Something is fighting here.";

		if ((1+RANDU(100)) > 80)
			msg.add_dist(level, x, y, dmgmess.c_str(), C_RED,
				"You hear angry noises!", C_WHITE);

		thp=damage_issue(level, target, mptr, ELEMENT_NOTHING, damage, tslot,
			dmgmess.c_str());
	}

	if (thp<=0)
		mptr->Gain_Experience(target->Experience_Points_Earned());

	return true;
}
