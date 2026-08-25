/**************************************************************************
 * world.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 27.8.2021 - 28.9.2024 Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "message.h"
#include "move.h"
#include "node.h"
#include "output.h"
#include "outworld.h"
#include "salamath.h"
#include "spot.h"
#include "storage.h"
#include "terrain.h"
#include "world.h"
#include "uncover.h"

using std::string;
using std::vector;

World::World()
	: index(0)
{
	//initialize a random world time
	const int cyear=1200+RANDU(100);
	const int cmonth=RANDU(TIME_MONTHS);
	const int cday=RANDU(TIME_DAYS);
	const int chour=6+RANDU(13);
	const int cmin=RANDU(TIME_MINUTES);

	worldtime.set(cyear, cmonth, cday, chour, cmin);
	worldtime.set_events(false);

	for (int r=0; r<dng::Max_Dungeons; r++)
		visited_dungs[r]=false;

	//construct the game world, this is the enum list of
	//dungeon types (main places in the game world) which
	//each contain one or a set of levels.
	for (int i=0; i<dng::Max_Dungeons; i++)
	{
		const int a=dungeonlist[i].Get_Amount_Of_Levels();

		for (int d=0; d<a; d++)
		{
			//determine the location in the list of levels
			int wh;
			if (d==0) wh=dng::Top;
			else if (d==a-1) wh=dng::Bottom;
			else wh=dng::Middle;

			const int theme=Theme::Get_Random_Level_Type(i, d, a);

			Level site(theme, i, d, wh);
			levels.push_back(new Levelnode(site));
		}
	}

	//after level nodes are created it's time to connect them by
	//adding portals in the level node's list. These are the
	//blueprint for actual terrain stairs or border exits created
	//when the level is entered first time.
	int portal_id=0;
	for (int i=0; i<dng::Max_Dungeons; i++)
	{
		//main connections defined in dungeon data
		int p=0;
		while (dungeonlist[i].portals[p]!=-1)
		{
			const int dest_dung=dungeonlist[i].portals[p+2];
			Levelnode *src=Find_Node_By_Location(
				dungeonlist[i].portals[p+1],
				dest_dung);

			const int pterrain=dungeonlist[i].portals[p];

			//create source portal that we enter
			Portal port(portal_id, portal_id+1, pterrain);
			src->Add_Portal(port);

			Levelnode *dest=Find_Node_By_Location(dng::Top, dest_dung);

			const int dest_terrain=dest->Get_Reverse_Portal(pterrain);

			//each portal has a destination pair with reversed id numbers
			Portal dest_port(portal_id+1, portal_id, dest_terrain);
			dest->Add_Portal(dest_port);

			portal_id+=2;
			p+=3;
		}

		//connections between levels of the same dungeon, these have always
		//stairs down paired with stairs up in the destination level
		const int a=Number_Of_Levels(i);
		if (a>1)
		{
			int levi=Find_First_Level_Index(i);
			for (int t=0; t<a-1; t++)
			{
				Portal port(portal_id, portal_id+1, TYPE_STAIRDOWN);
				levels[levi]->Add_Portal(port);

				Portal dest_port(portal_id+1, portal_id, TYPE_STAIRUP);
				levels[levi+1]->Add_Portal(dest_port);

				levi++;
				portal_id+=2;
			}
		}
	}
}

World::~World()
{
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		delete (*ii);
	}
}

int World::Find_First_Level_Index(int dung)
{
	int a=0;

	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->site.dungeon==dung)
			return a;
		a++;
	}

	//if anything goes wrong points to outworld
	return 0;
}

//Returns level index of this portal id, in other words where it is.
int World::Find_Level_By_Portal(int id)
{
	int a=0;

	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->Has_Portal(id))
			return a;
		a++;
	}

	//if anything goes wrong points to outworld
	return 0;
}

Levelnode *World::Find_Node_By_Location(int loc, int dung)
{
	int a=0;
	vector<int> ids;

	//count number of levels in this dungeon
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->site.dungeon==dung)
			ids.push_back(a);
	}

	int ni=0; //default: top index
	if (loc==dng::Bottom) ni=a-1;
	else if (loc==dng::Middle)
	{
		if (a<=2) ni=0;
		else
			ni=random_number(1, a-2);
	}

	const int lindex=ids[ni];
	return levels[lindex];
}

string World::Get_Level_Name()
{
	return levels[index]->site.Get_Name();
}

//Count number of levels in a dungeon.
int World::Number_Of_Levels(int dung)
{
	int a=0;
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->site.dungeon==dung) a++;
	}
	return a;
}

//When entering the game world at the start of game.
void World::Arrival()
{
	index=Find_First_Level_Index(dng::Santhel);
	player.sight=15;
	player.delta=6;

	Levelnode *n=Get_Current_Node();

	n->Visit(); //creates new level if needed

	//also checks dungeon visits
	const int dung=n->site.dungeon;
	visited_dungs[dung]=true;

	//use random location for now
	level_type *level=n->Get_Level();
	teleport_player(level, false, true);
}

void World::Enter_Portal(int8u number)
{
	//stairs numbers start from 1... (0 is no portal)
	const int portal_id=(int)number-1;

	Portal &p=levels[index]->Get_Portal(portal_id);
	const int src_dung=Get_Dungeon();

	const int dest=Find_Level_By_Portal(p.dest_id);
	const int dest_dung=levels[dest]->site.dungeon;

	if (src_dung!=dest_dung)
	{
		const char *txt;
		if (dest_dung==dng::Mountains)
			txt="outside";
		else
			txt=dungeonlist[dest_dung].name;

		msg.vnewmsg(C_GREEN, "Entering %s!", txt);
		msg.newmsg(dungeonlist[dest_dung].desc, C_WHITE);
	}
	else
	{
		if (p.terrain_type==TYPE_STAIRDOWN)
			msg.newmsg("You entered down stairs...", C_YELLOW);
		else if (p.terrain_type==TYPE_STAIRUP)
			msg.newmsg("You entered up stairs...", C_YELLOW);
		else
			msg.newmsg(C_YELLOW, "You leave %s.", dungeonlist[src_dung].name);
	}

	index=dest; //change to new level
	Levelnode *n=Get_Current_Node();

	n->Visit(); //creates new level if needed

	//also checks dungeon visits
	visited_dungs[dest_dung]=true;

	player.delta=6;
	level_type *level=n->Get_Level();
	const int th=n->Get_Theme();
	bool to_stairs=true;

	if (th==Theme::Town)
	{
		player.sight=15;
		//use random location for now
		teleport_player(level, false, true);
		to_stairs=false;
	}
	else if (th==Theme::Outworld)
	{
		player.sight=4;
		player.delta=2;

		if (player.huntmode)
		{
			player.huntmode=false;
			//player.Reset_Location(player.wild.x, player.wild.y); //note: fix later
		}
	}
	else
	{
		player.sight=10;
	}

	if (to_stairs)
	{
		int8u sn=n->Get_Stairs_Number(p.dest_id);
		Coord pos;

		//in case stairs are not found, which is a bug, shows this message
		if (sn==0 || level->Find_Stairs(pos, sn)==false)
		{
			msg.newmsg("Huh, what, where am I?!");
			pos=find_random_location(level, 1);
		}

		player.Jump_To(pos);
	}

	/* enable or disable weather notifications */
	Display_Time_Events(Has_Weather());
}

bool World::Leave_Town()
{
	string s("Do you want to leave ");
	s.append(Get_Level_Name());

	if (confirm_yn(s.c_str(), false, true))
	{
		player.lastdir=0;
		Enter_Portal(1);
		return true;
	}

	return false;
}

void World::Advance_Time(int ticks)
{
	worldtime.tick(ticks);
	weather.passtime(ticks, worldtime);
}

void World::Display_Return_Message(const char *plrname)
{
	string s=Get_Level_Name();
	const int dung=Get_Dungeon();

	my_printf("%s, Welcome back.\nYou left while you were in %s (%s).\n",
		plrname,
		dungeonlist[dung].name,
		s.c_str());
}

void World::Display_Time_Events(bool v)
{
	worldtime.set_events(v);
}

level_type *World::Get_Current_Level()
{
	return levels[index]->Get_Level();
}

Levelnode *World::Get_Current_Node()
{
	return levels[index];
}

int World::Get_Dungeon()
{
	return levels[index]->site.dungeon;
}

int World::Get_Level_Type()
{
	return levels[index]->Get_Theme();
}

bool World::Has_Weather()
{
	if (Get_Dungeon()==dng::Mountains || Get_Level_Type()==Theme::Town)
		return true;

	return false;
}

bool World::Is_Matching_Place(int dung, int depth)
{
	Levelnode *n=Get_Current_Node();

	if (Get_Dungeon()==dung && n->site.depth==depth)
		return true;

	return false;
}

bool World::Is_Night()
{
	const int lvltype=Get_Level_Type();
	const int dung=Get_Dungeon();

	/* night is always present in the dungeons */
	if (dung!=dng::Mountains && lvltype!=Theme::Town)
		return true;

	return worldtime.is_night();
}

bool World::Is_Outside() const
{
	if (levels[index]->site.dungeon==dng::Mountains) return true;
	return false;
}

//Return number of levels visited.
int World::Num_Levels()
{
	int a=0;
	for (levitr ii = levels.begin() ; ii != levels.end() ; ++ii)
	{
		if ((*ii)->Is_Visited())
			a++;
	}
	return a;
}

//Return number of places (dungeons) visited.
int World::Num_Places()
{
	int n=0;
	for (int t=0; t<dng::Max_Dungeons; t++)
	{
		if (visited_dungs[t]) n++;
	}
	return n;
}

void World::Remake_Current_Level()
{
	levels[index]->Remake_Level();
}

void World::Show_Birth_Time()
{
	my_printf("You were born at ");
	worldtime.print_worldtime();
}

void World::Show_Time()
{
	worldtime.show_worldtime();
}

void World::Weather_Report()
{
	weather.report();
}

void World::Display_Overview()
{
	const int nlevels=(int)levels.size();
	string s=Get_Level_Name();

	my_printf("Current level: %s (%d)\n", s.c_str(), index);

	my_printf("Dungeons in the world: %d, dungeons player has visited: %d\n",
		dng::Max_Dungeons, Num_Places());
	my_printf("Levels in the world: %d, levels player has visited: %d\n",
		nlevels, Num_Levels());

	if (Is_Night())
		print_text("It's night or dark.\n");
	else
		print_text("It's daytime.\n");

	worldtime.print_worldtime();
}

void World::Display_Level_Data()
{
	//show current level's basic information
	levels[index]->Display_Data(index);
}

void World::Display_Location()
{
	string s;

	if (player.huntmode)
		s="Wilderness";
	else
	{
		const int dung=Get_Dungeon();

		s.append(dungeonlist[dung].name);
		s.append(" Lvl:");
		s.append(Get_Level_Name());
	}

	const int sz=(int)s.size();

	print_text_to(SCREEN_COLS-sz, STATUSROW, s.c_str());
}

void World::Save(Tar_Ball &tb)
{
	tb.Put(index);
	tb.Put((int)levels.size());

	for (levitr ii=levels.begin(); ii!=levels.end(); ++ii)
		(*ii)->Save(tb);

	weather.save(tb);
	worldtime.save(tb);

	for (int t=0; t<dng::Max_Dungeons; t++)
		tb.Put_Bool(visited_dungs[t]);
}

void World::Load(Tar_Ball &tb)
{
	index=tb.Get_Next_Value();
	const int a=tb.Get_Next_Value();

	for (int i=0; i<a; i++)
	{
		Levelnode *n=new Levelnode;
		n->Load(tb);
		levels.push_back(n);
	}

	weather.load(tb);
	worldtime.load(tb);

	const int v=tb.Get_Next_Value();
	for (int t=0; t<v; t++)
	{
		visited_dungs[t]=tb.Get_Next_Bool();
	}
}
