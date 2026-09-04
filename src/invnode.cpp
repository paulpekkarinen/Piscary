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
#include "purse.h"
#include "storage.h"

invnode::invnode()
	: x(0), y(0), count(1), slot(-1), inv(0)
{

}

invnode::~invnode()
{
	if (inv != 0)
	{
		delete inv;
		inv=0;
	}
}

Coord invnode::Get_Location()
{
	return Coord(x, y);
}

real invnode::Get_Material_Mod()
{
	real matmod;

	if (Get_Type()!=IS_FOOD)
		matmod=materials[i.material].vmod;
	else
		matmod=1.0;

	return matmod;
}

const char *invnode::Get_Name()
{
	return i.name.c_str();
}

const char *invnode::Get_Material_Name()
{
	return materials[i.material].name;
}

int invnode::Get_Price()
{
	return i.price;
}

int invnode::Get_Type()
{
	return i.type;
}

//Returns weight of item (1000 is 1kg)
int invnode::Get_Weight()
{
	//return item times amount in stack
	return Get_Weight_Of_One() * count;
}

int invnode::Get_Weight_Of_One()
{
	int unit=i.weight;

	//if container, return itself plus container weight
	if (inv!=0)
		return unit + inv->Get_Weight();

	return unit;
}

bool invnode::Is_Armor()
{
	const int t=Get_Type();

	if (t == IS_ARMOR || t == IS_SHIELD)
		return true;

	return false;
}

bool invnode::Is_Lightsource()
{
	if (Get_Type()==IS_LIGHT)
		return true;

	return false;
}

bool invnode::Is_Set(int32u f)
{
	return i.status & f;
}

bool invnode::Is_Weapon()
{
	const int t=Get_Type();

	if (t == IS_WEAPON1H || t == IS_WEAPON2H ||
		t == IS_MISWEAPON || t == IS_MISSILE)
			return true;

	return false;
}

void invnode::Identify()
{
	i.status |= ITEM_IDENTIFIED;
}

int invnode::Rate()
{
	/* money is good to keep :) */
	if (Get_Type()==IS_MONEY)
		return 1000;

	return materials[i.material].Get_Value();
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

	//if this item has no inventory, save zero
	if (inv==0) tb.Put(0);
	else
	{
		//save 1 and the inventory
		tb.Put(1);
		inv->save(tb);
	}
}

void invnode::Load(Tar_Ball &tb)
{
	x=tb.Get_Next_Value();
	y=tb.Get_Next_Value();
	count=tb.Get_Next_Value();
	slot=tb.Get_Next_Value();

	i.Load(tb);

	const int v=tb.Get_Next_Value();

	//if no inventory, set pointer to zero
	if (v==0) inv=0;
	else
	{
		//or load the inventory
		inv=new inventory;
		inv->load(tb);
	}
}
