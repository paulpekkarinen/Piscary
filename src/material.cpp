//material.cpp

//Refactored 15.7.2022 - 21.9.2025 by Paul K. Pekkarinen

#include "colors.h"
#include "items.h"
#include "material.h"

matlist materials[]=
{
	{"leather", 0, 0, 0, 0, CH_DGRAY,
		MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTWEAPON,
		0, 10, 600, 0.8, 0.4},
	{"chain", 1, 0, 0, 0, C_WHITE, MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTWEAPON,
		0, 30, 600, 1.0, 0.5},
	{"wooden", -2, 0, 0, 0, CH_DGRAY, MATSTAT_NOTARMOR,
		0, 10, 500, 0.7, 0.2},
	{"glass", 0, 0, 0, 0, CH_CYAN,
		MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTARMOR | MATSTAT_CANSHARD | MATSTAT_RING,
		0, 4, 680, 0.5, 1.0},
	{"copper", 0, 0, 0, 0, C_YELLOW, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 100, 500, 1.0, 0.5},

	{"iron", 0, 0, 0, 0, C_WHITE, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 150, 500, 1.0, 0.8},
	{"steel", 0, 0,  0, 0, C_WHITE, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 200, 500, 1.1, 0.8},
	{"stone", 1, 0, 0, 0, CH_DGRAY,
		MATSTAT_NOTMISSILE | MATSTAT_NOTARMOR | MATSTAT_NOTBOWS | MATSTAT_CANSHARD,
		0, 5, 500, 3.0, 0.4},
	{"silver", 1, 0, 0, 0, CH_WHITE, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 80, 700, 0.9, 1.1},
	{"golden", 1, 0, 0, 0, CH_YELLOW, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 70, 750, 2.0, 2.2},

	{"crystal", 2, 0, 0, 0, CH_BLUE,
		MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTARMOR | MATSTAT_RING,
		0, 100, 740, 1.0, 1.1},
	{"elven", 3, 0, 0, 0, CH_WHITE, MATSTAT_RING,
		0, 200, 700, 1.0, 1.2},
	{"dwarven", 3, 0, 0, 0, CH_BLUE, MATSTAT_RING,
		0, 200, 750, 1.5, 1.3},
	{"orcish", 4, 0, 0, 0, CH_GREEN, MATSTAT_RING,
		0, 200, 750, 2.5, 1.4},
	{"mithril", 4, 0, 0, 0, CH_YELLOW, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 220, 760, 2.0, 1.5},

	{"platinium", 5, 0, 0, 0, CH_CYAN, MATSTAT_NOTBOWS | MATSTAT_RING,
		0, 220, 770, 2.0, 1.6},
	{"ebony", 1, 0, 0, 0, CH_WHITE, MATSTAT_NOTARMOR,
		0, 20, 750, 2.0, 1.0},
	{"diamond", 7, 0, 0, 0, CH_WHITE,
		MATSTAT_NOTMISSILE | MATSTAT_NOTBOWS | MATSTAT_NOTARMOR | MATSTAT_RING,
		0, 300, 800, 3.0, 1.6},
	{"adamantium", 8, 0, 0, 0, CH_MAGENTA, MATSTAT_RING,
		0, 400, 800, 3.0, 1.8},
	{"white dragonscale", 10, 0, 0, 0, CH_WHITE,
		MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 4000, 800, 3.0, 2.5},

	{"red dragonscale", 10, 0, 0, 0, CH_RED,
		MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 4000, 800, 3.0, 2.5},
	{"green dragonscale", 10, 0, 0, 0, CH_GREEN,
		MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 4000, 800, 3.0, 2.5},
	{"blue dragonscale",  10, 0, 0, 0, CH_BLUE,
		MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 4000, 800, 3.0, 2.5},
	{"black dragonscale", 10, 0, 0, 0, CH_DGRAY,
		MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 4000, 800, 3.0, 2.5},
	{"cloth", 0, 0, 0, 0, CH_WHITE, MATSTAT_SPECIAL,
		0, 2000, 600, 1.0, 1.0},

	{"papyrus", 0, 0, 0, 0, CH_WHITE, MATSTAT_SPECIAL,
		0, 2000, 600, 1.0, 1.0},
	{"organic", 0, 0, 0, 0, CH_MAGENTA,
		MATSTAT_NOTARMOR | MATSTAT_NOTMISSILE | MATSTAT_NOTWEAPON | MATSTAT_NOTBOWS,
		0, 2000, 600, 1.0, 1.0},

	{0, 0}
};

int matlist::Get_Value()
{
	return appearprob+dam+hit+speed;
}

//Check if item can be made of this material.
bool matlist::Is_Suitable_For(item_def *i)
{
	if (status & MATSTAT_SPECIAL)
		return false;

	if (i->type==IS_MISWEAPON && i->group==WPN_BOW &&
		(status & MATSTAT_NOTBOWS))
			return false;

	if (i->type==IS_MISSILE && (status & MATSTAT_NOTMISSILE))
		return false;

	if ((i->type==IS_WEAPON2H || i->type==IS_WEAPON1H) &&
		(status & MATSTAT_NOTWEAPON))
			return false;

	if ((i->type==IS_SHIELD || i->type==IS_BRACELET) &&
		(status & MATSTAT_NOTWEAPON))
			return false;

	if (i->type==IS_RING && !(status & MATSTAT_RING))
		return false;

	if (i->type==IS_ARMOR && (status & MATSTAT_NOTARMOR))
		return false;

	return true;
}

void matlist::Apply_Item_Mods(item_def *i)
{
	/* durability */
	i->turnsleft=durability;

	/* apply the material weight modifier */
	i->weight=(int16u)(wmod * i->weight);

	/* apply the damage and armorvalue modifiers */
	if (i->type == IS_WEAPON2H || i->type == IS_WEAPON1H)
		i->meldam_mod+=dam;

	if (i->type == IS_MISWEAPON || i->type==IS_MISSILE)
		i->misdam_mod+=dam;

	if (i->type == IS_ARMOR || i->type == IS_SHIELD || i->type == IS_BRACELET)
		i->ac+=dam;

	i->dv+=dv;
}
