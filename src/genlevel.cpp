/**************************************************************************
 * genlevel.cpp --                                                        *
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

//Refactored 1.3.2023 - 29.8.2023 Paul K. Pekkarinen

#include "avatar.h"
#include "dice.h"
#include "genlevel.h"
#include "rooms.h"
#include "spot.h"
#include "terrain.h"
#include "world.h"

Genlevel::Genlevel(int width, int height, int bt, int danger)
	: level_type(width, height, bt, danger)
{

}

void Genlevel::Add_Room(roomdef &r)
{
	rooms.push_back(r);
}

/* if special is set, allow creating secret door etc. */
bool Genlevel::Create_Door(int x, int y, bool special)
{
	if(Is_Outside(x, y))
		return false;

	/* create a closed door */
	Set_Terrain(x, y, TYPE_DOORCLOS);
	loc[y][x].doorfl=DOOR_CLOSED;

	if(!special)
		return true;

	/* secret door shown with a wall '#' */
	if(RANDU(100) < CHANCE_SECRETDOOR)
		Set_Terrain(x, y, TYPE_DOORSECR);

	/* stuck doors */
	if(RANDU(100) < CHANCE_STUCKDOOR)
		loc[y][x].jam_door();

	/* note: this will eventually calculate the chances for doors to be trapped */

	int dl = danglev;

	/* if null then get the default chance */
	if(!dl)
		dl = CHANCE_DOORTRAP;

	if (RANDU(100) < dl)
	{
		/* create a random trap */
		//note: at the moment this is the only place traps are created, there are no ground traps yet
		Create_Trap(-1, Coord(x, y));
	}

	return true;
}

void Genlevel::Create_Floor(const Coord &c, int floor, int16u flags)
{
	if (Is_Outside(c)) return;

	loc[c.y][c.x].type=floor;
	loc[c.y][c.x].flags=flags;
}

void Genlevel::Create_Stairs(const Coord &c, int tt, int8u number)
{
	loc[c.y][c.x].set_stairs(tt, number);
}

void Genlevel::Create_Wall(const Coord &c, int wall, bool nocarve, bool dark)
{
	if (Is_Outside(c)) return;

	loc[c.y][c.x].type=wall;
	loc[c.y][c.x].flags=terrains[wall].flags;

	if (nocarve)
		loc[c.y][c.x].sval=GENERATE_DONOTCARVE;

	if (dark==false)
		loc[c.y][c.x].flags|=CAVE_LIGHT;
}

void Genlevel::Create_Wall(const Coord &c, int wall)
{
	Create_Wall(c, wall, false, false);
}

void Genlevel::Protect_From_Carving(const Coord &c)
{
	if (Is_Outside(c)) return;
	loc[c.y][c.x].sval=GENERATE_DONOTCARVE;
}
