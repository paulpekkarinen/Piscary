/**************************************************************************
 * factory.cpp --                                                         *
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

//Refactored 3.6.2022 - 26.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "classes.h"
#include "codex.h"
#include "dice.h"
#include "factory.h"
#include "gameview.h"
#include "invnode.h"
#include "itemdata.h"
#include "itempack.h"
#include "material.h"
#include "message.h"
#include "names.h"
#include "roleplay.h"
#include "scrolls.h"
#include "spot.h"
#include "world.h"

Factory factory; //global instance

struct Appearlist
{
	int dungeon_type;
	int LEVEL;
	int X;
	int Y;
};

/* Descriptions:
	Dungeon type,
	Dungeon level,
	x, y, (0 if random)
*/

/* THIS MUST BE IN THE SAME ORDER AS THE NPC LIST!!! */

Appearlist appearlist[] =
{
	{ PLACE_DPRIMITIVE,	1, 0, 0 },
	{ PLACE_TSANTHEL, 	1, 0, 0 },
	{ PLACE_DPRIMITIVE, 4, 0, 0 },
	{ PLACE_TSANTHEL, 	1, 0, 0 },
	{ 0, 0, 0, 0 }, /* end of the list */
};

/* Create a monster */
void Factory::Add_Monster(level_type *level, int x, int y, int type)
{
	being *newptr=New_Empty_Monster(type);
	level->crew.Add_Monster(newptr);
	Monster_Initrandom(newptr, type);

	Coord c(x, y);
	Plant_Monster(newptr, c);
	Monster_Postgeneration(level, newptr);
}

void Factory::Add_Random_Monster(level_type *level, const Coord &c)
{
	const int type=random_number(0, Species::Max_Amount-1);

	Add_Monster(level, c.x, c.y, type);
}

void Factory::Add_Shopkeeper(level_type *level, int roomnum)
{
	/* get a random race */
	int rrace;
	while (1)
	{
		rrace=RANDU(mucho.num_npcraces);

		/* no chaotic or animal shopkeepers! :) */
		if (npc_races[rrace].align!=CHAOTIC && !(npc_races[rrace].behave & BEHV_ANIMAL))
			break;
	}

	/* get a random shopkeeper template */
	const int index=RANDU(mucho.num_shopkeepers);

	being *b=New_Empty_Monster(rrace);
	level->crew.Add_Monster(b);

	/* copy data */
	b->m=shopkeeper_list[index];
	b->m.race=rrace;

	random_name(b->m.name, CNAME_MAX-1);
	b->m.name[0]=toupper(b->m.name[0]);

	/* set movement limits */
	Area ar=level->rooms[roomnum].Get_Area();
	ar.Shrink(); //limit inside walls
	b->myarea=ar;

	b->sindex=0;

	level->set_room_owner(roomnum, b);

	/* set initial coordinates to the room in case */
	Coord c=get_random_location(ar);
	Plant_Monster(b, c);
	b->m.status=MST_SHOPKEEPER;

	roleplay.Advance_To_Level(b, b->m.level);

	b->base_hp=npc_races[b->m.race].hp_base;
	b->mana.Initialize(npc_races[b->m.race].sp_base);

	initialize_class(b, b->m.mclass);

	/* add initial money purse */
	b->inv.Add_Gold(4000+RANDU(4000));

	/* use items */
	b->Useitems(level);
}

void Factory::Add_Special_Monsters(level_type *level)
{
	monsterdef *monptr=npc_list;
	Appearlist *aptr=appearlist;

	while ((aptr->dungeon_type!=0) && (aptr->LEVEL!=0))
	{
		if (world->Is_Matching_Place(aptr->dungeon_type, aptr->LEVEL))
		{
			const int sp=monptr->race;

			/* create a new node */
			being *b=New_Empty_Monster(sp);
			level->crew.Add_Monster(b);

			/* just in case, init a random monster */
			Monster_Initrandom(b, sp);

			b->m=*monptr; //this should copy values, because mondef has = operator

			Coord c;
			if (aptr->X==0 || aptr->Y==0)
				c=find_random_location(level, 5);
			else
				c.Set_Location(aptr->X, aptr->Y);

			Plant_Monster(b, c);
			Monster_Postgeneration(level, b);
		}
		monptr++;
		aptr++;
	}
}

void Factory::Init_Scrollitem(int subtype, item_def *i)
{
	/* if type is <0 init a random valuable */
	if (subtype<0 || subtype >= mucho.num_scrolls)
		subtype = RANDU(mucho.num_scrolls);

	//copy item data from scroll template
	i->Clone(&templ_scroll);

	/* check for self written scroll type */
	if (subtype==SCROLL_SELFWRITTEN)
		subtype++;

	i->group=subtype;
	i->pmod1=list_scroll[subtype].group;
	i->pmod2=list_scroll[subtype].spell;
	i->pmod3=list_scroll[subtype].skill;

	/* scroll label in "sname" */
	i->sname=list_scroll[subtype].uname;

	/* scroll realname in "rname" */
	i->rname=list_scroll[subtype].name;

	/* automagically identify known scrolls */
	if (list_scroll[subtype].flags & SCFLAG_IDENTIFIED)
		i->status |= ITEM_IDENTIFIED;
}

void Factory::Init_Moneyitem(int subtype, item_def *i)
{
	/* if type is <0 init a random valuable */
	if (subtype<0 || subtype >= mucho.num_valuables)
		subtype = RANDU(mucho.num_valuables);

	/* set name */
	i->name=valuables[subtype].name;

	/* set other item attributes */
	i->type=IS_MONEY;
	i->group=subtype;
	i->material=valuables[subtype].material;
	i->weight=valuables[subtype].weight;
}

//Returns monster type from char. Assumes that each monster has a dedicated
//output character.
int Factory::Get_Species_From_Char(char ch)
{
	Npcrace *stdmon=npc_races;
	int index=0;

	while (stdmon->name)
	{
		//found it!
		if (stdmon->out==ch)
			return index;

		index++;
		stdmon++;
	}

	return -1; //failed to find the matching species
}

/* init monster struct with random data */
void Factory::Monster_Initrandom(being *newptr, int type)
{
	Npcrace *stdmon=npc_races+type;

	newptr->m.randomize(stdmon, type);

	if (stdmon->behave & BEHV_ANIMAL)
	{
		//note: adds weapons skills for animals???
		newptr->skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 5+RANDU(20));
	}

	newptr->base_hp=npc_races[type].hp_base;
	newptr->mana.Initialize(npc_races[type].sp_base);
}

void Factory::Monster_Postgeneration(level_type *level, being *mptr)
{
	for (int i=0; i<HPSLOT_MAX; i++)
	{
		if (npc_races[mptr->m.race].bodyparts[i]==-1)
		{
			const int slot=eqslot_from_hpslot[i];
			mptr->equips.set_status(slot, EQSTAT_NOLIMB);
		}
	}

	mptr->base_hp=npc_races[mptr->m.race].hp_base;
	mptr->mana.Initialize(npc_races[mptr->m.race].sp_base);

	roleplay.Advance_To_Level(mptr, mptr->m.level);

	initialize_class(mptr, mptr->m.mclass);

	mptr->Useitems(level);

	roleplay.Calculate_Itembonus(mptr);
}

invnode *Factory::New_Item(const Itempack &ip)
{
	invnode *inode=new invnode;
	inode->i.Clear(); //reset everything to default values

	inode->count=ip.amount;

	item_def *item_def_data=0;
	int subtype=ip.subtype;

	if (ip.type==IS_MONEY)
	{
		Init_Moneyitem(subtype, &inode->i);
	}
	else if (ip.type==IS_ARMOR)
	{
		item_def_data=armor;
		if (subtype < 0 || subtype>=mucho.num_armors)
			subtype = RANDU(mucho.num_armors);
	}
	else if (ip.type==IS_WEAPON1H || ip.type==IS_WEAPON2H || ip.type==IS_MISWEAPON)
	{
		item_def_data=weapons;

		if (subtype < 0 || subtype>=mucho.num_weapons)
			subtype = RANDU(mucho.num_weapons);
	}
	else if (ip.type==IS_LIGHT || ip.type==IS_FOOD || ip.type==IS_CONTAINER
		|| ip.type==ISMG_MISCITEM )
	{
		item_def_data=miscitems;

		if (subtype < 0 || subtype>=mucho.num_miscitems)
			subtype = RANDU(mucho.num_miscitems);
	}
	else if (ip.type==IS_SPECIAL)
	{
		item_def_data=SPECIAL_ITEMS;
		if (subtype < 0 || subtype>=mucho.num_specials)
			subtype = RANDU(mucho.num_specials);
	}
	else if (ip.type==IS_SCROLL)
	{
		Init_Scrollitem(subtype, &inode->i);
	}
	else
	{
		msg.vnewmsg(CHB_RED, "Error: Item type %d not supported.", ip.type);

		delete inode; //remember to delete the created object
		return 0;
	}

	if (item_def_data)
	{
		inode->i.Clone(&item_def_data[subtype]);

		//random material if -1, also sets values that material affect
		Set_Material(&inode->i, ip.material);

		if (inode->i.type==IS_CONTAINER)
		{
			/* container gets created here! */
			inode->i.inv = new inventory;

			if (!inode->i.inv)
			{
				msg.vnewmsg(CHB_RED, "Failed to initialize container %s.",
					inode->i.name.c_str());
			}
		}
	}

	return inode;
}

invnode *Factory::New_Item_From(invnode *src, int count)
{
	/* allocate new node for inventory */
	invnode *ni=new invnode;

	if (!ni)
	{
		msg.newmsg("Factory::New_Item_From(): no memory!", CH_RED);
		return 0;
	}

	ni->i.Clone(&src->i);
	ni->x = src->x;
	ni->y = src->y;
	ni->count = count;

	return ni;
}

invnode *Factory::New_Rock()
{
	return New_Item(Itempack(IS_WEAPON1H, WEAPONS_ROCK, 1, MAT_STONE));
}

being *Factory::New_Empty_Monster(int sp)
{
	being *b=0;

	// create a new empty nondescriptive creature
	try
	{
		b=new being(sp);
	}
	catch (const std::bad_alloc& e)
	{
		panic_exit("Factory::New_Monster: Out of memory");
	}

	return b;
}

void Factory::Plant_Monster(being *b, const Coord &c)
{
	//move to location and place to gameview for first time
	b->Set_Location(c.x, c.y);
	gameview.Put_Monster(b, c);
}

/*
 * Set item cursed/blessed status
 *
 */
void Factory::Set_Alignment(item_def *i)
{
	/* set cursed, uncursed status */
	int prob=player.stat[STAT_LUC].Get() / 2;

	int r=throwdice(1, 100, 0);

	//note: sets status, with alignment which isn't set...
	if (r <= prob)
		i->status |= ITEM_CURSED;
	else if (r >= 100-prob)
		i->status |= ITEM_BLESSED;

	/* set item religious alignment
	 * note: item align is always set to NEUTRAL
	 */
	i->align = NEUTRAL;
}

/*
 * This sets the material for the item
 *
 */
void Factory::Set_Material(item_def *i, int material)
{
	/* don't touch artifacts */
	if (i->status & ITEM_ARTIFACT || i->type == IS_SPECIAL)
		return;

	Set_Alignment(i);

	if (i->type == IS_FOOD || i->type == IS_CONTAINER ||
		i->type == IS_SCROLL)
		return;

	if (i->type == IS_LIGHT)
	{
		i->material=MAT_WOOD;
		return;
	}

	if (i->type == IS_POTION)
	{
		i->material=MAT_GLASS;
		return;
	}

	int mat;

	//note: pmod4 is not used, but assigned a random value when the material is fabric,
	//in output_items it's used as color value, but the code is commented out
	if (i->material==MAT_FABRIC)
	{
		i->pmod4=1+RANDU(15);
		return;
	}

	if (material==-1)
	{
		/* find a material */
		while (1)
		{
			mat=RANDU(mucho.num_materials);
			const int prob=throwdice(10, 100, player.stat[STAT_LUC].Get());

			/* if rand value smaller than appearing probability, continue */
			if (prob >= materials[mat].appearprob)
			{
				/* accept only correct types of material */
				if (materials[mat].Is_Suitable_For(i)==false)
					continue;

				if (materials[mat].appearprob >= 750)
				{
					msg.newmsg("Somehow you feel fortunate.", CH_GREEN);
				}
				break;
			}
		}
	}
	else
		mat=material;

	/* apply the material to item */
	i->material=mat;

	//apply items modifiers
	materials[mat].Apply_Item_Mods(i);
}
