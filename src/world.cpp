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
#include "game.h"
#include "gametime.h"
#include "gameview.h"
#include "message.h"
#include "node.h"
#include "output.h"
#include "outworld.h"
#include "randgen.h"
#include "salamath.h"
#include "storage.h"
#include "terrain.h"
#include "world.h"
#include "uncover.h"

using std::string;

Level Testhole[]=
{
	{"Top", 1, 0, DTYPE_ROOMY, 2, 12, 0, Testhole+1, 0, 0},
	{"Primitive 2", 2, 0, DTYPE_ROOMY2, 0, 0, Testhole, Testhole+2, 0, 0},
	{"Primitive 3", 3, 0, DTYPE_ROOMY2, 0, 0, Testhole+1, Testhole+3, 0, 0},
	{"Bottom", 4, 0, DTYPE_ROOMY, 19, 52, Testhole+2, 0, 0, 0},
	{0}
};

Level Abyss[]=
{
	{"Entrance", 1, 0, DTYPE_ROOMY, 19, 54, 0, Abyss+1, 0, 0},
	{"Abyss 1", 2, 0, DTYPE_ROOMY2, 0, 0, Abyss, Abyss+2, 0, 0},
	{"Abyss 2", 3, 0, DTYPE_ROOMY, 0, 0, Abyss+1, Abyss+3, 0, 0},
	{"Abyss 3", 4, 0, DTYPE_ROOMY2, 0, 0, Abyss+2, Abyss+4, 0, 0},
	{"Abyss 4", 5, 0, DTYPE_RANDOM, 0, 0, Abyss+3, Abyss+5, 0, 0},
	{"Abyss 5", 6, 0, DTYPE_RANDOM, 0, 0, Abyss+4, Abyss+6, 0, 0},
	{"Abyss 6", 7, 0, DTYPE_RANDOM, 0, 0, Abyss+5, Abyss+7, 0, 0},
	{"Abyss 7", 8, 0, DTYPE_RANDOM, 0, 0, Abyss+6, Abyss+8, 0, 0},
	{"Abyss 8", 9, 0, DTYPE_RANDOM, 0, 0, Abyss+7, 0, 0, 0},
	{0}
};

Level Bottoms[]=
{
	{"Level 1", 1, 0, DTYPE_MAZE2, 6, 2, 0, Bottoms+1, 0, 0},
	{"Level 2", 2, 0, DTYPE_MAZE2, 0, 0, Bottoms, Bottoms+2, 0, 0},
	{"Level 3", 3, 0, DTYPE_MAZE2, 0, 0, Bottoms+1, Bottoms+3, 0, 0},
	{"Level 4", 4, 0, DTYPE_RANDOM, 0, 0, Bottoms+2, Bottoms+4, 0, 0},
	{"Level 5", 5, 0, DTYPE_ROOMY2, 0, 0, Bottoms+3, Bottoms+5, 0, 0},
	{"Level 6", 6, 0, DTYPE_ROOMY2, 0, 0, Bottoms+4, Bottoms+6, 0, 0},
	{"Level 7", 7, 0, DTYPE_MAZE2, 0, 0, Bottoms+5, Bottoms+7, 0, 0},
	{"Level 8", 8, 0, DTYPE_MAZE2, 0, 0, Bottoms+6, Bottoms+8, 0, 0},
	{"Level 9", 9, 0, DTYPE_MAZE2, 0, 0, Bottoms+7, Bottoms+9, 0, 0},
	{"Level 10", 10, 0, DTYPE_MAZE2, 0, 0, Bottoms+8, 0, 0, 0},
	{0}
};

Level town_santhel[]=
{
	{"Town", 1, 0, DTYPE_TOWN, 8, 33, 0, town_santhel+1, 0, 0},
	{"Dungeons of Santhel", 2, 0, DTYPE_ROOMY, 0, 0, town_santhel, town_santhel+2, 0, 0},
	{"Deep Dungeon", 3, 0, DTYPE_ROOMY, 0, 0, town_santhel+1, 0, 0, 0},
	{0}
};

Level outworld[]=
{
	{"Mountains", 1, 0, DTYPE_OUTWORLD, 0, 0, 0, 0, 0, 0},
	{0}
};

//This is a list of dungeons in Saladir
Dungeon dungeonlist[]=
{
	/* first is the wilderness (outworld map) */
	{"Salmorrian mountains",
	"Salmorrian mountains.",
	PLACE_MOUNTAINS,
	outworld, 0, 0, 0},

	/* here are the dungeons */
	{"Very primitive dungeon",
	"This dungeon looks dull and boresome.",
	PLACE_DPRIMITIVE,
	Testhole, STAIRDOWN1, 1, TYPE_DUNGEON2},

	{"Very primitive dungeon",
	"You've entered the alternate entrance, but still it looks dull and boresome.",
	PLACE_DPRIMITIVE,
	Testhole+3, STAIRDOWN2, 1, TYPE_DUNGEON2},

	{"Caverns of Tha'nthol",
	"There are the famous cavers of Tha'nthol, the bored elf wizard.",
	PLACE_DTHANTHOL,
	Bottoms, STAIRDOWN1, 1, TYPE_DUNGEON1},

	{"Abyss",
	"Abyss, a soon to be a place of danger and hunger :-).",
	PLACE_DABYSS,
	Abyss, STAIRDOWN1, 1, TYPE_DUNGEON3},

	{"Santhel",
	"A small town with one castle and few houses. Oddly there're only few people here.",
	PLACE_TSANTHEL,
	town_santhel, STAIRDOWN1, 0, TYPE_TOWN1},

	{0, 0, 0, 0, 0, 0, 0}
};

bool World::Player_Go_Down(level_type *level)
{
	Coord pc=player.Get_Location();

	//is at stairs or in the outworld
	if (level->Get_Terrain(pc) == TYPE_STAIRDOWN || (dungeon==0 && Get_Level_Index()==1))
		player.Lastdir_To_Doorflag(level);
	else
		return false;

	int sdir=0;
	Level *lvldata=currnode->Get_Level_Data();

	//if player is in wilderness, enter a new dungeon (or town)
	if (dungeon==0)
	{
		//search the dungeon player is entering (with coords)
		int i=1; //indexes start from 1, because overworld is 0
		bool isdung=false;

		for (locsitr ii = dunglocs.begin() ; ii != dunglocs.end() ; ++ii)
		{
			if (pc==(*ii))
			{
				Enter_New_Dungeon(i);
				isdung=true;
				break;
			}
			i++;
		}

		if (!isdung)
		{
			msg.newmsg("Want do start digging a new dungeon?", C_WHITE);
			return false;
		}
	}
	else
	{
		msg.newmsg("You entered down stairs...", C_YELLOW);

		if (player.lastdir==STAIRDOWN1)
		{
			Set_Current_Node(lvldata->linkto1);
			sdir=STAIRUP1;
		}
		if (player.lastdir==STAIRDOWN2)
		{
			sdir=STAIRUP2;
			Set_Current_Node(lvldata->linkto2);
		}

		//check and set the visited flag for the level, this will also
		//create the level if not yet visited
		currnode->Visit();
	}

	player.delta=6;
	level=currnode->Get_Level();

	if (lvldata->dtype==DTYPE_TOWN)
	{
		player.sight=15;
		world->Display_Time_Events(true);

		Coord c;
		for (c.y=3; c.y<level->sizey-4; c.y++)
		{
			for (c.x=3; c.x<level->sizex-4; c.x++)
			{
				if (level->Is_Passable(c) &&
					level->Get_Terrain(c)!=TYPE_ROOMFLOOR)
				{
					player.Jump_To(c);
				}
			}
		}
	}
	else
	{
		player.sight=10;
		world->Display_Time_Events(false);
		Jump_To_Stairs(level, TYPE_STAIRUP, sdir);
	}

	return true;
}

void World::Player_Go_Outworld()
{
	//save old dungeon location, because entering first time outworld its
	//locations have to be created
	const int olddung=dungeon;

	Enter_New_Dungeon(0);

	//find outworld entry point based on current dungeon
	Coord arrival;
	int index=1; //start from 1, skip overworld..
	for (locsitr ii = dunglocs.begin() ; ii != dunglocs.end() ; ++ii)
	{
		if (index==olddung) //the dungeon we did exit from
		{
			arrival=(*ii); //store location of the dungeon
			break;
		}
		index++;
	}

	player.sight=4;
	player.delta=2;

	if (player.huntmode)
	{
		player.huntmode=false;
		//player.Reset_Location(player.wild.x, player.wild.y); //note: fix later
	}

	player.Jump_To(arrival);

	/* enable weather notifications */
	Display_Time_Events(true);

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}

bool World::Player_Go_Up(level_type *level)
{
	Level *lvldata=currnode->Get_Level_Data();

	if (player.lastdir==STAIROUT &&
		lvldata->outx && lvldata->outy)
	{
		Player_Go_Outworld();
		return true;
	}

	Coord pc=player.Get_Location();

	if (level->Get_Terrain(pc) == TYPE_STAIRUP)
		player.Lastdir_To_Doorflag(level);
	else
		return false;

	int sdir=0;

	world->Display_Time_Events(false);

	msg.newmsg("You entered up stairs...", C_YELLOW);

	if (player.lastdir==STAIRUP1)
	{
		Set_Current_Node(lvldata->linkfrom1);
		sdir=STAIRDOWN1;
	}
	if (player.lastdir==STAIRUP2)
	{
		Set_Current_Node(lvldata->linkfrom2);
		sdir=STAIRDOWN2;
	}
	
	//check and set the visited flag for the level, this will also create
	//the level if not yet visited
	currnode->Visit();
	Jump_To_Stairs(level, TYPE_STAIRDOWN, sdir);

	return true;
}

World::World()
	: currnode(0), dungeon(0)
{
	//initialize a random world time
	const int cyear=1200+RANDU(100);
	const int cmonth=RANDU(TIME_MONTHS);
	const int cday=RANDU(TIME_DAYS);
	const int chour=6+RANDU(13);
	const int cmin=RANDU(TIME_MINUTES);

	worldtime.set(cyear, cmonth, cday, chour, cmin);
	worldtime.set_events(false);

	//construct dungeon node list
	Dungeon *dptr=dungeonlist;
	int index=0; //unique index of the dungeon
	while (dptr->name)
	{
		dungeons.push_back(new Dungnode(dptr, index));
		dptr++;
		index++;
	}
}

World::~World()
{
	for (dngitr ii = dungeons.begin() ; ii != dungeons.end() ; ++ii)
	{
		delete (*ii);
	}
}

void World::Advance_Time(int ticks)
{
	worldtime.tick(ticks);
	weather.passtime(ticks, worldtime);
}

//Create entrance terrains to the places they were determined earlier.
void World::Create_Dungeon_Entrances(level_type *level)
{
	Dungeon *dptr=dungeonlist;
	dptr++; //skip overworld, it's not a location in the overworld itself
	
	for (locsitr ii = dunglocs.begin() ; ii != dunglocs.end() ; ++ii)
	{
		level->Set_Terrain(*ii, dptr->out);
		dptr++;
	}
}

//Find random places for each dungeon location in the overworld when the level is created.
void World::Determine_Dungeon_Locations(const Plane &lvl)
{
	const int amt_of_dungeons=(int)dungeons.size()-1; //-1 = don't count the overworld itself

	dunglocs.clear(); //clean up in case of re-creating the level

	for (int t=0; t<amt_of_dungeons; t++)
		New_Dungeon_Location(lvl);
}


void World::Display_Return_Message(const char *plrname)
{
	my_printf("%s, Welcome back.\nYou left while you were in %s (%s).\n",
		plrname,
		Get_Dungeon_Name(),
		Get_Level_Name());
}

void World::Connect_Dungeons(Outworld *owo, float v)
{
	for (int t=0; t<(int)dunglocs.size()-1; t++)
	{
		owo->Create_Passage(dunglocs[t], dunglocs[t+1], v);
	}
}

void World::Display_Time_Events(bool v)
{
	worldtime.set_events(v);
}

void World::Enter_New_Dungeon(int d)
{
	dungeon=d;
	Dungnode *dnode=dungeons[dungeon];
	currnode=dnode->Get_Node(0);
	
	Dungeon *dptr=dnode->Get_Dungeon();

	player.lastdir=dptr->staircase;
	dnode->Visit(dungeon);

	const char *txt;
	if (d==0)
		txt="outside";
	else
		txt=dptr->name;

	msg.vnewmsg(C_GREEN, "Entering %s!", txt);
	if (dptr->desc)
		msg.newmsg(dptr->desc, C_WHITE);

	//visit/create also the new level if not yet created
	currnode->Visit();
}

level_type *World::Get_Current_Level()
{
	return currnode->Get_Level();
}

Dungeon *World::Get_Dungeon(int dng)
{
	Dungnode *n=dungeons[dng];
	return n->Get_Dungeon();
}

const char *World::Get_Dungeon_Name()
{
	Dungeon *d=Get_Dungeon(dungeon);
	return d->name;
}

const char *World::Get_Level_Name()
{
	Level *curlev=Get_Level_Data();
	return curlev->name;
}

Level *World::Get_Level_Data()
{
	return currnode->Get_Level_Data();
}

int World::Get_Level_Index()
{
	Level *curlev=Get_Level_Data();
	return curlev->index;
}

int World::Get_Level_Type()
{
	Level *curlev=Get_Level_Data();
	return curlev->dtype;
}

bool World::Is_First_Level_Of_Town()
{
	if (Get_Level_Type()==DTYPE_TOWN && Get_Level_Index()==1)
		return true;
	return false;
}

bool World::Is_Matching_Place(int ptype, int index)
{
	Dungeon *d=Get_Dungeon(dungeon);

	if (d->placetype==ptype &&
		Get_Level_Index()==index) return true;
	return false;
}

bool World::Is_Night()
{
	const int lvltype=Get_Level_Type();

	/* night is always present in the dungeons */
	if (dungeon!=0 && lvltype!=DTYPE_TOWN)
		return true;

	return worldtime.is_night();
}

bool World::Is_Outside() const
{
	if (dungeon==0) return true;
	return false;
}

void World::Jump_To_Stairs(level_type *level, int stairs_type, int8u number)
{
	Coord dest;

	if (level->Find_Stairs(dest, stairs_type, number)==false)
	{
		msg.newmsg("Huh, what, where am I?!");
		dest=find_random_location(level, 1);
	}
	
	player.Jump_To(dest);
}

void World::New_Dungeon_Location(const Plane &lvl)
{
	bool banana=false;
	Coord c;

	for (int t=0; t<5000; t++)
	{
		//skip 5 tiles around the borders of the level
		c=get_random_location(lvl, 5);
		banana=true; //assume we found a valid location

		//check existing coordinates, if they are closer than 10 tiles, find a new location
		for (locsitr ii = dunglocs.begin() ; ii != dunglocs.end() ; ++ii)
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
		const int x=(int)dunglocs.size(); //each stairs has a unique x location
		c.Set(x, 3);
	}

	dunglocs.push_back(c);
}

void World::Set_Current_Node(Level *dest)
{
	Levelnode *n=dungeons[dungeon]->Get_Node_By_Level(dest);
	if (n!=0)
		currnode=n;
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

void World::Visit_Dungeon(int index)
{
	for (dngitr ii = dungeons.begin() ; ii != dungeons.end() ; ++ii)
	{
		(*ii)->Visit(index);
	}
}

void World::Weather_Report()
{
	weather.report();
}

void World::Display_Data()
{
	const int amt_of_dungeons=(int)dungeons.size();
	int nlevels=0;

	for (int t=0; t<amt_of_dungeons; t++)
	{
		bool cd;
		if (t==dungeon) cd=true;
		else cd=false;

		dungeons[t]->Display_Data(cd);
		nlevels+=dungeons[t]->Get_Amount_Of_Levels();
	}
	my_printf("Dungeons in the world: %d, dungeons player has visited: %d\n",
		amt_of_dungeons, player.num_places);
	my_printf("Levels in the world: %d, levels player has visited: %d\n",
		nlevels, player.num_levels);

	if (Is_Night())
		print_text("It's night or dark.\n");
	else
		print_text("It's daytime.\n");
		
	worldtime.print_worldtime();
	uncover.Gameview_Data();
		
	//show dungeon locations if at overworld
	if (dungeon==0)
	{
		Dungeon *dptr=dungeonlist;
		dptr++; //skip overworld, it's not a location in the overworld itself
	
		for (locsitr ii = dunglocs.begin(); ii != dunglocs.end(); ++ii)
		{
			my_printf("Entrance to %s at %d, %d.\n", dptr->name, (*ii).x, (*ii).y);
			dptr++;
		}
	}
}

void World::Display_Level_Data()
{
	my_printf("Player's location: %d, %d. Dungeon: %d, level index: %d\n",
		player.x, player.y, dungeon, Get_Level_Index());

	//show current level's basic information
	currnode->Display_Data(dungeon);	
}

void World::Save(Tar_Ball &tb)
{
	//note: how to save and load current node
	tb.Put(dungeon);

	for (dngitr ii = dungeons.begin() ; ii != dungeons.end() ; ++ii)
		(*ii)->Save(tb);

	const int v=dunglocs.size();
	tb.Put(v);

	for (locsitr ii = dunglocs.begin(); ii != dunglocs.end(); ++ii)
		(*ii).Save(tb);

	weather.save(tb);
	worldtime.save(tb);
}

void World::Load(Tar_Ball &tb)
{
	dungeon=tb.Get_Next_Value();

	for (dngitr ii = dungeons.begin() ; ii != dungeons.end() ; ++ii)
		(*ii)->Load(tb);

	dunglocs.clear(); //clear just in case
	const int v=tb.Get_Next_Value();
	for (int t=0; t<v; t++)
	{
		Coord c;
		c.Load(tb);
		dunglocs.push_back(c);
	}

	weather.load(tb);
	worldtime.load(tb);
}
