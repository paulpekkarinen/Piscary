/**************************************************************************
 * roleplay.cpp --                                                        *
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

//Refactored 20.8.2021 - 2.8.2023 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "dice.h"
#include "game.h"
#include "invnode.h"
#include "message.h"
#include "names.h"
#include "output.h"
#include "roleplay.h"

using std::string;

Roleplay roleplay;

Roleplay::Roleplay()
{
	expneeded[0]=150;

	for (int i=1; i<Maxnum_Levels; i++)
	{
		real exp2=(real)expneeded[i-1];
		exp2=exp2*1.60;
		expneeded[i]=(int)exp2;

		if (i > 10)
		{
			real exp3=(real)i / 50 + 1.0;
			exp2=(real)expneeded[8];
			exp2=exp2*exp3;
			expneeded[i]=expneeded[i-1]+(int)exp2;
		}
	}

	expneeded[0]=0;
	expneeded[1]=0; //note: is this a bug?

	/* init DEX to SPEED table */
	for(int i=0; i<Table_Len; i++)
	{
		DEX_SPEED[i]=(i-50)/2;
	}

	for(int i=0; i<Table_Len; i++)
	{
		CON_HITP[i]=((100-i)*250)+1000;
	}
}

void Roleplay::Additembonus(hpslot *hpslot, Equipslot *eqslot)
{
	if (!hpslot || !eqslot)
		return;

	if (eqslot->item == 0)
		return;

	/* armor class (damage protection) */
	hpslot->ac += eqslot->item->i.ac;

	/*
	 * defence value, substracted directly from attack
	 * skill on melee attacks
	 */
	hpslot->dv += eqslot->item->i.dv;

	if (eqslot->item->i.status & ITEM_ENCHANTED)
	{
		hpslot->res.Modify(eqslot->item->i.ench.res);
	}
}

int Roleplay::Get_Con_HP(int index)
{
	return CON_HITP[index];
}

int Roleplay::Get_Dex_Speed(int index)
{
	return DEX_SPEED[index];
}

int Roleplay::Get_Experience_Levels(int &level, int exp)
{
	if (level > Maxnum_Levels)
	{
		level=Maxnum_Levels;
		return 0;
	}

	//counts how many levels gained based on experience points
	int raise=0;
	for (int i=level+1; i<Maxnum_Levels; i++)
	{
		if (exp >= expneeded[i])
		{
			raise++;
		}
		else
			break;
	}

	//returns number of levels or zero if no levels gained
	return raise;
}

void Roleplay::Advance_To_Level(being *b, int explvl)
{
	b->exp=expneeded[explvl]+1;
	b->m.level=0;

	Check_Levelraise(b, true);
}

void Roleplay::Advance_Levels(being *mptr)
{
	//int race=mptr->m.race;

	/* init basic stats */
	for (int t=0; t<STAT_BASICARRAY; t++)
	{
		mptr->stat[t].Set_Initial(mptr->m.stats.Get_Statpack_Value(t),
			STATMAX_GEN, STATMIN_GEN);
	}

	//set luck and speed separately
	mptr->stat[STAT_LUC].Set_Initial(mptr->m.stats.LUC, STATMAX_LUCK, 0);
	mptr->stat[STAT_SPD].Set_Initial(mptr->m.stats.SPD, STATMAX_SPEED, 1);

	mptr->stat[STAT_SPD].Change_Initial(DEX_SPEED[mptr->m.stats.DEX], false);

	/* get base stats from racelist */
 //   mptr->hp_max=npc_races[mptr->m.race].hp_base;
 //   mptr->sp_max=npc_races[mptr->m.race].sp_base;

	/* calculate hitpoints and spellpoints */
	mptr->base_hp+=throwdice(npc_races[mptr->m.race].hp_plev_dt,
		npc_races[mptr->m.race].hp_plev_ds, 0);
	mptr->mana.Increase_Max(throwdice(npc_races[mptr->m.race].sp_plev_dt,
		npc_races[mptr->m.race].sp_plev_ds, 0));
	mptr->mana.Maximize();

	/* now increase skills */
	mptr->skills.raiselevel();

	Calculate_HP(mptr->hpp, mptr->base_hp, mptr->Get_Race());

	mptr->Advance_Level();
}

/*****************************************************
 calculate initial hitpoints from base give in basehp
 ***************************************************/
/* now takes account the fact of "missing" bodyparts */
/* as some races DON'T have some bodyparts */

void Roleplay::Calculate_HP(hpslot *hpack, int basehp, int race)
{
	if (basehp==0)
		basehp=12+RANDU(13);

	/* EVERY MONSTER HAS A HEAD AND BODY */

	hpack[HPSLOT_BODY].Set_Points(basehp);

	real val=basehp * HP_HEADMOD;
	hpack[HPSLOT_HEAD].Set_Points((int)val);

	/* HANDS AND LEGS ARE OPTIONAL */
	if (npc_races[race].bodyparts[HPSLOT_LEFTHAND]>=0)
	{
		val=basehp * HP_HANDMOD;
	}
	else
		val=0;

	hpack[HPSLOT_LEFTHAND].Set_Points((int)val);

	if (npc_races[race].bodyparts[HPSLOT_RIGHTHAND]>=0)
	{
		val=basehp * HP_HANDMOD;
	}
	else
		val=0;

	hpack[HPSLOT_RIGHTHAND].Set_Points((int)val);

	if (npc_races[race].bodyparts[HPSLOT_LEGS]>=0)
	{
		val=basehp * HP_LEGSMOD;
	}
	else
		val=0;

	hpack[HPSLOT_LEGS].Set_Points((int)val);
}

/*
 * calculate ALL bonuses caused by equipped items for a creature
 */
void Roleplay::Calculate_Itembonus(Actor *actor)
{
	Equipslot *equip=actor->equips.equip;
	hpslot *hpp=actor->hpp;
	statpack *stat=actor->stat;
	const int prace=actor->Get_Race();
	int i;

	/* first clear all bonuses from active bodyparts (which have hpslot) */
	for (i=0; i<HPSLOT_MAX; i++)
		hpp[i].Clear_Bonuses();

	for (i=0; i<STAT_ARRAYSIZE; i++)
		stat[i].temp=0;

	/* add stats modifiers */
	for (i=0; i<MAX_EQUIP; i++)
	{
		if (equip[i].item)
		{
			/* check if the enchantment pack is valid */
			if (equip[i].item->i.status & ITEM_ENCHANTED)
			{
				/* add enchantment bonus */
				for (int t=0; t<STAT_ARRAYSIZE; t++)
					stat[t].temp+=equip[i].item->i.ench.stats.Get_Statpack_Value(t);
			}
		}
	}

	/* recalc speed */
	stat[STAT_SPD].initial=npc_races[prace].stats.SPD + DEX_SPEED[stat[STAT_DEX].Get()];

	/* items which affect every bodypart */
	for (i=0; i<HPSLOT_MAX; i++)
	{
		/* set race ac */
		hpp[i].ac=npc_races[prace].ac;

		/* set race resistances */
		hpp[i].res=npc_races[prace].res;

		Additembonus(&hpp[i], &equip[EQUIP_NECK]);
		Additembonus(&hpp[i], &equip[EQUIP_LRING]);
		Additembonus(&hpp[i], &equip[EQUIP_RRING]);
		Additembonus(&hpp[i], &equip[EQUIP_CLOAK]);

		/* weapons are considered like this */
		Additembonus(&hpp[i], &equip[EQUIP_LHAND]);
		Additembonus(&hpp[i], &equip[EQUIP_RHAND]);
		Additembonus(&hpp[i], &equip[EQUIP_TOOL]);
	}

	/* items which affect only head */
	Additembonus(&hpp[HPSLOT_HEAD], &equip[EQUIP_HEAD]);

	/* items which affect only legs */
	Additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_PANTS]);
	Additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_BOOTS]);
	Additembonus(&hpp[HPSLOT_LEGS], &equip[EQUIP_LEGS]);

	/* items which affect only body */
	Additembonus(&hpp[HPSLOT_BODY], &equip[EQUIP_BODY]);
	Additembonus(&hpp[HPSLOT_BODY], &equip[EQUIP_SHIRT]);

	/* items which affect only left hand */
	Additembonus(&hpp[HPSLOT_LEFTHAND], &equip[EQUIP_LARM]);
	Additembonus(&hpp[HPSLOT_LEFTHAND], &equip[EQUIP_GLOVES]);

	/* items which affect only left hand */
	Additembonus(&hpp[HPSLOT_RIGHTHAND], &equip[EQUIP_RARM]);
	Additembonus(&hpp[HPSLOT_RIGHTHAND], &equip[EQUIP_GLOVES]);
}

void Roleplay::Calculate_Raisestats(playerinfo &plr)
{
	const int pr=plr.Get_Race();
	const int dicet=npc_races[pr].hp_plev_dt;
	const int dices=npc_races[pr].hp_plev_ds;

	const int hpgain=throwdice(dicet, dices, 0);

	Calculate_HP(plr.hpp, plr.hpp[HPSLOT_BODY].max+hpgain, pr);

	plr.Advance_Level();
}

int Roleplay::Calculate_Slot_AC(int bodypart, Equipslot *eqslots)
{
	int value;

	if (bodypart>HPSLOT_MAX)
		return 0;

	value=0;
	/* calculate absorb for head */
	if (bodypart==HPSLOT_HEAD)
	{
		if (eqslots[EQUIP_HEAD].item)
			value+=eqslots[EQUIP_HEAD].item->i.ac;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.ac;
	}
	else if (bodypart==HPSLOT_LEFTHAND)
	{
		if (eqslots[EQUIP_LARM].item)
			value+=eqslots[EQUIP_LARM].item->i.ac;
		if (eqslots[EQUIP_GLOVES].item)
			value+=eqslots[EQUIP_GLOVES].item->i.ac;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.ac;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.ac;
		if (eqslots[EQUIP_LRING].item)
			value+=eqslots[EQUIP_LRING].item->i.ac;

		/* shield should be tied to some skill */
		/* including other handheld armor */
		if (eqslots[EQUIP_LHAND].item)
			value+=eqslots[EQUIP_LHAND].item->i.ac;
	}
	else if (bodypart==HPSLOT_RIGHTHAND)
	{
		if (eqslots[EQUIP_RARM].item)
			value+=eqslots[EQUIP_RARM].item->i.ac;
		if (eqslots[EQUIP_GLOVES].item)
			value+=eqslots[EQUIP_GLOVES].item->i.ac;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.ac;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.ac;
		if (eqslots[EQUIP_RRING].item)
			value+=eqslots[EQUIP_RRING].item->i.ac;

		/* shield should be tied to some skill */
		/* including other handheld armor */
		if (eqslots[EQUIP_RHAND].item)
			value+=eqslots[EQUIP_RHAND].item->i.ac;
	}
	else if (bodypart==HPSLOT_BODY)
	{
		if (eqslots[EQUIP_BODY].item)
			value+=eqslots[EQUIP_BODY].item->i.ac;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.ac;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.ac;
	}
	else if (bodypart==HPSLOT_LEGS)
	{
		if (eqslots[EQUIP_LEGS].item)
			value+=eqslots[EQUIP_LEGS].item->i.ac;
		if (eqslots[EQUIP_PANTS].item)
			value+=eqslots[EQUIP_PANTS].item->i.ac;
		if (eqslots[EQUIP_BOOTS].item)
			value+=eqslots[EQUIP_BOOTS].item->i.ac;
	}
	//   msg.newmsg(C_YELLOW, "Your armor absorbs %d points", value);
	return value;
}

int Roleplay::Calculate_Slot_PV(int bodypart, Equipslot *eqslots)
{
	int value;

	if (bodypart>HPSLOT_MAX)
		return 0;

	value=0;
	if (bodypart==HPSLOT_HEAD)
	{
		if (eqslots[EQUIP_HEAD].item)
			value+=eqslots[EQUIP_HEAD].item->i.dv;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.dv;
	}
	else if (bodypart==HPSLOT_LEFTHAND)
	{
		if (eqslots[EQUIP_LARM].item)
			value+=eqslots[EQUIP_LARM].item->i.dv;
		if (eqslots[EQUIP_GLOVES].item)
			value+=eqslots[EQUIP_GLOVES].item->i.dv;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.dv;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.dv;
		if (eqslots[EQUIP_LRING].item)
			value+=eqslots[EQUIP_LRING].item->i.dv;

		/* shield should be tied to some skill */
		/* including other handheld armor */
		if (eqslots[EQUIP_LHAND].item)
			value+=eqslots[EQUIP_LHAND].item->i.dv;
	}
	else if (bodypart==HPSLOT_RIGHTHAND)
	{
		if (eqslots[EQUIP_RARM].item)
			value+=eqslots[EQUIP_RARM].item->i.dv;
		if (eqslots[EQUIP_GLOVES].item)
			value+=eqslots[EQUIP_GLOVES].item->i.dv;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.dv;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.dv;
		if (eqslots[EQUIP_RRING].item)
			value+=eqslots[EQUIP_RRING].item->i.dv;

		/* shield should be tied to some skill */
		/* including other handheld armor */
		if (eqslots[EQUIP_RHAND].item)
			value+=eqslots[EQUIP_RHAND].item->i.dv;
	}
	else if (bodypart==HPSLOT_BODY)
	{
		if (eqslots[EQUIP_BODY].item)
			value+=eqslots[EQUIP_BODY].item->i.dv;
		if (eqslots[EQUIP_SHIRT].item)
			value+=eqslots[EQUIP_SHIRT].item->i.dv;
		if (eqslots[EQUIP_CLOAK].item)
			value+=eqslots[EQUIP_CLOAK].item->i.dv;
	}
	else if (bodypart==HPSLOT_LEGS)
	{
		if (eqslots[EQUIP_LEGS].item)
			value+=eqslots[EQUIP_LEGS].item->i.dv;
		if (eqslots[EQUIP_PANTS].item)
			value+=eqslots[EQUIP_PANTS].item->i.dv;
		if (eqslots[EQUIP_BOOTS].item)
			value+=eqslots[EQUIP_BOOTS].item->i.dv;
	}
	//   msg.newmsg(C_YELLOW, "Your armor absorbs %d points", value);
	return value;
}

void Roleplay::Check_Expneeded(playerinfo &plr)
{
	const int exp_level=plr.Get_Experience_Level();
	const int needed=expneeded[exp_level+1]-plr.exp;
	
	set_color(C_WHITE);

	my_printf("Level %d with %d exp points, %d points to next level.\n",
		exp_level, expneeded[exp_level], needed);
}

bool Roleplay::Check_Levelraise(playerinfo &plr)
{
	int raise=plr.Gain_Levels();

	if (raise==0)
		return false;

	GAME_NOTIFYFLAGS |= GAME_SHOWALLSTATS;

	while (raise>0)
	{
		msg.newmsg("LEVEL!", CH_GREEN);
		Calculate_Raisestats(plr);
		raise--;
	}

	msg.vaddwait(CH_GREEN, "Welcome to level %d, %s!",
		plr.Get_Experience_Level(), plr.Get_Name());

	return true;
}

bool Roleplay::Check_Levelraise(being *monster, bool initmode)
{
	int raise=monster->Gain_Levels();

	if (raise==0)
		return false;

	while (raise>0)
	{
		if (!initmode)
		{
			string s=monster_sprintf(monster, true, true);
			s.append(" suddenly looks more powerful.");
			msg.Add_Dist(monster->x, monster->y, s.c_str(), C_GREEN,
				NULL, C_GREEN);
		}
		Advance_Levels(monster);

		raise--;
	}

	return true;
}
