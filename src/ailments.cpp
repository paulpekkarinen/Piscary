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

Ailments::~Ailments()
{
	init();
}

Condition* Ailments::add(int cond, int value)
{
	/* remove all Ailments with same group as the new condition */
	const int g=Condition::Get_Group_Of(cond);
	if (g>=0)
		delete_group(g);

	Condition *c=find(cond);

	//add new if it doesn't exist
	if (c==0)
	{
		c=new Condition(cond);
		conds.push_back(c);
	}

	c->Change_Value(value);

	//note: time is not used, maybe value should be strenght of the condition
	//and time the current "value" decreased each turn by amount of slots
	//c->time=0;

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
		if ((*ii)->Get_Group()==group)
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
		if (cnd->Get_Type()==cond) return cnd;
	}

	return 0;
}

int Ailments::get_value(int cond)
{
	Condition *cnd=find(cond);
	if (cnd==0) return 0;
	return cnd->Get_Value();
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
		if((*ii)->Get_Type()==cond)
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
		//stop showing if goes over the screen border
		if ((*ii)->Show()==false)
			break;
	}
}

void Ailments::handle(Actor *actor, int slots)
{
	citr ii = conds.begin();

	while (ii != conds.end())
	{
		const bool done=(*ii)->Spend(slots);

		if ((*ii)->Get_Type()==CONDIT_CONFUSED)
			actor->Handle_Confusion(*ii);

		if (done)
		{
			/* if set, then the condition should be deleted! */
			GAME_NOTIFYFLAGS |= GAME_CONDCHG;

			delete (*ii);
			ii=conds.erase(ii);
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
