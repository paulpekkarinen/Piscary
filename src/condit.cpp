/**************************************************************************
 * condit.cpp --                                                          *
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

//Refactored 2021 - 8.6.2023 by Paul K. Pekkarinen

#include <cstring>
#include "condit.h"
#include "output.h"
#include "storage.h"

//Data for each condition.
struct cond_desc
{
	const char *name;
	const char *desc;
	int16u flags;
	int group;
	int color;
};

const cond_desc cond_data[CONDIT_MAX]=
{
	{"Bloated",    NULL, 0, CONDGRP_FOOD, CH_GREEN },
	{"Satiated",   NULL, 0, CONDGRP_FOOD, C_GREEN },
	{"Hungry",     NULL, 0, CONDGRP_FOOD, CH_YELLOW },
	{"Starving",   NULL, 0, CONDGRP_FOOD, C_RED },
	{"Fainting",   NULL, 0, CONDGRP_FOOD, CH_RED },

	{"Fainted",    NULL, 0, CONDGRP_FOOD, CH_RED },
	{"Burdened",   NULL, 0, CONDGRP_PW, C_YELLOW },
	{"Strained",   NULL, 0, CONDGRP_PW, C_RED },
	{"Overloaded", NULL, 0, CONDGRP_PW, CH_RED },
	{"Confused",   NULL, 0, -1, C_MAGENTA },

	{"Blessed",    NULL, 0, -1, CH_GREEN },
	{"Cursed",     NULL, 0, -1, C_RED },
	{"Stun",       NULL, 0, -1, C_RED },
	{"Lucky",      NULL, 0, -1, CH_GREEN },
	{"Bleeding",   NULL, 0, -1, CH_RED },

	{"Poisoned",   NULL, 0, -1, CH_RED },
	{"Paralysed",  NULL, 0, -1, CH_RED },
	{"Legs_BAD",   NULL, 0, -1, CH_RED },
	{"LArm_BAD",   NULL, 0, -1, CH_RED },
	{"RArm_BAD",   NULL, 0, -1, CH_RED }
};

int Condition::Get_Group()
{
	return cond_data[type].group;
}

int Condition::Get_Group_Of(int t)
{
	return cond_data[t].group;
}

bool Condition::Spend(int v)
{
	val-=v;
	if (val<0)
	{
		val=0;
		return true;
	}

	return false;
}

bool Condition::Show()
{
	const char *condname=cond_data[type].name;

	/* stop if going over screen border */
	if (is_over_border((int)strlen(condname)))
		return false;

	set_color(cond_data[type].color);
	my_printf("%s ", condname);

	return true;
}

void Condition::Save(Tar_Ball &tb)
{
	tb.Put(type);
	tb.Put(val);
	tb.Put(time);
}

void Condition::Load(Tar_Ball &tb)
{
	type=tb.Get_Next_Value();
	val=tb.Get_Next_Value();
	time=tb.Get_Next_Value();
}
