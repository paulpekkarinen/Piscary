/**************************************************************************
 * avatar.cpp --                                                          *
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

//Refactored 28.8.2021 - 7.4.2026 Paul K. Pekkarinen

#include <cstring>
#include "avatar.h"
#include "being.h"
#include "body.h"
#include "caves.h"
#include "condit.h"
#include "creature.h"
#include "currency.h"
#include "damage.h"
#include "dice.h"
#include "display.h"
#include "foodstat.h"
#include "game.h"
#include "gametime.h"
#include "gameview.h"
#include "inventor.h"
#include "invnode.h"
#include "itemdata.h"
#include "lexicon.h"
#include "message.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "pack.h"
#include "roleplay.h"
#include "saldebug.h"
#include "script.h"
#include "selpack.h"
#include "storage.h"
#include "tactics.h"
#include "terrain.h"
#include "trapdash.h"
#include "world.h"

using std::string;

playerinfo player; // this holds the player info

//Using non-species constructor which means Initialize() is setting
//character for a new game.
playerinfo::playerinfo()
{
	backpack=new SelectItemsPack(inv, -1);
}

playerinfo::~playerinfo()
{
	delete backpack;
}

const char *playerinfo::Get_Title()
{
	return m.desc.c_str(); //desc is player's title data
}

void playerinfo::Change_Alignment(char dir, int amount)
{
	m.steer_alignment(dir, amount);
	GAME_NOTIFYFLAGS|=GAME_ALIGNCHG;
}

void playerinfo::Getangry(level_type *level, Actor *kohde, bool always)
{
	msg.vnewmsg(CH_RED, "Error: Trying to get angry with %s at %s!",
		kohde->Get_Name(), level->Get_Name());
}

void playerinfo::Changetactics()
{
	tactic++;

	if (tactic==TACTIC_BERZERK+1)
		tactic=TACTIC_COWARD;

	msg.update();
	tactics_data[tactic].Show();
}

void playerinfo::Check_Bill() const
{
	if (!bill)
	{
		msg.newmsg("You don't have any debts!", C_WHITE);
	}
	else
	{
		Currency rupees(bill);
		msg.vnewmsg(C_WHITE, "Your current bill: "
			"%ld gold, %ld silver and %ld copper!",
				rupees.gold, rupees.silver, rupees.copper);
	}
}

void playerinfo::Checkstat(level_type *level, bool lower, bool showmsg)
{
	bool alarmi=false;
	Calculate_Totalhp();

	/* check for hpslot healing and hp warning */
	for (int i=0; i<HPSLOT_MAX; i++)
	{
		const int eqslot=eqslot_from_hpslot[i];
		Bodypart part(i);

		/* warn for low hitpoints */
		if (CONFIGVARS.health_alarm>0 && lower && showmsg
			&& (equips.is_usable(eqslot)))
		{
			real rhp=(real)hpp[i].max;

			rhp=(rhp/100.0) * CONFIGVARS.health_alarm;

			if (hpp[i].cur <= (int)rhp)
			{
				if (!alarmi)
				{
					alarmi=true;
					msg.newmsg("HPALARM!", CHB_RED);
				}
				msg.vnewmsg(C_RED, "Your %s is in bad condition!", part.Get_Name());
			}
		}

		if ((hpp[i].cur > 0) &&
			(equips.is_usable(eqslot)))
		{

			/* put the slot back to work */
			equips.make_usable(eqslot);

			if (i==HPSLOT_LEGS)
				conditions.remove(CONDIT_BADLEGS);
			else if (i==HPSLOT_LEFTHAND)
				conditions.remove(CONDIT_BADLARM);
			else if (i==HPSLOT_RIGHTHAND)
				conditions.remove(CONDIT_BADRARM);

			msg.vnewmsg(C_GREEN, "Your %s looks better!", part.Get_Name());
		}
	}

	if (debug->Cheat()) //if debug mode on, don't take damage
		return;

	/* check for the hpslot damage */
	damage_checkbodyparts(level, this);

	if (Is_Alive()==false)
	{
		msg.vnewmsg(C_RED, "You die... (%s).", get_player_killer());
		Death();
		return;
	}
}

void playerinfo::Damage_Message(Damage &dmg)
{
	if (dmg.Whole_Body())
	{
		msg.newmsg("Your whole body takes damage.", C_RED);
		return;
	}

	const int bp=dmg.bodypart;
	const int damage=dmg.amount;
	const real rhp=hpp[bp].cur;

	Bodypart part(bp);
	const char *partname=part.Get_Name();
	const char *partart=part.Get_Art();

	if(damage>rhp)
		msg.vnewmsg(CH_RED, "Your %s %s very badly injured!",
			partname, partart);
	else if(damage > (rhp*0.8))
		msg.vnewmsg(C_RED, "Your %s %s severely injured!",
			partname, partart);
	else if(damage > (rhp*0.5))
		msg.vnewmsg(CH_YELLOW, "Your %s %s moderately injured!",
			partname, partart);
	else if(damage > (rhp*0.2))
		msg.vnewmsg(C_YELLOW, "Your %s %s slightly injured!",
			partname, partart);
	else if(damage > (rhp*0.05))
		msg.vnewmsg(C_WHITE, "Your %s %s is just scratched.",
			partname, partart);
}

void playerinfo::Eat_Addnutr(item_def *item, int weight)
{
	/* food nutrition based on weight and multiplier pmod1 */
	real nutrv = (real)weight * 0.5;

	nutrv = nutrv / 100 * item->pmod1;

	nutr += (int32u)nutrv;

	if (nutr >= FOOD_MAXNUTR)
		nutr=FOOD_MAXNUTR;

	if (nutr <= FOOD_FAINTING)
		conditions.add(CONDIT_FAINTING, 1);
	else if (nutr <= FOOD_STARVING)
		conditions.add(CONDIT_STARVING, 1);
	else if (nutr <= FOOD_HUNGRY)
		conditions.add(CONDIT_HUNGRY, 1);
	else if (nutr <= FOOD_FULL)
		conditions.delete_group(CONDGRP_FOOD);
	else if (nutr <= FOOD_SATIATED)
		conditions.add(CONDIT_SATIATED, 1);
	else
		conditions.add(CONDIT_BLOATED, 1);
}

void playerinfo::Gain_Experience(int gain)
{
	GAME_NOTIFYFLAGS |= GAME_EXPERCHG;
	exp+=gain;
	roleplay.Check_Levelraise(*this);
}

void playerinfo::Go_Hunting()
{
	Set_Location(10, 10); //note: what tf is this location?
	sight=10;
	huntmode=true;
}

void playerinfo::Handle_Confusion(Condition *cond)
{
	const int v=cond->Get_Value();

	if (v>0)
	{
		//note: I guess this is missing?
		msg.newmsg("Confusion handling for you!", C_RED);
	}
	else
		msg.newmsg("You feel more stable now.", CH_GREEN);
}

void playerinfo::Handlestatus(level_type *level, int slots)
{
	static int oldpackweight=0;

	if (Regenerate_Health(slots, stat[STAT_SPD].Get()))
	{
		/* when regenerated, show hpsp area */
		GAME_NOTIFYFLAGS|=GAME_HPSPCHG;

		Checkstat(level, false, true);
	}

	/* substract food, each slot takes one food */
	int oldnutr = nutr;

	nutr-=slots;

	if (oldnutr >= FOOD_FAINTING && nutr < FOOD_FAINTING)
	{
		msg.addwait("You need food badly!", C_RED);
		conditions.add(CONDIT_FAINTING, 1);
	}

	if (oldnutr >= FOOD_STARVING && nutr < FOOD_STARVING)
	{
		conditions.add(CONDIT_STARVING, 1);
		msg.addwait("You're getting really hungry!", C_RED);
	}

	if (oldnutr >= FOOD_HUNGRY && nutr < FOOD_HUNGRY)
	{
		conditions.add(CONDIT_HUNGRY, 1);
		msg.addwait("You're getting hungry!", C_RED);
	}

	if (oldnutr >= FOOD_FULL && nutr < FOOD_FULL)
	{
		conditions.delete_group(CONDGRP_FOOD);
	}

	if (oldnutr >= FOOD_BLOATED && nutr < FOOD_BLOATED)
	{
		conditions.add(CONDIT_BLOATED, 1);
	}

	if (oldnutr >= FOOD_SATIATED && nutr < FOOD_SATIATED)
	{
		conditions.add(CONDIT_SATIATED, 1);
	}

	if (nutr <= FOOD_FAINTED)
	{
		player_killer("Starved to death");
		msg.add("You died by starving to death.", CH_RED);

		conditions.add(CONDIT_FAINTED, 1);
		Death();

		GAME_NOTIFYFLAGS|=GAME_SHOWALLSTATS;
		return;
	}

	/* handle all conditions, like confusion, poison etc. */
	Handle_Conditions(slots);

	const int invload=inv.Get_Weight();

	if (oldpackweight!=invload)
	{
		oldpackweight=invload;

		const int carry = Calc_Carryweight();

		/* remove all conditions related to carry weight */
		conditions.delete_group(CONDGRP_PW);

		/* set new conditions if necessary */
		if (invload > (carry *  WGH_OVERLOAD / 100))
		{
			conditions.add(CONDIT_OVERLOADED, 1);
		}
		else if (invload > (carry * WGH_STRAIN / 100))
		{
			conditions.add(CONDIT_STRAINED, 1);
		}
		else if (invload > (carry * WGH_BURDEN / 100))
		{
			conditions.add(CONDIT_BURDENED, 1);
		}
	}
}

void playerinfo::Hitwall()
{
	texts->Random_Message(Script::Wall_Hit, 1+RANDU(15));
}

void playerinfo::Initialize()
{
	Reset();
	m.playerize();
	buffoon.Mutate(Species::Human);
	lastdir=0;
	color=CH_RED;
	nutr=FOOD_SATIATED;

	sight=8;
	delta=4;
	num_kills=0;

	huntmode=false;
	repeatwalk=false;
	monsterinsight=false;
	searchmode=false;

	//recalculate hp and mana
	const int pr=Get_Race();
	roleplay.Calculate_HP(hpp, npc_races[pr].hp_base, pr);
	Calculate_Totalhp();
	mana.Initialize(20);

	/* initialize player stats */
	for (int i=0; i<STAT_ARRAYSIZE; i++)
		stat[i].Reset(50);

	stat[STAT_SPD].initial=BASE_SPEED + roleplay.Get_Dex_Speed(stat[STAT_DEX].Get());
	stat[STAT_SPD].max=STATMAX_SPEED;

	stat[STAT_LUC].initial=5+RANDU(11);
	stat[STAT_LUC].max=STATMAX_LUCK;

	/* clear quick skills */
	for (int i=0; i<NUM_QUICKSKILLS; i++)
		qskills[i].Reset();

	conditions.init();
	quests.removeall();
	path.clear();

	roleplay.Calculate_Itembonus(this);
}

void playerinfo::Jump_To(const Coord &c)
{
	Set_Location(c.x, c.y);

	last_room=-1; //"exit" current room if in any
	Check_Room(world->Get_Current_Level());

	gameview.Center(c);
}

void playerinfo::Killedmonster(being *mptr)
{
	const int expgain = mptr->Experience_Points_Earned();

	num_kills++;

	msg.vnewmsg(C_GREEN, "(%ld exp)!", expgain);

	Gain_Experience(expgain);

	quests.check_kill(mptr);

	GAME_NOTIFYFLAGS|=GAME_EXPERCHG;
}

void playerinfo::Noticestuff(level_type *level)
{
	const Coord pc=Get_Location();

	const bool tilelight=gameview.Is_Visible(pc);

	if (!tilelight)
	{
		if (RANDU(100)>80)
			msg.newmsg("It's really dark here.", C_WHITE);
	}

	const int tt=level->Get_Terrain(pc);

	if (tt==TYPE_STAIRUP)
	{
		if (tilelight)
			msg.newmsg("You see stairs leading up!", C_GREEN);
		else
			msg.newmsg("You feel a staircase here.", C_GREEN);
	}
	else
	{
		if (tt==TYPE_STAIRDOWN)
		{
			if (tilelight)
				msg.newmsg("You see stairs leading down!", C_GREEN);
			else
				msg.newmsg("You feel a staircase here.", C_GREEN);
		}
	}

	const int a=gameview.Count_Items(pc);
	if (tilelight)
	{
		if (a>1)
			msg.newmsg("Several items are here!", C_WHITE);
		else if (a==1)
		{
			/* do autopickup if required */
			invnode *ptr=gameview.Get_Item(pc);
			display->Item_Info(&ptr->i, ptr->i.weight, ptr->count, "You see here");

			if (CONFIGVARS.autopickup)
			{
				repeatwalk=false;
				player_autopickup(level, pc);
			}
		}
	}
	else
	{
		if (a)
		{
			msg.newmsg("Feels like some items are here.", C_WHITE);
		}
	}

	/* activate traps */
	if (handletrap(level, pc, this))
		repeatwalk=false;
}

void playerinfo::Shouldflee(level_type *level)
{
	msg.vnewmsg(CH_RED, "Error: Trying to flee in the %s!",
		level->Get_Name());
}

void playerinfo::Show_Inventory()
{
	backpack->Set_Filter(-1); //display all items
	backpack->Get_Handle("Items in your backpack");
}

void playerinfo::Switch_Searchmode()
{
	if (searchmode)
	{
		searchmode=false;
		msg.newmsg("You won't be so attentive anymore.");
	}
	else
	{
		if (!skills.check(SKILLGRP_GENERIC, SKILL_SEARCHING))
		{
			msg.newmsg("Sorry, you don't have the skill.");
			return;
		}

		searchmode=true;
		msg.newmsg("You're now more observant.");
	}
}

void playerinfo::Display()
{
	put_char('@', color);
}

void playerinfo::Save(Tar_Ball &tb)
{
	being::Save(tb);

	tb.Put(color);
	tb.Put(nutr);
	tb.Put(sight);

	tb.Put(num_kills);

	tb.Put_Bool(huntmode);
	tb.Put_Bool(repeatwalk);
	tb.Put_Bool(monsterinsight);
	tb.Put_Bool(searchmode);

	for (int i=0; i<NUM_QUICKSKILLS; i++)
		qskills[i].Save(tb);

	quests.save(tb);
}

void playerinfo::Load(Tar_Ball &tb, level_type *level)
{
	being::Load(tb, level);

	color=tb.Get_Next_Value();
	nutr=tb.Get_Next_Value();
	sight=tb.Get_Next_Value();

	num_kills=tb.Get_Next_Value();

	huntmode=tb.Get_Next_Bool();
	repeatwalk=tb.Get_Next_Bool();
	monsterinsight=tb.Get_Next_Bool();
	searchmode=tb.Get_Next_Bool();

	for (int i=0; i<NUM_QUICKSKILLS; i++)
		qskills[i].Load(tb);

	quests.load(tb);
}
