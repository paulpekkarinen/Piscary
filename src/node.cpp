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

#include <format>
#include "avatar.h"
#include "dice.h"
#include "dungeon.h"
#include "featlev.h"
#include "gameview.h"
#include "lexicon.h"
#include "node.h"
#include "output.h"
#include "salamath.h"
#include "storage.h"
#include "terrain.h"
#include "theme.h"
#include "world.h"

using std::format;
using std::string;
using std::vector;

string Level::Get_Name()
{
	bool basename=false;
	string s;

	switch (dungeon)
	{
		case dng::Mountains:
			s=dungeonlist[dungeon].level_name;
		break;
		case dng::Thanthol: basename=true; break;
		case dng::Primitive:
			if (where==dng::Top) s="Top";
			else if (where==dng::Bottom) s="Bottom";
			else basename=true;
		break;
		case dng::Abyss:
			if (where==dng::Top) s="Entrance";
			else basename=true;
		break;
		case dng::Santhel:
			if (where==dng::Top) s="Town";
			else if (where==dng::Middle) s="Dungeons of Santhel";
			else s="Deep dungeon";
		break;
		default: s="Strange place"; break;
	}

	if (basename)
	{
		s=dungeonlist[dungeon].level_name;
		s.append(" ");
		s.append(to_string(depth+1));
	}

	return s;
}

string Level::Get_Data()
{
	string s=format("'{}' at depth {}",
		Get_Name(), depth);

	return s;
}

//===

Levelnode::~Levelnode()
{
	delete level;
}

Portal &Levelnode::Get_Portal(int index)
{
	return portals[index];
}

int Levelnode::Get_Reverse_Portal(int terratype)
{
	if (terratype==TYPE_TOWN1 || terratype==TYPE_TOWN2)
		return TYPE_DARK; //this is border exit

	if (terratype==TYPE_STAIRUP)
		return TYPE_STAIRDOWN;

	//assumes that we have only pair for stairs down left
	return TYPE_STAIRUP;
}

//Returns the stairs number stored in Tile::doorfl, this is needed to
//find the physical stairs location based on its id. Each stairs get
//a number based on its creation order.
int8u Levelnode::Get_Stairs_Number(int id)
{
	int8u si=1; //starts from 1

	for (pitr ii = portals.begin() ; ii != portals.end() ; ++ii)
	{
		if ((*ii).id==id)
			return si;
		si++;
	}

	return 0;
}

bool Levelnode::Has_Portal(int id)
{
	for (pitr ii = portals.begin() ; ii != portals.end() ; ++ii)
	{
		if ((*ii).id==id)
			return true;
	}
	return false;
}

bool Levelnode::Is_First_Level_Of_Town()
{
	if (site.theme==Theme::Town && site.depth==0)
		return true;
	return false;
}

bool Levelnode::Is_Visited()
{
	return (flags & LEVEL_VISITED);
}

void Levelnode::Add_Portal(Portal &p)
{
	portals.push_back(p);
}

//Create physical stairs to level based on portal list.
void Levelnode::Create_Portals(Feature_Level *f)
{
	vector<Coord> prev; //entrances that were created
	int8u num=1;

	for (pitr ii = portals.begin() ; ii != portals.end() ; ++ii)
	{
		Portal &p=(*ii);
		const int tt=p.terrain_type;
		if (tt!=TYPE_DARK)
		{
			Coord c;

			//when creating outworld entrances, distance them away
			//from each other
			if (site.dungeon==dng::Mountains)
				c=New_Dungeon_Location(f, prev);
			else
				c=f->places.Get_Random();

			f->Create_Stairs(c, tt, num);

			prev.push_back(c);
		}
		//stairs id is also given to border exit, but it's
		//just not physically created as stairs
		num++;
	}
}

void Levelnode::Display_Data(int i)
{
	string vis;

	if (Is_Visited())
		vis="visited.";
	else
		vis="not visited.";

	string levname=site.Get_Data();

	if (level==0)
		my_printf("Level %d: %s not yet created.\n", i, levname.c_str());
	else
	{
		my_printf("Level %d: %s is %s\n", i, levname.c_str(), vis.c_str());
		level->Display_Data();
	}

	//show portals defined in the data
	for (pitr ii = portals.begin() ; ii != portals.end() ; ++ii)
	{
		Portal &p=(*ii);
		const int tt=p.terrain_type;

		my_printf("Portal '%s': From %d to %d.\n",
			terrains[tt].desc, p.id, p.dest_id);
	}
}

void Levelnode::Display_Compact_Data(int i)
{
	string s=format("{}/{}: {} th{}",
		i, site.dungeon, site.Get_Name(), site.theme);

	const char *p;
	if (site.where==dng::Top) p="(Top)";
	else if (site.where==dng::Bottom) p="(Bot)";
	else p="(Mid)";

	string ps;
	for (pitr ii = portals.begin() ; ii != portals.end() ; ++ii)
	{
		Portal &po=(*ii);
		const int tt=po.terrain_type;

		ps+=format(" ({}: {}->{})",
			terrains[tt].desc, po.id, po.dest_id);
	}

	my_printf("%s %s %s\n", s.c_str(), p, ps.c_str());
}

Coord Levelnode::New_Dungeon_Location(Feature_Level *f, vector<Coord> &vc)
{
	bool banana=false;
	Coord c;
	Plane area=f->Get_Size();

	for (int t=0; t<5000; t++)
	{
		//skip 5 tiles around the borders of the level
		c=get_random_location(area, 5);
		banana=true; //assume we found a valid location

		//check existing coordinates, if they are closer than 10 tiles,
		//find a new location
		for (vector<Coord>::iterator ii = vc.begin(); ii != vc.end(); ++ii)
		{
			if (get_distance(*ii, c)<10)
			{
				banana=false;
				break;
			}
		}

		if (banana) break;
	}

	//in case of failure, put the stairs to a special location
	if (banana==false)
	{
		const int x=(int)vc.size(); //each stairs has a unique x location
		c.Set_Location(x, 3);
	}

	return c;
}

bool Levelnode::Remake_Level()
{
	flags &= (0xffffffff ^ LEVEL_VISITED); //set level not visited again
	delete level;
	return Visit();
}

//Returns true if new level was made.
bool Levelnode::Visit()
{
	bool rv;

	if (Is_Visited()==false)
	{
		flags |= LEVEL_VISITED;
		Theme dtype(site.theme);

		//determine random size and basetile
		Plane p(dtype.Get_Random_Level_Size());
		const int basetile=dtype.Get_Basetile();

		const int dang=dungeonlist[site.dungeon].danger;

		//if not visited yet, create the level
		Feature_Level *flevel=new Feature_Level(
			p.width, p.height, basetile, dang);

		//downgrade the class for the node list, because creation routines
		//are not needed during the gameplay
		level=flevel;

		//set gameview stats before creating level, otherwise we init order fiasco
		gameview.Enter_New_Level(level);

		flevel->Create(site.theme);
		Create_Portals(flevel);

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
