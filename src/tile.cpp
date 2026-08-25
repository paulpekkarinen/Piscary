/**************************************************************************
 * tile.cpp --                                                            *
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

//Refactored 3.9.2021 - 17.1.2023 Paul K. Pekkarinen

#include "dice.h"
#include "storage.h"
#include "terrain.h"
#include "tile.h"

void cave_type::initialize(int tt)
{
	type=tt;
	flags=terrains[tt].flags;
	sval=0;
	doorfl=0;
	object=OBJECT_NONE;
}

void cave_type::jam_door()
{
	doorfl |= DOOR_STUCK;
	sval=50+RANDU(100);
}

void cave_type::set_stairs(int tt, int8u number)
{
	type = tt;
	//note: remove later
	//flags |= terrains[TYPE_STAIRDOWN].flags;
	doorfl = number;
}

void cave_type::save(Tar_Ball &tb)
{
	tb.Put(flags);
	tb.Put(type);
	tb.Put_Char(sval);
	tb.Put_Char(doorfl);
	tb.Put_Char(object);
}

void cave_type::load(Tar_Ball &tb)
{
	flags=tb.Get_Int16u();
	type=tb.Get_Next_Value();
	sval=tb.Get_Next_Value();
	doorfl=tb.Get_Next_Char();
	object=tb.Get_Next_Char();
}
