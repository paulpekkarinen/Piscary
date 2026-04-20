/**************************************************************************
 * terrain.cpp --                                                         *
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

//Refactored 20.8.2021 - 28.3.2023 Paul K. Pekkarinen

#include "caves.h"
#include "output.h"
#include "terrain.h"

/*
** terrain information
**
** char, color, description
*/
const Terradata terrains[]=
{
  { ' ', C_BLACK,   0, 100, "Dark", }, 
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Path" },
  { '.', CH_DGRAY,	CAVE_PASSABLE, 110, "Slow path" },
  { '<', C_GREEN,	CAVE_PASSABLE, 100, "stairs up" },
  { '>', C_GREEN,	CAVE_PASSABLE, 100, "stairs down" },
  { '+', C_WHITE,	CAVE_DOOR, 100, "closed door" },
  { '+', C_WHITE,	CAVE_DOOR, 100, "locked door" },
  { '/', C_WHITE,	CAVE_PASSABLE|CAVE_DOOR, 100, "open door" },
  { '#', CH_DGRAY,	0, 100, "Wall" }, 
  { '+', C_RED,	    0, 100, "trapped door" },
  { '#', CH_DGRAY,	0, 100, "Wall" },
  { '#', CH_DGRAY,	0, 100, "Wall" },
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Floor" },
  { '.', C_WHITE,	CAVE_PASSABLE, 100, "Passage" },
  { '.', C_GREEN,	CAVE_PASSABLE, 100, "Grass" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '^', CH_DGRAY,	CAVE_MOUNTAIN, 100, "Mountains" },
  { '^', CH_WHITE,	CAVE_MOUNTAIN, 100, "High mountains" },
  { '^', C_RED,	    CAVE_MOUNTAIN, 100, "Volcano" },
  { '~', C_YELLOW,	CAVE_PASSABLE, 160, "Hills" },
  { '=', CH_BLUE,	CAVE_PASSABLE|CAVE_WATER, 140, "Water" },
  { '~', C_BLUE,	CAVE_PASSABLE|CAVE_WATER, 160, "Deep water" },
  { 'T', C_GREEN,	CAVE_PASSABLE, 110, "Forest" },
  { '"', C_GREEN,	CAVE_PASSABLE, 120, "Swamp" },
  { '"', CH_GREEN,	CAVE_PASSABLE, 100, "Plains" },
  { ',', CH_GREEN,	CAVE_PASSABLE, 110, "Grassland" },
  { '.', C_YELLOW,	CAVE_PASSABLE, 80, "Road" },
  { '%', C_YELLOW,	CAVE_PASSABLE, 100, "Cornfield" },
  { '.', CH_DGRAY,	CAVE_PASSABLE, 100, "Field" },
  { '&', CH_WHITE,	CAVE_PASSABLE, 120, "Snow" },
  { 'T', CH_WHITE,	CAVE_PASSABLE, 100, "Trees (snow)" },
  { '"', CH_WHITE,	CAVE_PASSABLE, 100, "Tundra" },
  { '=', CH_WHITE,	CAVE_PASSABLE, 150, "Ice" },
  { '~', CH_RED,	CAVE_PASSABLE, 100, "Lava" },
  { '.', C_RED,		CAVE_PASSABLE, 100, "Hot ground" },
  { 'T', CH_GREEN,	CAVE_PASSABLE, 100, "A Tree" },
  { '|', C_YELLOW,	CAVE_PASSABLE, 100, "Wooden bridge" },
  { '-', C_YELLOW,	CAVE_PASSABLE, 100, "Wooden bridge" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { 'o', C_RED,		CAVE_PASSABLE, 100, "Dungeon" },
  { '*', CH_DGRAY,	CAVE_PASSABLE, 100, "Cave" },
  { 'O', CH_WHITE,	CAVE_PASSABLE, 100, "Large opening" },
  { '#', CH_WHITE,	CAVE_PASSABLE, 100, "Village" },
  { 'O', CH_BLUE,	CAVE_PASSABLE, 100, "Small town" },
  { 'O', CH_RED,	CAVE_PASSABLE, 100, "Town" },
  { 'o', CH_BLUE,	CAVE_PASSABLE, 100, "Tower" },
  { 'O', CH_CYAN,	CAVE_PASSABLE, 100, "Tower" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '^', CH_RED,	CAVE_DOOR, 100, "boulder trap" },
  { '^', C_RED,		CAVE_PASSABLE, 100, "bomb trap" },
  { '^', C_BLUE,	CAVE_PASSABLE, 100, "water trap" },
  { '^', CH_DGRAY,	CAVE_PASSABLE, 100, "rock trap" },
  { '^', C_WHITE,	CAVE_PASSABLE, 100, "pit trap" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { '?', C_WHITE,	0, 100, "nothing" },
  { 0, 0, 0, 0, ""}
};

bool Terratype::Is_Door()
{
	if (type==TYPE_DOORCLOS || type==TYPE_DOOROPEN) return true;
	return false;
}

bool Terratype::Is_Portal()
{
	bool rv;

	//note: towers don't seem to have defines
	switch (type)
	{
		case TYPE_STAIRDOWN:
		case TYPE_STAIRUP:
		case TYPE_DUNGEON1:
		case TYPE_DUNGEON2:
		case TYPE_DUNGEON3:
		case TYPE_VILLAGE:
		case TYPE_TOWN1:
		case TYPE_TOWN2:
			rv=true;
		break;
		default:
			rv=false;
		break;
	}

	return rv;
}

bool Terratype::Is_Stairs()
{
	if (type==TYPE_STAIRDOWN || type==TYPE_STAIRUP) return true;
	return false;
}
