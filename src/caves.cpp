/**************************************************************************
 * caves.cpp --                                                           *
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

//Refactored 4.9.2021 - 21.9.2025 Paul K. Pekkarinen

#include "being.h"
#include "caves.h"
#include "codex.h"
#include "dice.h"
#include "display.h"
#include "factory.h"
#include "gametime.h"
#include "gameview.h"
#include "message.h"
#include "output.h"
#include "storage.h"
#include "terrain.h"
#include "traps.h"
#include "viewtile.h"

using std::vector;

level_type *c_level; //current level

level_type::level_type(int width, int height, int bt, int danger)
	: sizex(width), sizey(height), basetile(bt), danglev(danger), loc{0}
{
	allocate_terrain_map();
}

level_type::~level_type()
{
	clear_data();
	for (int ty=0; ty<sizey; ty++)
	{
		delete[] loc[ty];
	}
}

void level_type::allocate_terrain_map()
{
	//allocate memory for new level and clear the maze
	for (int ty=0; ty<sizey; ty++)
	{
		loc[ty] = new cave_type[sizex+1]; // allocate row

		if (!loc[ty])
			panic_exit("initlevel: OUT OF MEMORY!");

		for (int tx=0; tx<sizex; tx++)
			loc[ty][tx].initialize(basetile); // clear row basetile
	}
}

void level_type::clear_data()
{
	inv.Clear();
	crew.removeall();
	rooms.clear();
}

int level_type::Checkterraineffects(int x, int y)
{
	if (Is_Outside(x, y))
	{
		msg.newmsg("You stare at the void a moment...", C_WHITE);
		return 0;
	}

	if (loc[y][x].flags & CAVE_WATER)
	{
		msg.newmsg("You can't swim.", C_WHITE);
		return 0;
	}

	if (loc[y][x].flags & CAVE_MOUNTAIN)
	{
		msg.newmsg("Impossible, look how high the mountain is.", C_WHITE);
		return 0;
	}

	if (Is_Passable(x, y)==false)
		return -1;

	/* basic movement time */
	int timeadd=TIME_MOVEAROUND;
	real rtime=0;

	/* get the time effect from terrainslist */
	int rmod=terrains[loc[y][x].type].timemod;

	if (rmod!=100)
	{
		if (rmod<1 || rmod>500)
			rmod=100;

		rtime=(TIME_MOVEAROUND / 100) * rmod;

		timeadd=(int)rtime;

		if (RANDU(100)<50)
		{
			if (rmod>=400)
			{
				msg.newmsg("This is almost impossible walk on.", C_YELLOW);
			}
			else if (rmod>=150)
			{
				msg.newmsg("This is really slow to walk on.", C_YELLOW);
			}
			else if (rmod > 100)
			{
				msg.newmsg("This terrain slows you down.", C_YELLOW);
			}

			if (rmod<20)
				msg.newmsg("Feels like flying.", C_YELLOW);
			else if (rmod<50)
				msg.newmsg("This terrain is really easy to walk on.", C_YELLOW);
			else if (rmod<100)
				msg.newmsg("It's easy to walk on this..", C_YELLOW);
		}
	}
	else
		timeadd=TIME_MOVEAROUND;

	return timeadd;
}

bool level_type::Find_Stairs(Coord &here, int stair_type, int8u number)
{
	Coord c;

	for (c.y=0; c.y<sizey; c.y++)
	{
		for (c.x=0; c.x<sizex; c.x++)
		{
			if (loc[c.y][c.x].type==stair_type)
			{
				if (loc[c.y][c.x].doorfl==number)
				{
					here=c;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool level_type::Free_To_Walk(const Coord &c)
{
	if (Is_Passable(c)) return true;
	if (Get_Terrain(c)==TYPE_DOORCLOS) return true; //move against closed doors
	return false;
}

int level_type::get_amount_of_rooms()
{
	return (int)rooms.size();
}

roomdef& level_type::get_last_created_room()
{
	vector<roomdef>::iterator i=rooms.end()-1;
	return (*i);
}

const char *level_type::Get_Name()
{
	return "level";
}

int level_type::Get_Terrain(const Coord &c)
{
	if (Is_Outside(c)) return TYPE_DARK;
	return loc[c.y][c.x].type;
}

Trap &level_type::Get_Trap(const Coord &c)
{
	return traps.Get(c);
}

bool level_type::Has_Door(const Coord &c)
{
	Terratype tt(Get_Terrain(c));
	return tt.Is_Door();
}

bool level_type::Has_Object(const Coord &c, int8u flag)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].object & flag)
		return true;
	return false;
}

//This spot has static light of some sort.
bool level_type::Has_Static_Light(const Coord &c)
{
	if (Is_Outside(c)) return false;

	if (loc[c.y][c.x].flags & CAVE_LIGHT)
		return true;

	return false;
}

bool level_type::Has_Stairs(const Coord &c)
{
	Terratype t(Get_Terrain(c));
	return t.Is_Stairs();
}

bool level_type::Inside_Room(int roomnum, const Coord &c)
{
	if ((c.x > rooms[roomnum].x1 && c.x < rooms[roomnum].x2) &&
		(c.y > rooms[roomnum].y1 && c.y < rooms[roomnum].y2))
		return true;

	return false;
}

//Routine for FOV to check if light can pass this tile.
bool level_type::Is_Blocking_Los(const Coord &c)
{
	if (Is_Outside(c)) return true;
	if (loc[c.y][c.x].doorfl & DOOR_CLOSED) return true;
	if (loc[c.y][c.x].flags & CAVE_PASSABLE) return false;

	return true;
}

bool level_type::Is_Carveable(const Coord &c)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].sval==GENERATE_DONOTCARVE) return false;
	return true;
}

bool level_type::Is_Closed_Door(const Coord &c)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].doorfl & DOOR_CLOSED) return true;
	return false;
}

bool level_type::Is_Door(const Coord &c)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].flags & CAVE_DOOR)
		return true;

	return false;
}

bool level_type::Is_Open_Door(const Coord &c)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].doorfl & DOOR_OPEN) return true;
	return false;
}

bool level_type::Is_Outside(int x, int y)
{
	if (x<0 || y<0 || y>=sizey || x>=sizex) return true;
	return false;
}

bool level_type::Is_Outside(const Coord &c)
{
	if (c.x<0 || c.y<0 || c.y>=sizey || c.x>=sizex) return true;
	return false;
}

bool level_type::Is_Outside_Border(int x, int y)
{
	if (x<1 || y<1 || y>=sizey-1 || x>=sizex-1) return true;
	return false;
}

bool level_type::Is_Explored(int y, int x)
{
	return loc[y][x].flags & CAVE_EXPLORED;
}

bool level_type::Is_Passable(const Coord &c)
{
	if (Is_Outside(c)) return false;
	if (loc[c.y][c.x].flags & CAVE_PASSABLE)
		return true;
	return false;
}

bool level_type::Is_Passable(int x, int y)
{
	Coord c(x, y);
	return Is_Passable(c);
}

bool level_type::Is_Secretdoor(const Coord &c)
{
	if (Get_Terrain(c)==TYPE_DOORSECR)
		return true;

	return false;
}

bool level_type::Is_Wall(const Coord &c)
{
	if (Get_Terrain(c)==TYPE_WALLIP) return true;
	return false;
}

bool level_type::Is_Wall(int x, int y)
{
	Coord c(x, y);
	return Is_Wall(c);
}

bool level_type::max_rooms_created()
{
	if (get_amount_of_rooms()>=ROOM_MAXNUM) return true;
	return false;
}

bool level_type::Close_Door(const Coord &c)
{
	const bool rv=Is_Open_Door(c);

	if (rv)
	{
		Set_Terrain(c, TYPE_DOORCLOS);
		loc[c.y][c.x].doorfl=DOOR_CLOSED;
	}

	return rv;
}

void level_type::Refresh_Gameview()
{
	Coord c;

	//set room ids
	for (int rc=0; rc<get_amount_of_rooms(); rc++)
	{
		for (c.y=rooms[rc].y1; c.y<=rooms[rc].y2; c.y++)
		{
			for (c.x=rooms[rc].x1; c.x<=rooms[rc].x2; c.x++)
			{
				gameview.Set_Room_Id(c, rc);
			}
		}
	}

	//place traps to gameview
	traps.Refresh();

	inv.Refresh_Item_Map();
}

bool level_type::Open_Door(const Coord &c)
{
	const bool rv=Is_Closed_Door(c);

	if (rv)
	{
		Set_Terrain(c, TYPE_DOOROPEN);
		loc[c.y][c.x].doorfl=DOOR_OPEN;
	}

	return rv;
}

void level_type::Plot_Maze(int x, int y)
{
	Coord c(x, y);
	if (Is_Outside(c)) return;
	Set_Terrain(c, TYPE_PATH);
}

void level_type::Remove_Dead_Monsters()
{
	crew.Remove_Dead(this);
}

void level_type::Remove_Trap(const Coord &c)
{
	if (traps.Remove(c))
		Clear_Object(c, OBJECT_TRAP);
}

void level_type::remove_room_owner(being *b)
{
	rooms[b->roomnum].owner=0;
	b->roomnum=-1;
}

void level_type::Reveal_Trap(const Coord &c)
{
	if (Has_Object(c, OBJECT_TRAP))
	{
		Trap &t=Get_Trap(c);

		if(Is_Door(c)==false)
			Set_Terrain(c, TYPE_FIRSTTRAP+t.Get_Type()-1);
	}
}

void level_type::set_room_owner(int roomid, being *b)
{
	rooms[roomid].owner=b;
	b->roomnum=roomid;
}

void level_type::Clear_Object(const Coord &c, int8u flag)
{
	loc[c.y][c.x].object &= (0xff ^ flag);
}

void level_type::Clear_Terrain(int terraintype)
{
	for (int ty=0; ty<sizey; ty++)
	{
		for (int tx=0; tx<sizex; tx++)
		{
			Set_Terrain(tx, ty, terraintype);
		}
	}
}

void level_type::reveal()
{
	Coord c;
	for (c.y=0; c.y<sizey; c.y++)
	{
		for (c.x=0; c.x<sizex; c.x++)
		{
			Set_Seen(c);
		}
	}
}

void level_type::Set_Flag(const Coord &c, int16u flag)
{
	loc[c.y][c.x].flags |= flag;
}

void level_type::Clear_Flag(const Coord &c, int16u flag)
{
	loc[c.y][c.x].flags&=(0xffff ^ flag);	
}

void level_type::Set_Object(const Coord &c, int8u flag)
{
	loc[c.y][c.x].object |= flag;
}

void level_type::Set_Seen(const Coord &c)
{
	Set_Flag(c, CAVE_EXPLORED);
}

void level_type::Set_Terrain(int x, int y, int terratype)
{
	Coord c(x, y);
	Set_Terrain(c, terratype);
}

void level_type::Set_Terrain(const Coord &c, int terratype)
{
	if (Is_Outside(c)) return;

	loc[c.y][c.x].type=terratype;
	loc[c.y][c.x].flags=terrains[terratype].flags;
}

/* initializes all shops in level! */
/* shoptype must be set before, when the room is created */
void level_type::Shop_Init()
{
	/* scan the roomlist for shops */
	for (int i=0; i<get_amount_of_rooms(); i++)
	{
		if (rooms[i].type==ROOM_SHOP)
		{
			factory.Add_Shopkeeper(this, i);

			/* equip shops with items */
			rooms[i].make_shop(this);
		}
	}
}

//Shows visible tile, assumes (y, x) is inside the level.
void level_type::Display_Tile(int y, int x)
{
	char ch='?';
	int color=CH_RED;
	
	//has trap been identified here
	if (loc[y][x].flags & CAVE_TRAPIDENT)
	{
		Coord c(x, y);

		if (Is_Door(c)) //show door in red color if it's trapped
		{
			ch=terrains[loc[y][x].type].out;
			color=CH_RED;
		}
		else
		{
			const int t=gameview.Get_Trap_Type(c);
			ch=list_traps[t].out;
			color=list_traps[t].color;
		}
	}
	else
	{
		ch=terrains[loc[y][x].type].out;
		color=terrains[loc[y][x].type].color;
	}

	put_char(ch, color);	
}

void level_type::Display_Data()
{
	my_printf("Size: %d x %d, danger level: %d, basetile: %s.\n",
		sizex, sizey, danglev, terrains[basetile].desc);

	const int r=(int)rooms.size();
	my_printf("%d rooms in this level.\n", r);

	const int i=inv.Get_Linear_Size();

	my_printf("%d items, not counting items inside containers.\n", i);

	crew.Debug_List();
}

void level_type::List_Rooms()
{
	clear_screen();
	
	const int a=get_amount_of_rooms();
	if (a==0)
	{
		my_printf("No rooms in this level.");
		return;
	}

	for (int t=0; t<a; t++)
	{
		rooms[t].Display_Data(t);
	}
}

void level_type::List_Terrain()
{
	const int max_amt=TYPE_TRAPPIT+1; //use last tile as max amount

	int tiles[max_amt]={0};
	vector<Coord> stairs;

	Coord c;
	for (c.y=0; c.y<sizey; c.y++)
	{
		for (c.x=0; c.x<sizex; c.x++)
		{
			Terratype i(Get_Terrain(c));
			if (i.Is_Portal())
				stairs.push_back(c);
			tiles[i.type]++;
		}
	}

	clear_screen();
	int si=0; //stairs index

	for (int t=0; t<max_amt; t++)
	{
		const int amt=tiles[t];
		if (amt==0) continue; //don't show if no tiles of this type found
		
		display->Draw_Terrain(t, Viewtile::Visible);
		standend();
		Terratype tt(t);
		if (tt.Is_Portal())
		{
			//show stored location of stairs
			my_printf(" %d %s at %d, %d\n",
				amt, terrains[t].desc, stairs[si].x, stairs[si].y);
			si++;
		}
		else
			my_printf(" %d %s\n", amt, terrains[t].desc);
	}
}

void level_type::Show_Tile_Description(const Coord &c)
{
	set_color(C_WHITE);
	gotoxy(0, 2);
	my_printf("%-15s", terrains[loc[c.y][c.x].type].desc);
}

void level_type::save(Tar_Ball &tb)
{
	tb.Put(sizex);
	tb.Put(sizey);
	tb.Put(danglev);

	//save monsters before rooms for room owners
	crew.save(tb);

	//save room data
	const int r=(int)rooms.size();
	tb.Put(r); //store number of rooms, if any
	for (int t=0; t<r; t++)
	{
		rooms[t].Save(tb, this);
	}

	//save level items
	inv.save(tb);

	traps.Save(tb);

	//save terrain
	for (int ty=0; ty<sizey; ty++)
	{
		for (int tx=0; tx<sizex; tx++)
		{
			loc[ty][tx].save(tb);
		}
	}
}

level_type::level_type(Tar_Ball &tb)
{
	sizex=tb.Get_Next_Value();
	sizey=tb.Get_Next_Value();
	danglev=tb.Get_Next_Value();

	allocate_terrain_map();

	//load monsters before rooms to get room owner handles
	crew.load(tb, this);

	//load room data
	const int r=tb.Get_Next_Value();
	for (int t=0; t<r; t++)
	{
		roomdef huone;
		huone.Load(tb, this);
		rooms.push_back(huone);
	}

	//load level items
	inv.load(tb);

	traps.Load(tb);

	//load terrain
	for (int ty=0; ty<sizey; ty++)
	{
		for (int tx=0; tx<sizex; tx++)
		{
			loc[ty][tx].load(tb);
		}
	}
}
