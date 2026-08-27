/**************************************************************************
 * actor.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 12.7.2022 - 24.5.2024 Paul K. Pekkarinen

#include "build.h"

#include "actor.h"
#include "creature.h"
#include "damage.h"
#include "dice.h"
#include "game.h"
#include "gameview.h"
#include "invnode.h"
#include "material.h"
#include "message.h"
#include "roleplay.h"
#include "storage.h"
#include "tactics.h"

#ifdef saladir_debug
#include "input.h"
#endif

Actor::Actor(int sp)
	: buffoon(sp)
{
	Reset();
}

void Actor::Advance_Level()
{
	Calculate_Totalhp();
	m.level++;
}

int Actor::Calc_Carryweight()
{
	/* carrying capasity = weight * (1+(stat_STR - 50)/100) */

	real tmp=(real)stat[STAT_STR].Get() - 50;
	tmp+=(real)(stat[STAT_TGH].Get() - 50)/2;
	tmp=(tmp / 100) + 1.0;

	real carry=(real)m.weight;

	return (int)(carry * tmp);
}

void Actor::Calculate_Totalhp()
{
	int total=0;

	for(int i=0; i<HPSLOT_MAX; i++)
	{
		total+=hpp[i].cur;
	}

	//use initialize to set both current and max value
	health.Initialize(total);
}

int Actor::Calculate_Meleehit(item_def *iptr, Actor *target, int bodypart)
{
	/* note: JOS KÄDESSÄ OLEVA ESINE ON JOTAIN MUUTA KUIN ASE
	NIIN EI SKILLEJÄ PIDÄ KASVATTAA!!!
	Nyt kasvaa handskilli aina silloin */

	const int aluck=stat[STAT_LUC].Get();

	equipment *tequip=&target->equips;
	skillset *tskills=&target->skills;
	const int tluck=target->stat[STAT_LUC].Get();
	const int trace=target->Get_Race();
	const int tg_tactic=target->tactic;
	//      tdv=calculate_slot_pv(bodypart, target->inv.equip );

	bool useleft, useright;
	tequip->decide_meleeweapon(&useleft, &useright);

	/* determine attacker weapon bonuses */
	int skill_type=SKILL_HAND;
	int wpnbonus=0;

	if (iptr)
	{
		if (iptr->type==IS_WEAPON1H || iptr->type==IS_WEAPON2H
			|| iptr->type==IS_MISWEAPON)
		{
			skill_type=iptr->group; //weapon group is same as the skill type

			/* material bonus to hit */
			wpnbonus=materials[iptr->material].hit;
		}
		else skill_type=-1;
	}

	//determine defender weapon skill
	//if no weapon in hand, then must be determined from "dodging" etc?
	item_def *defwpn1;
	if (useright)
	{
		defwpn1=tequip->get_item(EQUIP_RHAND);
	}
	else if (useleft)
	{
		defwpn1=tequip->get_item(EQUIP_LHAND);
	}
	else
		defwpn1=NULL;

	/* if defender skill missing THEN NO bonuses */
	int defskill=0;
	if (defwpn1)
	{
		if (defwpn1->type==IS_WEAPON1H || defwpn1->type==IS_WEAPON2H)
			defskill=tskills->check(SKILLGRP_WEAPON, defwpn1->group);
	}

	int attskill=0;
	if (skill_type>=0)
		attskill=skills.check(SKILLGRP_WEAPON, skill_type);

	/* add luck modifiers from target and destination */
	attskill+=(aluck - 10)-(tluck - 10)+wpnbonus;

	/* substract half of defender weapn skill */
	attskill-=(defskill/2);

	/* every difficulty point substracts 2 points from hit */
	attskill-=(2 * npc_races[trace].bodyparts[bodypart]);

	//attacker's hit modifier
	attskill+=tactics_data[tactic].hit;

	//target's defense value
	attskill-=tactics_data[tg_tactic].dv;

	int tdv=0; //note: value not set, look at code in line 82

	/* defender bodypart DV */
	attskill-=tdv;

	/* attacker has always some probability to hit */
	/* I use 5% */
	if (attskill < MIN_SKILLSCORE)
		attskill=MIN_SKILLSCORE;

	return attskill;
}

int Actor::Calculate_Time(int ticks)
{
	return Gametime::Calculate(stat[STAT_SPD].Get(), ticks, tactic);
}

bool Actor::Can_Carry(int weight)
{
	const int carrycap=Calc_Carryweight();
	if (inv.Get_Weight() + weight > carrycap)
		return false;

	return true;
}

void Actor::Damage_Issue(Damage &dmg)
{
	const int race=Get_Race();
	const int bp=dmg.bodypart;

	/* if target has no such bodypart, cancel out */
	if(npc_races[race].bodyparts[bp] < 0)
		return;

	int damage=dmg.amount;

	/* for damage protection */
	int dprot=hpp[bp].res.Get_Damage_Protection(damage, dmg.element);

	/* armor protection */
	if (hpp[bp].ac > damage)
		damage=0;
	else
		damage-=hpp[bp].ac;

	/* resistance protection */
	if (dprot > damage)
		damage=0;
	else
		damage-=dprot;

	//   msg.vnewmsg(C_GREEN, "AC=%d, resprot=%d.", hpptr[bodypart].ac, dprot);
	//   msg.vnewmsg(C_GREEN, "final=%d.", damage);

	/* substract damage */
	hpp[bp].cur-=damage;

	if(hpp[bp].cur<0)
		hpp[bp].cur=0;
}

void Actor::Death()
{
	if (Is_Player())
	{
#ifdef saladir_debug
		if (confirm_yn("Die?", true, true)==false)
		{
			//restore health
			Renew();
			msg.newmsg("But you feel all right now.");
			return;
		}
#endif
		Game.Set_State(gamedata::End_Of_Game);
	}

	health.value=0;
}

int Actor::Gain_Levels()
{
	return roleplay.Get_Experience_Levels(m.level, exp);
}

int Actor::Get_Hit_Points()
{
	return health.value;
}

const char *Actor::Get_Name()
{
	return m.Get_Name();
}

int Actor::Get_Race()
{
	return m.race;
}

const char *Actor::Get_Description()
{
	return m.desc.c_str();
}

int Actor::Get_Experience_Level()
{
	return m.level;
}

int Actor::Experience_Points_Earned()
{
	const int trace=m.race;
	const int tlevel=m.level;
	return npc_races[trace].exp * tlevel;
}

Coord Actor::Get_Location()
{
	return Coord(x, y);
}

//Return a skill value for current ranged weapon in use
bool Actor::Get_Rangedskill(int *group, int *skill)
{
	int mistype=-1;
	int wpntype=-1;
	int bonus=0;

	if (equips.Get_Ranged_Gear(wpntype, mistype, bonus)==false)
	{
		if (Is_Player())
			msg.newmsg("You don't have any missiles readied!");
		return false;
	}

	/* if bow then needs a missile type 1 */
	if (wpntype==WPN_BOW && mistype==WPN_MISSILE_1)
	{
		*skill=SKILL_BOW;
		*group=SKILLGRP_WEAPON;

		attackbonus=bonus;

		return true;
	}
	/* if crossbow, needs a missile type 2 */
	else if (wpntype==WPN_CROSSBOW && mistype==WPN_MISSILE_2)
	{
		*skill=SKILL_CROSSBOW;
		*group=SKILLGRP_WEAPON;

		attackbonus=bonus;

		return true;
	}
	/* else, throw the missile */
	else
	{
		*skill=SKILL_THROW;
		*group=SKILLGRP_WEAPON;

		attackbonus=-2;

		return true;
	}
	return false;
}

int Actor::Get_Special_Id()
{
	return m.special;
}

bool Actor::Has_Time_For(int ticks)
{
	const int left=timetaken-Calculate_Time(ticks);

	if (left<=0) return false;
	return true;
}

bool Actor::Is_Alive()
{
	if (health.value>0) return true;
	return false;
}

bool Actor::Is_At(int dx, int dy)
{
	if (x==dx && y==dy) return true;
	return false;
}

bool Actor::Is_At(const Coord &c)
{
	if (x==c.x && y==c.y) return true;
	return false;
}

bool Actor::Is_Shopkeeper()
{
	if (m.status & MST_SHOPKEEPER)
		return true;

	return false;
}

void Actor::Drop_Item(invnode *in_src, int count, const Coord &c)
{
	/* if item is equipped */
	if (in_src->slot!=-1)
		equips.clear_slot(in_src->slot);

	invnode *src=inv.remove_n_items(in_src, count);

	if (src == 0)
	{
		msg.newmsg("Error: Item not found from this inventory!", CHB_RED);
		return;
	}

	gameview.Land_Item(src, c);
}

void Actor::Drop_Single_Item(invnode *in_src, const Coord &c)
{
	Drop_Item(in_src, 1, c);
	gameview.Refresh_Item_Map(c); //automatic for single drop
}

void Actor::Handle_Conditions(int slots)
{
	conditions.handle(this, slots);
}

bool Actor::Regenerate_Health(int slots, int ctime)
{
	int i=0;
	int slotheal[6]={0};

	/************************/
	/* regenerate hitpoints */
	/************************/
	/* collect the slots which need healing */
	int j=0;
	for (i=0; i<HPSLOT_MAX; i++)
	{
		if (hpp[i].cur < hpp[i].max)
			slotheal[j++]=i;
	}

	/* j is bigger than 0 if some slot needs healing */
	/* slots to be healed are indexed in array slotheal[], maxindex is j-1 */

	regentime+=slots * ctime;

	int stcon=stat[STAT_CON].Get();

	bool hpregen=false;
	if (j==0)
		regentime=0;
	else if (regentime >= roleplay.Get_Con_HP(stcon))
		hpregen=true;
	else
	{
		int heal=skills.check(SKILLGRP_GENERIC, SKILL_HEALING);
		heal=heal/4;

		if (throwdice(1, 100, 0) < heal)
			hpregen=true;
	}

	if (hpregen)
	{
		regentime=0;

		/* regen every hpslot randomly */
		i=RANDU(j);

		if (hpp[slotheal[i]].cur < hpp[slotheal[i]].max)
			hpp[slotheal[i]].cur++;
	}

	return hpregen;
}

void Actor::Renew()
{
	conditions.Clear_All();
	Restore_Health();
	health.Maximize();
}

void Actor::Reset()
{
	attackbonus=0;
	bill=0;
	exp=0;
	lastdir=0;
	light=0;
	movecount=0;
	regentime=0;
	tactic=TACTIC_NORMAL;
	timetaken=BASE_TIMENEED;
	x=0;
	y=0;

	for (int t=0; t<HPSLOT_MAX; t++)
		hpp[t].Clear();

	for (int t=0; t<STAT_ARRAYSIZE; t++)
		stat[t].Reset(0);
}

void Actor::Restore_Health()
{
	for (int t=0; t<HPSLOT_MAX; t++)
		hpp[t].Maximize();
}

void Actor::Set_Location(int dx, int dy)
{
	x=dx;
	y=dy;
}

/*
 * Test skill against success or failure.
 * This routine was included here because later on some special conditions
 * might affect these conditions. Ie. a blessed player could get extra
 * try, cursed player might get worse results. Lucky player has a better
 * chance and so on.
 *
 * Returns true if skill throw was a success. False if not success.
 *
 */
bool Actor::Skill_Testsuccess(int group, int type)
{
	int luck=0; //note: luck is always zero

	/* get skill value */
	const int value=skills.check(group, type) + luck;

	if(value<=0)
		return false;

	const int dc=throwdice(1, 100, 0);

	if(dc<=value)
		return true;

	return false;
}

void Actor::Spend_Time(int ticks)
{
	timetaken-=Calculate_Time(ticks);
}

void Actor::Save(Tar_Ball &tb)
{
	tb.Put(attackbonus);
	tb.Put(bill);
	tb.Put(buffoon.Get());
	conditions.save(tb);
	tb.Put(exp);
	health.Save(tb);

	for (int i=0; i<HPSLOT_MAX; i++)
		hpp[i].Save(tb);

	inv.save(tb);
	equips.save(tb);
	tb.Put(lastdir);
	tb.Put(light);
	mana.Save(tb);
	m.Save(tb);
	tb.Put(movecount);
	path.Save(tb);
	tb.Put(regentime);
	skills.save(tb);

	for (int i=0; i<STAT_ARRAYSIZE; i++)
		stat[i].Save(tb);

	tb.Put(tactic);
	tb.Put(timetaken);
	tb.Put(x);
	tb.Put(y);
}

void Actor::Load(Tar_Ball &tb)
{
	attackbonus=tb.Get_Next_Value();
	bill=tb.Get_Next_Value();
	buffoon.Mutate(tb.Get_Next_Value());
	conditions.load(tb);
	exp=tb.Get_Next_Value();
	health.Load(tb);

	for (int i=0; i<HPSLOT_MAX; i++)
		hpp[i].Load(tb);

	inv.load(tb);
	//equipment needs to be loaded after the inventory to set equipment
	//point to items in the inventory
	equips.load(tb, inv);

	lastdir=tb.Get_Next_Value();
	light=tb.Get_Next_Value();
	mana.Load(tb);
	m.Load(tb);
	movecount=tb.Get_Next_Value();
	path.Load(tb);
	regentime=tb.Get_Next_Value();
	skills.load(tb);

	for (int i=0; i<STAT_ARRAYSIZE; i++)
		stat[i].Load(tb);

	tactic=tb.Get_Next_Value();
	timetaken=tb.Get_Next_Value();
	x=tb.Get_Next_Value();
	y=tb.Get_Next_Value();
}
