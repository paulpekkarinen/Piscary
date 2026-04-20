/**************************************************************************
 * invnode.cpp --                                                         *
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

//Refactored 6.6.2023 - 10.4.2026 Paul K. Pekkarinen

#include "geometry.h"
#include "invnode.h"
#include "material.h"
#include "storage.h"

invnode::invnode()
	: x(0), y(0), count(1), slot(-1)
{

}

Coord invnode::Get_Location()
{
	return Coord(x, y);
}

real invnode::Get_Material_Mod()
{
	real matmod;

	if (i.type!=IS_FOOD)
		matmod=materials[i.material].vmod;
	else
		matmod=1.0;

	return matmod;	
}

const char *invnode::Get_Name()
{
	return i.name.c_str();
}

int invnode::Get_Weight()
{
	return i.Get_Weight() * count;
}

void invnode::Set_Location(const Coord &c)
{
	x=c.x;
	y=c.y;
}

void invnode::Save(Tar_Ball &tb)
{
	tb.Put(x);
	tb.Put(y);
	tb.Put(count);
	tb.Put(slot);

	i.Save(tb);
}

void invnode::Load(Tar_Ball &tb)
{
	x=tb.Get_Next_Value();
	y=tb.Get_Next_Value();
	count=tb.Get_Next_Value();
	slot=tb.Get_Next_Value();

	i.Load(tb);
}
