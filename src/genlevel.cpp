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
#include "randgen.h"
#include "rooms.h"
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

	if(RANDU(100) < dl)
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

/*
** This creates up and down stairs in the level
*/
void Genlevel::Create_Stairs()
{
	/* do we need up stair (first) */
	int px=0;
	int py=0;

	bool doup1=false;
	bool doup2=false;
	bool dodown2=false;
	bool dodown1=false;
	bool doout=false;

	Level *levptr=world->Get_Level_Data();

	if (levptr->linkfrom1) doup1=true;
	if (levptr->linkto1) dodown1=true;
	if (levptr->linkfrom2) doup2=true;
	if (levptr->linkto2) dodown2=true;

	if (levptr->outx && levptr->outy) doout=true;

	const bool first_town_level=world->Is_First_Level_Of_Town();
	Coord c;

	if (first_town_level)
	{
		doup1=false;
		doup2=false;
		doout=false;
	}

	/* up stair 1 */
	if (doup1)
	{
		c=Create_Stairs_Up(STAIRUP1);

		if (player.lastdir==STAIRDOWN1)
		{
			px=c.x;
			py=c.y;
		}
	}

	/* down stair */
	if (dodown1)
	{
		c=Create_Stairs_Down(STAIRDOWN1, first_town_level);

		if (player.lastdir==STAIRUP1)
		{
			px=c.x;
			py=c.y;
		}
	}

	if (doup2)
	{
		c=Create_Stairs_Up(STAIRUP2);

		if (player.lastdir==STAIRDOWN2)
		{
			px=c.x;
			py=c.y;
		}
	}

	/* down stair */
	if (dodown2)
	{
		c=Create_Stairs_Down(STAIRDOWN2, first_town_level);

		if (player.lastdir==STAIRUP2)
		{
			px=c.x;
			py=c.y;
		}
	}

	if (doout)
	{
		c=Create_Stairs_Up(STAIROUT);

		if (player.lastdir==STAIROUT)
		{
			px=c.x;
			py=c.y;
		}
	}

	player.Set_Location(px, py);
}

Coord Genlevel::Create_Stairs_Down(int8u number, bool first_town_level)
{
	bool READY=false;
	Coord c;
	while (!READY)
	{
		c=find_random_location(this, 1);

		if (first_town_level)
		{
			if (Get_Terrain(c)==TYPE_ROOMFLOOR)
			{
				loc[c.y][c.x].stairs_down(number);
				READY=true;
			}
		}
		else
		{
			if (Is_Passable(c))
			{
				loc[c.y][c.x].stairs_down(number);
				READY=true;
			}
		}
	}

	return c;
}

Coord Genlevel::Create_Stairs_Up(int8u number)
{
	Coord c=find_random_location(this, 1);

	loc[c.y][c.x].stairs_up(number);

	return c;
}

void Genlevel::Create_Trap(int type, const Coord &c)
{
	//this location already has a trap
	if (Has_Object(c, OBJECT_TRAP))
		return;

	int16u trapflag;
	if (Is_Door(c))
		trapflag=TRAPF_DOORTRAP;
	else
		trapflag=TRAPF_CAVETRAP;

	traps.Create(type, c, trapflag);

	//flag that this location has a trap of some kind
	Set_Object(c, OBJECT_TRAP);
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
