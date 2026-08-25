/**************************************************************************
 * dungeon.cpp --                                                         *
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

//Refactored 27.8.2021 - 22.8.2026 Paul K. Pekkarinen

#include "dice.h"
#include "dungeon.h"
#include "terrain.h"

//This is a list of dungeons in Saladir
Dungeon dungeonlist[dng::Max_Dungeons]=
{
	{"Salmorrian mountains",
	"Mountains",
	"These mountains are a famous fishing location, if not a bit dangerous.",
	1, 1,
	{
		TYPE_DUNGEON2, dng::Top, dng::Primitive,
		TYPE_DUNGEON1, dng::Top, dng::Thanthol,
		TYPE_DUNGEON3, dng::Top, dng::Abyss,
		TYPE_TOWN1, dng::Top, dng::Santhel,
		-1, -1, -1
	}, 0},

	{"Very primitive dungeon",
	"Primitive",
	"This dungeon looks dull and boresome.",
	4, 4,
	{-1, -1, -1}, 1},

	{"Caverns of Tha'nthol",
	"Caverns",
	"There are the famous cavers of Tha'nthol, the bored elf wizard.",
	8, 10,
	{-1, -1, -1}, 1},

	{"Ranuan Abyss",
	"Abyss",
	"A chaotic place of danger and despair.",
	7, 8,
	{-1, -1, -1}, 1},

	{"Town of Santhel",
	"Santhel",
	"A small town with one castle and few houses. Oddly there're only few people here.",
	3, 3,
	{-1, -1, -1}, 0},
};

int Dungeon::Get_Amount_Of_Levels()
{
	if (min_levels==max_levels)
		return max_levels;

	return random_number(min_levels, max_levels);
}
