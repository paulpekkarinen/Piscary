/**************************************************************************
 * theme.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 20.8.2022 - 22.8.2026 Paul K. Pekkarinen

#include "dice.h"
#include "dungeon.h"
#include "geometry.h"
#include "output.h"
#include "terrain.h"
#include "theme.h"

int Theme::Get_Basetile()
{
	int rv;

	switch (theme)
	{
		case Outworld:
		case Town:
			rv=TYPE_GRASS;
		break;
		default:
			rv=TYPE_WALLIP;
		break;
	}

	return rv;
}

Plane Theme::Get_Random_Level_Size()
{
	// set level size first to the minimum sizes
	int sx=MINSIZEX + RANDU(MAXSIZEX-MINSIZEX);
	int sy=MINSIZEY + RANDU(MAXSIZEY-MINSIZEY);

	//check special cases and adjust the size
	switch (theme)
	{
		case Outworld:
			sx=MAXSIZEX;
			sy=MAXSIZEY;
		break;
		case Town:
			sx=MINSIZEX+RANDU(20);
			sy=40;
			if (sy<MINSIZEY) sy=MINSIZEY;
		break;
		case Maze:
		case Old_Maze:
			// ensure that level x and y are not even (for maze type levels)
			if (!(sx % 2)) sx++;
			if (!(sy % 2)) sy++;
		break;
		default: break;
	}

	return Plane(sx, sy);
}

int Theme::Get_Random()
{
	return random_number(0, Old_Maze);
}

int Theme::Get_Roomy()
{
	if (sometimes()) return Roomy_Doors;
	return Roomy;
}

int Theme::Get_Random_Level_Type(int dung, int depth, int max_depth)
{
	int rv;

	switch (dung)
	{
		case dng::Mountains: rv=Outworld; break;
		case dng::Primitive:
			if (depth==0 || depth==max_depth-1)
				rv=Roomy;
			else
				rv=Roomy_Doors;
		break;
		case dng::Thanthol:
			if (depth<4 || depth>max_depth-3)
				rv=Old_Maze;
			else
				rv=Get_Random();
		break;
		case dng::Abyss:
			if (depth==0 || depth==2)
				rv=Roomy;
			else if (depth==1 || depth==3)
				rv=Roomy_Doors;
			else
				rv=Get_Random();
		break;
		case dng::Santhel:
			if (depth==0) rv=Town;
			else rv=Get_Roomy();
		break;
		default: rv=Roomy; break;
	}

	return rv;
}
