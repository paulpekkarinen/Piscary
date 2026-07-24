/**************************************************************************
 * stats.cpp --                                                           *
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

//Refactored 20.8.2021 - 27.2.2023 Paul K. Pekkarinen

#include "game.h"
#include "output.h"
#include "stats.h"
#include "storage.h"

struct Stat_Data
{
	const char *name;
	const char *short_name;
	const char *description;
};

Stat_Data stat_data[STAT_ARRAYSIZE]=
{
	{"Strength", "STR",
	"\007High strength makes you, well eh..., stronger. It affects your carrying "
	"capasity and several other things like how hard you hit your foes. "
	},
	{"Toughness", "TGH",
	"\007Toughness affects in many ways. High toughness makes you more tolerant "
	"to attacks, affecting your defence rating. Low toughness makes the opposite. "
	"There're some other situations too where high toughness does aid your "
	"journeys."
	},
	{"Constitution", "CON",
	"\007Constitution tells how your body is built. The natural strength of your "
	"body. It affects your healing rate, how fast your wounds recover."
	},
	{"Charisma", "CHA",
	"\007Charisma is your \"outlook\", how well you get by with others, how "
	"good looking you are, and so on.\n\n"
	"\001You can't edit this stat because it really doesn't affect anything "
	"yet."
	},
	{"Dexterity", "DEX",
	"\007Dexterity affects your speed and other speed related issues. High "
	"dexterity allows you to act faster in all situations."
	},
	{"Wisdom", "WIS",
	"\007Wisdom tells something about your general knowledge.\n\n"
	"\001You can't edit this stat because it really doesn't affect anything "
	"yet."
	},
	{"Intelligence", "INT",
	"\007Intelligence affects your learning ability. The higher your intelligence "
	"is, the better you can learn new skills, spells and so on.\n\n "
	"\001You can't edit this stat because it really doesn't affect anything "
	"yet."
	},
	{"Luck", "LUC",
	"\007The amount of luck you have."
	},
	{"Speed", "SPD",
	"\007Speed is a derived attribute, it's calculated mainly by using your "
	"dexterity. High speed allows you to move faster."
	}
};

const char *txt_statrating[]=
{
   "Very bad",
   "Bad",
   "Low",
   "Poor",
   "Fair",
   "Average",
   "Nice",
   "Good",
   "Very good",
   "Superhuman",
   "Immortal",
   0
};

const char *Stat::Get_Name()
{
	return stat_data[st].name;
}

const char *Stat::Get_Short_Name()
{
	return stat_data[st].short_name;
}

void Stat::Show_Description(int top)
{
	my_wordwraptext(stat_data[st].description,
		top, SCREEN_LINES, 1, SCREEN_COLS);
	clrtobot();
}

//===

int statpack::Get()
{
	int result=0;

	result=initial + temp + perm;

	if (result < min)
		result = min;
	else if (result > max)
		result = max;

	return result;
}

void statpack::Reset(int i)
{
	initial=i;
	temp=0;
	perm=0;
	min=0;
	max=99;
}

void statpack::Save(Tar_Ball &tb)
{
	tb.Put(initial);
	tb.Put(temp);
	tb.Put(perm);
	tb.Put(min);
	tb.Put(max);
}

void statpack::Load(Tar_Ball &tb)
{
	initial=tb.Get_Next_Value();
	temp=tb.Get_Next_Value();
	perm=tb.Get_Next_Value();
	min=tb.Get_Next_Value();
	max=tb.Get_Next_Value();
}

void statpack::Change_Initial(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	initial+=amount;

	if (initial > max)
		initial = max;
	if (initial < min)
		initial = min;
}

void statpack::Change_Perm(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	perm+=amount;

	if (perm > STATMAX_GEN)
		perm = STATMAX_GEN;

	if (perm < -STATMAX_GEN)
		perm = -STATMAX_GEN;
}

void statpack::Change_Temp(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	temp+=amount;

	if (temp > STATMAX_GEN)
		temp = STATMAX_GEN;

	if (temp < -STATMAX_GEN)
		temp = -STATMAX_GEN;
}

void statpack::Set_Initial(int value, int imax, int imin)
{
	max=imax;
	min=imin;
	temp=0;

	initial=value;

	if (initial > max)
		initial = max;
	if (initial < min)
		initial = min;
}
