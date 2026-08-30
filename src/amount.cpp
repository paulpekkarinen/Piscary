//Legend of Saladir - amount.cpp

//Refactored 15.7.2022 - 27.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "classes.h"
#include "creature.h"
#include "dungeon.h"
#include "itemdata.h"
#include "items.h"
#include "material.h"
#include "mondef.h"
#include "names.h"
#include "output.h"
#include "roomgen.h"
#include "scrolls.h"
#include "world.h"

void Amount::Initialize()
{
	num_armors=0;
	item_def *ptr=armor;
	while (ptr->name.size()>0)
	{
		num_armors++;
		ptr++;
	}

	num_artifacts=0;
	ptr=artifacts;
	while (ptr->name.size()>0)
	{
		num_artifacts++;
		ptr++;
	}

	num_dungeons=dng::Max_Dungeons;

	num_lairs=0;
	Roomtemplate *lairptr=lairtemplates;
	while (lairptr->sx && lairptr->sy)
	{
		num_lairs++;
		lairptr++;
	}

	num_materials=0;
	matlist *matptr=materials;
	while (matptr->name)
	{
		num_materials++;
		matptr++;
	}

	num_miscitems=0;
	ptr=miscitems;
	while (ptr->name.size()>0)
	{
		num_miscitems++;
		ptr++;
	}

	num_npcraces=0;
	Npcrace *rptr=npc_races;
	while (rptr->name)
	{
		num_npcraces++;
		rptr++;
	}

	num_rooms=0;
	lairptr=towntemplates;
	while (lairptr->sx && lairptr->sy)
	{
		num_rooms++;
		lairptr++;
	}

	//don't count the first scroll
	num_scrolls=0;
	Scroll *scptr=list_scroll+1;
	while (scptr->name)
	{
		num_scrolls++;

		//initialize scrollnames
		random_scrollname(scptr->uname, ITEM_NAMEMAX);
		scptr++;
	}

	num_shopkeepers=0;
	monsterdef *mptr=shopkeeper_list;
	while (mptr->desc.size()>0)
	{
		num_shopkeepers++;
		mptr++;
	}

	num_specials=0;
	ptr=SPECIAL_ITEMS;
	while (ptr->name.size()>0)
	{
		num_specials++;
		ptr++;
	}

	num_valuables=0;
	valuable_data *vptr=valuables;
	while (vptr->name[0]!=0)
	{
		num_valuables++;
		vptr++;
	}

	num_weapons=0;
	ptr=weapons;
	while (ptr->name.size()>0)
	{
		num_weapons++;
		ptr++;
	}
}

void Amount::Show_Data()
{
	my_printf("%d weapons\n%d artifacts\n%d armor\n%d scrolls\n",
		num_weapons, num_artifacts, num_armors, num_scrolls);
	my_printf("%d classes\n%d races\n%d valuables\n",
		MAX_CLASSES, num_npcraces, num_valuables);
	my_printf("%d lair templates\n%d general room templates\n",
		num_lairs, num_rooms);
	my_printf("%d materials\n", num_materials);
	my_printf("%d shopkeeper templates\n%d dungeon specials\n",
		num_shopkeepers, num_specials);
}
