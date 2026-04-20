//specmon.cpp - special monsters

//Refactored 24.9.2021 - 25.3.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "dice.h"
#include "input.h"
#include "message.h"
#include "names.h"
#include "script.h"
#include "specmon.h"
#include "stats.h"

void billgates_chat(being *);
void billgates_act(being *, level_type *);
void eat_billgates(level_type *level);
void sparhawk_act(being *, level_type *);
void thomas_act(being *, level_type *);
void thomas_chat(being *);

void act_specialmonster(being *monster, level_type *level)
{
	switch (monster->m.special)
	{
		case NPC_THOMAS:
			thomas_act(monster, level);
			break;
		case NPC_BILLGATES:
			billgates_act(monster, level);
			break;
		case NPC_SPARHAWK:
			sparhawk_act(monster, level);
			break;
		default: break;
	}
}

void billgates_act(being *monster, level_type *level)
{
	if (RANDU(100) < 10)
		msg.add_dist(level, monster->x, monster->y,
			texts->Get_Random(Script::Bill_Gates_Message),
			1+RANDU(15), NULL, 0);
}

void billgates_chat(being *mptr)
{
	if (!(mptr->m.status & MST_KNOWN))
	{
		msg.newmsg("Hello, my name is Bill Gates!", C_GREEN);
		mptr->m.status|=MST_KNOWN; // monster has been met now
	}
	else
		msg.newmsg(C_GREEN, "Hello again %s!", player.Get_Name());
}

void eat_billgates(level_type *level)
{
	msg.setdelay(300);
	msg.add("...as you finish your meal your stomach", C_WHITE);
	msg.add("starts to vibrate violently", C_WHITE);
	msg.add("..oh no..", C_WHITE);
	msg.add("...this really...", C_GREEN);
	msg.add("...wasn't a good idea...", CH_GREEN);
	msg.add("...........", C_RED);
	msg.add("You transform into Windows 95(R)...", C_RED);
	msg.add("...you try to keep operating...", C_RED);
	msg.setdelay(500);
	msg.add("...but then...", C_RED);
	msg.setdelay(0);

	player_killer("eating a very unstable meal");
	msg.add("... you crash.", CH_RED);
	player.Death();
}

void eat_specialmonster(level_type *level, int type)
{
	switch (type)
	{
		case NPC_BILLGATES: eat_billgates(level);
			break;
		default:
			break;
	}
}

void monster_chatspecials(being *mptr)
{
	switch (mptr->m.special)
	{
		case NPC_BILLGATES:
			billgates_chat(mptr);
			break;
		case NPC_THOMAS:
			thomas_chat(mptr);
			break;
		default:
			msg.newmsg("He hasn't much to talk about", C_WHITE);
	}
}

void sparhawk_act(being *monster, level_type *level)
{
	if (RANDU(100) < 10)
	{
		const char *m=texts->Get_Random(Script::Sparhawk_Comments);
		
		msg.add_dist(level, monster->x, monster->y, m, CH_GREEN, NULL, 0);
	}
}

void thomas_act(being *monster, level_type *level)
{
	if (RANDU(100) < 10)
		msg.add_dist(level, monster->x, monster->y,
			texts->Get_Random(Script::Thomas_Message), CH_GREEN, NULL, 0);
}

void thomas_chat(being *mptr)
{
	if (!(mptr->m.status & MST_KNOWN))
	{
		msg.add("\"Hey, is it really you?\", Thomas suddenly asks.", CH_WHITE);
		msg.vadd(CH_WHITE, "You probably can't recognize me. My name is %s!",
            mptr->m.name.c_str());

		msg.add("After you last time saw me I have met a terrible misfortune,", CH_WHITE);
		msg.add("my own world turned against me and now I am corrupted like hell and", CH_WHITE);
		msg.add("stuck here in this world!", CH_WHITE);
		msg.add("I really would like to get rid of these bloody corruptions and", CH_WHITE);
		msg.add("finally get back to my world Ankardia!", CH_WHITE);
		mptr->m.status|=MST_KNOWN;
		msg.add("Are you willing to help me with this (Y/n)?", C_GREEN);
		//	  msg.notice();
		if (!confirm_yn(NULL, true, false))
		{
			msg.add("You bastard!", C_RED);
			msg.add("Thomas hits you with DJGPP C++ compiler!", C_RED);
			msg.add("You are badly optimized", C_RED);
			msg.add("You feel slower...", C_GREEN);
			player.stat[STAT_DEX].Change_Perm(-RANDU(20), true);
		}
		else
			msg.add("Thanks, quest will be here later!", CH_GREEN);
	}
	else
		msg.vadd(C_GREEN, "Any news for me %s!", player.Get_Name());
}
