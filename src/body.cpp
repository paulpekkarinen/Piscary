/**************************************************************************
 * body.cpp --                                                            *
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

//Refactored by Paul K. Pekkarinen

#include "body.h"
#include "output.h"

struct Gender_Data
{
	const char *name;
	const char *art1;
	const char *art2;
	const char *art3;
};

const Gender_Data gender_data[Gender::Amt_Of_Genders]=
{
	{"hermaphrodite", "it", "it", "its"},
	{"male", "he", "him", "his"},
	{"female", "she", "her", "her"},
	{"neutral", "it", "it", "its"}
};

struct Bodypart_Data
{
	const char *name;
	const char *name_flying;
	const char *art;
	real hp_mod;
};

const Bodypart_Data bp_data[Bodypart::Amt_Of_Parts]=
{
	{"head", "head", "is", 0.50},
	{"left hand", "left vane", "is", 0.75},
	{"right hand", "right vane", "is", 0.75},
	{"body", "body", "is", 1.00},
	{"legs", "legs", "are", 0.75}
};

//===

const char *Bodypart::Get_Name()
{
	return bp_data[part].name;
}

const char *Bodypart::Get_Name_Flying()
{
	return bp_data[part].name_flying;
}

const char *Bodypart::Get_Art()
{
	return bp_data[part].art;
}

void Bodypart::Show(int p)
{
	my_printf(" %s", bp_data[p].name);
}

//when calculating initial hitpoints for a creature/player
//each bodypart is issued a (init_hp * hp_bpmod[bodypart]) hitpoints
real Bodypart::Get_HP_Mod()
{
	return bp_data[part].hp_mod;
}

//===

const char *Gender::Get_Art(int a)
{
	const char *rv;

	switch (a)
	{
		case 1: rv=gender_data[type].art1; break;
		case 2: rv=gender_data[type].art2; break;
		case 3: rv=gender_data[type].art3; break;
		default: rv="their"; break;
	}

	return rv;
}

const char *Gender::Get_Name()
{
	return gender_data[type].name;
}
