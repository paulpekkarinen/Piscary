/**************************************************************************
 * node.cpp --                                                            *
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

//Refactored 20.8.2022 - 19.10.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "dice.h"
#include "featlev.h"
#include "gameview.h"
#include "node.h"
#include "output.h"
#include "storage.h"
#include "terrain.h"
#include "world.h"

using std::string;

Levelnode::~Levelnode()
{
	delete level;
}

void Levelnode::Display_Data(int dung)
{
	string vis;

	if (!(flags & LEVEL_VISITED))
		vis="not visited.";
	else
		vis="visited.";

	const char *dname=dungeonlist[dung].name;
	const int i=leveldata->index;

	if (level==0)
		my_printf("Level %d: '%s' of '%s' not yet created.\n", i, leveldata->name, dname);
	else
	{
		my_printf("Level %d: '%s' of '%s' is %s\n", i, leveldata->name, dname, vis.c_str());
		level->Display_Data();
	}

	//show portals defined in the data
	Display_Portal_Pair(leveldata->linkfrom1, leveldata->linkto1);
	Display_Portal_Pair(leveldata->linkfrom2, leveldata->linkto2);
}

void Levelnode::Display_Portal_Pair(Level *from, Level *to)
{
	if (from!=0)
		my_printf("Exits to %s.\n", from->name);
	if (to!=0)
		my_printf("Leads to %s.\n", to->name);
}

bool Levelnode::Remake_Level()
{
	flags &= (0xffffffff ^ LEVEL_VISITED); //set level not visited again
	delete level;
	player.num_levels--; //hack to prevent overflow in number of levels
	return Visit();
}

//Returns true if new level was made.
bool Levelnode::Visit()
{
	bool rv;

	if (!(flags & LEVEL_VISITED))
	{
		flags |= LEVEL_VISITED;
		int dtype=leveldata->dtype;

		//if random, select from randomly selectable level types
		if (dtype==DTYPE_RANDOM)
		{
			dtype=RANDU(NUM_DUNGEONTYPES);
		}

		//determine random size and basetile
		Plane p(Get_Random_Level_Size(dtype));
		const int basetile=Get_Basetile(dtype);

		//if not visited yet, create the level, note that a level is agnostic
		//of type etc. data which is in the leveldata struct
		Feature_Level *flevel=new Feature_Level(
			p.width, p.height, basetile, leveldata->danger);

		//downgrade the class for the node list, because creation routines
		//are not needed during the gameplay
		level=flevel;

		//set gameview stats before creating level, otherwise we init order fiasco
		gameview.Enter_New_Level(level);

		flevel->Create(dtype);

		player.num_levels++;

		rv=true;
	}
	else
	{
		//when entering already created level, also have to point gameview
		//to that level
		gameview.Enter_New_Level(level);
		rv=false;
	}

	//copy objects, room ids etc. to gameview after the level is created
	//or visited again
	level->Refresh_Gameview();

	return rv;
}

int Levelnode::Get_Basetile(int type)
{
	int rv;

	switch (type)
	{
		case DTYPE_OUTWORLD:
		case DTYPE_TOWN:
			rv=TYPE_GRASS;
		break;
		default:
			rv=TYPE_WALLIP;
		break;
	}

	return rv;
}

Plane Levelnode::Get_Random_Level_Size(int type)
{
	// set level size first to the minimum sizes
	int sx=MINSIZEX + RANDU(MAXSIZEX-MINSIZEX);
	int sy=MINSIZEY + RANDU(MAXSIZEY-MINSIZEY);

	//check special cases and adjust the size
	switch (type)
	{
		case DTYPE_OUTWORLD:
			sx=MAXSIZEX;
			sy=MAXSIZEY;
		break;
		case DTYPE_TOWN:
			sx=MINSIZEX+RANDU(20);
			sy=40;
			if (sy<MINSIZEY) sy=MINSIZEY;
		break;
		case DTYPE_MAZE:
		case DTYPE_MAZE2:
			// ensure that level x and y are not even (for maze type levels)
			if (!(sx % 2)) sx++;
			if (!(sy % 2)) sy++;
		break;
		default: break;
	}

	return Plane(sx, sy);
}

void Levelnode::Save(Tar_Ball &tb)
{
	if (level==0)
		tb.Put(0); //save 0 value if level not visited yet
	else
	{
		tb.Put(1); //or 1 if visited
		level->save(tb); //and save the level data
	}

	tb.Put(flags);
}

void Levelnode::Load(Tar_Ball &tb)
{
	const int v=tb.Get_Next_Value();
	if (v==1)
		level=new level_type(tb); //construct and load the level

	flags=tb.Get_Next_Unsigned();
}

Dungnode::Dungnode(Dungeon *d, int i)
	: dung(d), flags(0), dungindex(i)
{
	//construct level node list
	Level *ptr=dung->levels;
	while (ptr->name)
	{
		levels.push_back(new Levelnode(ptr));
		ptr++;
	}
}

Dungnode::~Dungnode()
{
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		delete (*ii);
	}
}

int Dungnode::Get_Amount_Of_Levels()
{
	return (int)levels.size();
}

Levelnode *Dungnode::Get_Node(int index)
{
	return levels[index];
}

Levelnode *Dungnode::Get_Node_By_Level(const Level *dest)
{
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->Get_Level_Data()==dest)
			return (*ii);
	}

	return 0;
}

bool Dungnode::Visit(int index)
{
	if (dungindex == index && !(flags & DUNGEON_VISITED))
	{
		flags|=DUNGEON_VISITED;
		player.num_places++;
		return true;
	}

	return false;
}

void Dungnode::Display_Data(bool currdung)
{
	const int amt_of_levels=(int)levels.size();

	my_printf("%s (%d levels) ", dung->name, amt_of_levels);

	if ((flags & DUNGEON_VISITED))
		my_printf("visited ");
	else
		my_printf("unknown ");

	if (currdung)
		my_printf("<-");

	my_printf("\n");
}

void Dungnode::Save(Tar_Ball &tb)
{
	//save only data which is needed, the dungeon and index are always static
	//so they don't need to be saved
	tb.Put(flags);

	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
		(*ii)->Save(tb);
}

void Dungnode::Load(Tar_Ball &tb)
{
	flags=tb.Get_Int16u();

	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
		(*ii)->Load(tb);
}
