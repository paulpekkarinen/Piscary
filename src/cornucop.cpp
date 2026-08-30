/**************************************************************************
 * cornucop.cpp --                                                        *
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

//Refactored 7.6.2023 - 27.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "cornucop.h"
#include "dice.h"
#include "factory.h"
#include "itempack.h"
#include "items.h"
#include "material.h"
#include "purse.h"
#include "skills.h"
#include "skillset.h"

const Itempack Cornucopia::starting_items[]=
{
	{IS_MONEY, MONEY_GOLD, 120, -1},

	{IS_WEAPON1H, WEAPONS_LONGBOW, 1, -1},
	{IS_WEAPON1H, WEAPONS_ARROW, 24, -1},

	{IS_LIGHT, MISCI_LANTERN, 1, -1},
	{IS_WEAPON1H, WEAPONS_STAFF, 1, MAT_ELVEN},
	{-1, -1, -1, -1}
};

void Cornucopia::Armour(int ar)
{
	inv.Create_Item(Itempack(IS_ARMOR, ar, 1, -1));
}

void Cornucopia::Chain_Armour(int ar)
{
	inv.Create_Item(Itempack(IS_ARMOR, ar, 1, MAT_CHAIN));
}

/* find a best weapon skill and give an item for that skill */
bool Cornucopia::Giveskillweapon(skillset &skills)
{
	int high=0, now=0, skill=SKILL_ENDLIST;
	int material;

	for (int i=0; i<NUM_WEAPONSKILLS; i++)
	{
		now=skills.check(SKILLGRP_WEAPON, i);
		if (now > high)
		{
			high=now;
			skill=i;
		}
	}

	if (skill==SKILL_ENDLIST)
		return false;

	/* no weapon for weaponless combat */
	if (skill==SKILL_HAND)
		return true;

	if (skill==SKILL_THROW)
	{
		inv.Add_Item(factory.New_Rock());
		return true;
	}

	int max_search=100000;
	while (max_search>0)
	{
		now=RANDU(mucho.num_weapons);

		if (weapons[now].group == skill)
			break;

		max_search--;
	}

	//somehow failed after max search times
	if (max_search<=0)
		return false;

	high=1+RANDU(100);
	if (high < 33)
		material = MAT_STEEL;
	else if (high < 66)
		material = MAT_IRON;
	else
		material = MAT_COPPER;

	inv.Create_Item(Itempack(IS_WEAPON1H, now, 1, material));

	return true;
}

void Cornucopia::Hunter_Items()
{
	One_Handed_Weapon(WEAPONS_DAGGER, 2, MAT_IRON);
	One_Handed_Weapon(WEAPONS_SHORTBOW, 1, MAT_ELVEN);
	One_Handed_Weapon(WEAPONS_ARROW, 10+RANDU(15), MAT_ELVEN);

	Armour(ARMOR_PANTS);
	Armour(ARMOR_SHIRT);
}

/* initialize player inventory */
void Cornucopia::Initpack(skillset &skills)
{
	int index=0;
	while (starting_items[index].type!=-1)
	{
		inv.Create_Item(starting_items[index]);

		index++;
	}

	Giveskillweapon(skills);
}

void Cornucopia::Knight_Items()
{
	One_Handed_Weapon(WEAPONS_CLAYMORE, 1, MAT_IRON);

	Chain_Armour(ARMOR_HELMET);
	Chain_Armour(ARMOR_CUIRASS);
	Chain_Armour(ARMOR_GREAVES);
	Chain_Armour(ARMOR_PAULDRONL);
	Chain_Armour(ARMOR_PAULDRONR);
}

void Cornucopia::Merchant_Items()
{
	One_Handed_Weapon(WEAPONS_DAGGER, 2, MAT_IRON);
	//One_Handed_Weapon(WEAPONS_SHORTBOW, 1, MAT_ELVEN);
	One_Handed_Weapon(WEAPONS_ARROW, 25+RANDU(25), MAT_ELVEN);

	Armour(ARMOR_PANTS);
	Armour(ARMOR_SHIRT);
}

void Cornucopia::Ninja_Items()
{
	One_Handed_Weapon(WEAPONS_KATANA, 1, MAT_IRON);
	One_Handed_Weapon(WEAPONS_SHORTBOW, 1, MAT_WOOD);
	One_Handed_Weapon(WEAPONS_ARROW, 10+RANDU(15), MAT_WOOD);

	Armour(ARMOR_SANDALS);
	Armour(ARMOR_HOODEDCLOAK);
}

void Cornucopia::Rogue_Items()
{
	Itempack ip(IS_WEAPON1H, WEAPONS_DAGGER, 2, MAT_IRON);

	if (RANDU(100) < 50)
		ip.subtype=WEAPONS_TANTO;

	inv.Create_Item(ip);

	ip.subtype=WEAPONS_SHORTSWORD;
	ip.amount=1;

	inv.Create_Item(ip);
}

void Cornucopia::One_Handed_Weapon(int st, int amt, int mat)
{
	inv.Create_Item(Itempack(IS_WEAPON1H, st, amt, mat));
}
