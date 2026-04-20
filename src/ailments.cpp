/**************************************************************************
 * ailments.cpp --                                                        *
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

//Refactored 2021 - 8.9.2025 by Paul K. Pekkarinen

#include <cstring>
#include "ailments.h"
#include "avatar.h"
#include "condit.h"
#include "game.h"
#include "message.h"
#include "output.h"
#include "storage.h"

/* a list of condition descriptions */
const Ailments::cond_desc Ailments::cond_list[]=
{
   { "Bloated",    NULL, 0, CONDGRP_FOOD, CH_GREEN },
   { "Satiated",   NULL, 0, CONDGRP_FOOD, C_GREEN },
   { "Hungry",     NULL, 0, CONDGRP_FOOD, CH_YELLOW },
   { "Starving",   NULL, 0, CONDGRP_FOOD, C_RED },
   { "Fainting",   NULL, 0, CONDGRP_FOOD, CH_RED },
   { "Fainted",    NULL, 0, CONDGRP_FOOD, CH_RED },
   { "Burdened",   NULL, 0, CONDGRP_PW, C_YELLOW },
   { "Strained",   NULL, 0, CONDGRP_PW, C_RED },
   { "Overloaded", NULL, 0, CONDGRP_PW, CH_RED },
   { "Confused",   NULL, 0, -1, C_MAGENTA },
   { "Blessed",    NULL, 0, -1, CH_GREEN },
   { "Cursed",     NULL, 0, -1, C_RED },
   { "Stun",       NULL, 0, -1, C_RED },
   { "Lucky",      NULL, 0, -1, CH_GREEN },
   { "Bleeding",   NULL, 0, -1, CH_RED },
   { "Poisoned",   NULL, 0, -1, CH_RED },
   { "Paralysed",  NULL, 0, -1, CH_RED },
   { "Legs_BAD",   NULL, 0, -1, CH_RED },
   { "LArm_BAD",   NULL, 0, -1, CH_RED },
   { "RArm_BAD",   NULL, 0, -1, CH_RED },
   { "Dead!",      NULL, 0, -1, CHB_RED }
};

Ailments::~Ailments()
{
	init();
}

Condition* Ailments::add(int cond, int value)
{
	/* remove all Ailments with same group as the new condition */
	if (cond_list[cond].group >= 0)
		delete_group(cond_list[cond].group);

	Condition *c=find(cond);

	//add new if it doesn't exist
	if (c==0)
	{
		c=new Condition;
		conds.push_back(c);
	}

	/* init condition */
	c->type=cond;
	c->val+=value;
	c->time=0;

	if (this==&player.conditions)
		GAME_NOTIFYFLAGS |= GAME_CONDCHG;

	return c;
}

void Ailments::delete_group(int group)
{
	if (this==&player.conditions)
		GAME_NOTIFYFLAGS |= GAME_CONDCHG;

	citr ii = conds.begin();

	while (ii != conds.end())
	{
		if(cond_list[(*ii)->type].group==group)
		{
			delete (*ii);
			ii=conds.erase(ii);
		}
		else ++ii;
	}
}

Condition* Ailments::find(int cond)
{
	for (citr ii = conds.begin() ; ii != conds.end() ; ++ii)
	{
		Condition *cnd=(*ii);
		if (cnd->type==cond) return cnd;
	}

	return 0;
}

int Ailments::get_value(int cond)
{
	Condition *cnd=find(cond);
	if (cnd==0) return 0;
	return cnd->val;
}

void Ailments::init()
{
	for (citr ii = conds.begin() ; ii != conds.end() ; ++ii)
		delete (*ii);
	conds.clear();
}

void Ailments::remove(int cond)
{
	if (this==&player.conditions)
		GAME_NOTIFYFLAGS |= GAME_CONDCHG;

	citr ii = conds.begin();

	while (ii != conds.end())
	{
		if((*ii)->type==cond)
		{
			delete (*ii);
			ii=conds.erase(ii);
		}
		else ++ii;
	}
}

/* display all set Ailments in the status row */
void Ailments::statshow(int y)
{
	clearline(y);
	gotoxy(0, y);

	for (citr ii = conds.begin() ; ii != conds.end() ; ++ii)
	{
		Condition *cnd=(*ii);

		if (cnd->type > CONDIT_MAX)
		{
			msg.vnewmsg(CH_RED,
				"Error: Illegal condition %d (max=%d) in condition list.",
				cnd->type, CONDIT_MAX);
		}
		else
		{
			/* stop if going over screen border */
			if (is_over_border((int)strlen(cond_list[cnd->type].name)))
				break;

			set_color(cond_list[cnd->type].color);
			my_printf("%s ",cond_list[cnd->type].name);
		}
	}
}

void Ailments::handle(Actor *actor, int slots)
{
	citr ii = conds.begin();

	while (ii != conds.end())
	{
		if((*ii)->type==CONDIT_CONFUSED)
		{
			/* if set, then the condition should be deleted! */
			if (actor->Handle_Confusion(*ii, slots))
			{
				GAME_NOTIFYFLAGS |= GAME_CONDCHG;

				delete (*ii);
				ii=conds.erase(ii);
			}
			else ++ii;
		}
		else ++ii;
	}
}

void Ailments::save(Tar_Ball &tb)
{
	const int amt=(int)conds.size();
	tb.Put(amt);

	for (citr ii = conds.begin() ; ii != conds.end() ; ++ii)
	{
		(*ii)->Save(tb);
	}
}

void Ailments::load(Tar_Ball &tb)
{
	const int amt=tb.Get_Next_Value();

	for (int t=0; t<amt; t++)
	{
		Condition *c=new Condition;
		c->Load(tb);
		conds.push_back(c);
	}
}
