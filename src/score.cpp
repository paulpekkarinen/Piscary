/*
 * Score.cpp for Legend of Saladir
 *
 * (C)1997/1998 by Erno Tuomainen
 *
 * Score type
 */

//Refactored 15.12.2021 - 4.9.2023 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include "avatar.h"
#include "creature.h"
#include "names.h"
#include "output.h"
#include "score.h"
#include "storage.h"
#include "world.h"

//the default constructor doesn't have to do anything useful, because it's
//used only when loading score data, but initialize values to defaults anyway
Score::Score()
	: copper(0), quests(0),
	dindex(0), places(0), levels(0),
	moves(0), kills(0), final(0)
{
	mytime = time(NULL);
}

//for testing the score list
Score::Score(const monsterdef &m, int a)
	: copper(10), quests(1),
	dindex(0), places(1), levels(1),
	moves(20), kills(5), final(a)
{
	monsu=m; //monsterdef doesn't have copy constructor, has to assign
	mytime = time(NULL);
	deathreason="Duck.";
}

Score::Score(const playerinfo &plr)
	: deathreason(get_player_killer()),
	copper(plr.inv.Get_Wealth()), quests(0),
	dindex(world->Get_Dungeon_Index()), places(plr.num_places), levels(plr.num_levels),
	moves(plr.movecount), kills(plr.num_kills)
{
	monsu=plr.m; //monsterdef doesn't have copy constructor, has to assign
	mytime = time(NULL);
	final = copper / 8;
	final += levels * 2;
	final += places * 10;
}

bool Score::Is_Better_Or_Equal(Score *s)
{
	if (final>s->final) return true;
	else if (final==s->final)
	{
		if (moves<s->moves) return true;
		else if (moves==s->moves)
		{
			if (mytime<s->mytime) return true;
			else if (mytime==s->mytime)
			{
				return true; //exactly the same result
			}
		}
	}

	return false;
}

bool Score::Is_List_Match(Score *s)
{
	if (s==0) return false;

	if (mytime==s->mytime && monsu.name.compare(s->monsu.name)==0)
		return true;
	return false;
}

void Score::Load(Tar_Ball &tb)
{
	monsu.Load(tb);
	deathreason=tb.Get_Next_String();

	copper=tb.Get_Next_Value();
	quests=tb.Get_Next_Value();
	dindex=tb.Get_Next_Value();
	places=tb.Get_Next_Value();
	levels=tb.Get_Next_Value();

	moves=tb.Get_Next_Value();
	kills=tb.Get_Next_Value();
	final=tb.Get_Next_Value();

	//note: how to load time?
}

void Score::Save(Tar_Ball &tb)
{
	monsu.Save(tb);
	tb.Put_String(deathreason);

	tb.Put(copper);
	tb.Put(quests);
	tb.Put(dindex);
	tb.Put(places);
	tb.Put(levels);

	tb.Put(moves);
	tb.Put(kills);
	tb.Put(final);

	//note: how to save time?
}

void Score::Show(int index, int y)
{
	static char timestr[80]={0};

	move(y, 0);

	tm *loctime=localtime(&mytime);

	strftime(timestr, sizeof(timestr), "%H:%M %d/%m/%C%y", loctime);

	my_printf("%2d %7ld %s %s (l%d %s %s) [%dm] - %s",
		index, final, monsu.name.c_str(), monsu.desc.c_str(),
		monsu.level,
		gendertext[monsu.gender],
		npc_races[monsu.race].name,
		moves,
		deathreason.c_str());

	my_printf("    %s\n", asctime(loctime));
}
