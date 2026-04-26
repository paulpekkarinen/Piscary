/**************************************************************************
 * gameview.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 8.12.2022 - 12.4.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "bytegrid.h"
#include "caves.h"
#include "gameview.h"
#include "invnode.h"
#include "log.h"
#include "output.h"
#include "salamath.h"
#include "viewtile.h"
#include "way.h"

Gameview gameview;

Gameview::Gameview()
	: level(0), data(0), view(MAPWIN_X, MAPWIN_Y, MAPWIN_SIZEX, MAPWIN_SIZEY)
{
	//use dummy size, resized later to match level size
	dirgrid=new Bytegrid(10, 10);
}

Gameview::~Gameview()
{
	delete[] data;
	delete dirgrid;
}

Viewtile *Gameview::Get(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1)
		return 0;

	return &data[i];
}

int Gameview::Get_Index(const Coord &c) const
{
	if (level->Is_Outside(c))
		return -1;

	return (c.y*level->sizex)+c.x;
}

Coord Gameview::Get_Screen_Location(const Coord &c)
{
	return Coord((c.x-camera.x)+view.x, (c.y-camera.y)+view.y);
}

int Gameview::Count_Items(const Coord &c)
{
	if (Get_Item(c)==0)
		return 0;

	return level->inv.Count_Items_Flat(c.x, c.y);
}

Actor *Gameview::Get_Actor(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return 0;

	Actor *a=data[i].tonttu;
	if (a==0) //if no monster here...
	{
		//check if the player happens to be here
		if (player.Get_Location()==c)
			a=&player;
	}

	return a;
}

int Gameview::Get_Room_Id(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return -1;
	return data[i].room_id;
}

invnode *Gameview::Get_Item(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return 0;
	return data[i].esine;
}

being *Gameview::Get_Monster(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return 0;
	return data[i].tonttu;
}

int Gameview::Get_Trap_Type(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return 0;
	return data[i].trap_type;
}

bool Gameview::Is_Visible(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return false;

	if (data[i].vision==Viewtile::Visible)
		return true;	

	return false;
}

bool Gameview::Is_Visible(int x, int y)
{
	Coord c(x, y);
	return Is_Visible(c);
}

//Can location mx, my see location px, py?
bool Gameview::Cansee(const Coord &mc, const Coord &pc, int dist)
{
	//get distance to the destination
	const int auxd=get_distance(mc, pc);

	//if distance low enough
	if (auxd < dist)
	{
		if (Is_Visible(pc) && Is_Visible(mc))
			return true;
	}
	
	return false;
}

void Gameview::Clear_Item(const Coord &c)
{
	const int i=Get_Index(c);
	if (i==-1) return;
	data[i].esine=0;
}

void Gameview::Enter_New_Level(level_type *kentta)
{
	level=kentta;
	
	//rebuild for the size of the current level
	delete[] data;

	const int w=level->sizex;
	const int h=level->sizey;

	data=new Viewtile[w*h];
	dirgrid->Resize(w, h);

	//copy objects, room ids etc. to gameview
	level->Refresh_Gameview();
	fov.Set_Level(level);
}

void Gameview::Monster_Torchlos(being *b)
{
	//note: write code for monster fov
}

//Creature notices items and other creatures.
bool Gameview::Notice_Something(being *b, bool items_too)
{
	Coord mon=b->Get_Location();
	Coord c=b->Get_Location();
	Coord d(-1, -1);
	Coord plr=player.Get_Location();
	bool saw_item=false;
	bool saw_creature=false;
	const int distance=10;
	being *otus=0;

	//check player's location always, without direction checking
	const bool saw_player=Cansee(mon, plr, distance);

	//note: use random direction to check for now
	const int dir=Way::Get_Random_Main_Direction();

	//check other stuff at direction for 10 tiles, 'b' will
	//remember first creature and item it saw
	for (int i=0; i<10; i++)
	{
		c.Move_Direction(dir);

		//find only first item, 'items_too' is for creatures that
		//are interested about items
		if (items_too && saw_item==false)
		{
			invnode *item=Get_Item(c);
			if (item!=0)
				saw_item=Cansee(mon, c, distance);

			if (saw_item)
				d=c; //store item location for targeting
		}

		otus=Get_Monster(c);
		if (otus!=0)
			saw_creature=Cansee(mon, c, distance);

		//break only if creature noticed, items are secondary target
		if (saw_creature)
			break;
	}

	//determine which object is the most interesting to this creature,
	//at this point however it's always player first etc.
	
	if (saw_player)
	{
		if (b->Gets_Angry_To_Player())
		{
			b->Getangry(level, &player);
			return true;
		}
	}

	if (saw_creature)
	{
		if (b->Gets_Angry_To(otus))
		{
			b->Getangry(level, otus);
			return true;
		}
	}

	if (saw_item)
	{
		b->Set_Target_Spot(d.x, d.y);
		b->m.status|=MST_PURSUEITEM;
		return true;
	}		

	return false;
}

void Gameview::Put_Item(invnode *i)
{
	const Coord c=i->Get_Location();
	const int index=Get_Index(c);
	if (index!=-1)
		data[index].esine=i;
}

void Gameview::Put_Monster(being *b, const Coord &c)
{
	const int i=Get_Index(c);
	if (i!=-1)
		data[i].tonttu=b;
}

void Gameview::Put_Trap(const Coord &c, int tt)
{
	const int i=Get_Index(c);
	if (i!=-1)
		data[i].trap_type=tt;
}

void Gameview::Set_Room_Id(const Coord &c, int rid)
{
	const int i=Get_Index(c);
	if (i!=-1)
		data[i].room_id=rid;
}

void Gameview::Clear_Fov(const Coord &c)
{
	//use direct index for speed without checking
	data[c.y*level->sizex+c.x].vision=Viewtile::Dark;
}

void Gameview::Set_Fov(const Coord &c)
{
	//use direct index for speed without checking
	data[c.y*level->sizex+c.x].vision=Viewtile::Visible;
	level->Set_Seen(c);
}

void Gameview::Clamp_Camera_X()
{
	if (camera.x<0) camera.x=0;
	else
	{
		const int w=level->sizex;
		const int gw=view.width;
		
		if (camera.x+gw>w) camera.x=w-gw;
	}
}

void Gameview::Clamp_Camera_Y()
{
	if (camera.y<0) camera.y=0;
	else
	{
		const int h=level->sizey;
		const int gh=view.height;
		if (camera.y+gh>h) camera.y=h-gh;
	}
}

void Gameview::Clamp_Camera()
{
	Clamp_Camera_X();
	Clamp_Camera_Y();
}

void Gameview::Center(const Coord &c)
{
	camera.Set(c.x-(view.width/2), c.y-(view.height/2));
	Clamp_Camera();
}

void Gameview::Show()
{
	Coord c=player.Get_Location();

	//check horizontal size
	const int a=view.width-level->sizex;
	if (a>=0) //smaller than view: center
	{
		camera.x=view.x+(a/2);
	}
	else //bigger than view
	{
		//scroll view if near edges
		const int dx=c.x-camera.x;
		
		if (dx<Horizontal_Limit)
		{
			camera.x-=Horizontal_Limit;
			Clamp_Camera_X();
		}
		else
		{
			if (dx>=view.width-Horizontal_Limit)
			{
				camera.x+=Horizontal_Limit;
				Clamp_Camera_X();
			}
		}
	}

	//check vertical size
	const int b=view.height-level->sizey;
	if (b>=0)
	{
		camera.y=view.y+(b/2);
	}
	else
	{
		//scroll view if near edges
		const int dy=c.y-camera.y;

		if (dy<Vertical_Limit)
		{
			camera.y-=Vertical_Limit;
			Clamp_Camera_Y();
		}
		else
		{
			if (dy>=view.height-Vertical_Limit)
			{
				camera.y+=Vertical_Limit;
				Clamp_Camera_Y();
			}
		}
	}

	//note: since Gameview is now showing monsters, monsterinsight doesn't work,
	//but this could be set in FOV routine?
	//player.monsterinsight=level->crew.new_output_monsters(level);

	level->Show_Tile_Description(c);

	Coord sc=Get_Screen_Location(c);

	//debug: show location
	//gotoxy(0, 3);
	//my_printf("(%d, %d)(%d, %d)", c.x, c.y, sc.x, sc.y);	

	for (int i=0; i<level->sizex*level->sizey; i++)
		data[i].vision=Viewtile::Dark;

	fov.Calculate_Fov(c);

	int ny=view.y;
	for (c.y=camera.y; c.y<camera.y+view.height; c.y++)
	{
		gotoxy(view.x, ny);

		for (c.x=camera.x; c.x<camera.x+view.width; c.x++)
		{
			if (level->Is_Outside(c)==false)
				Show_Tile(c.x, c.y);
		}

		ny++;
	}

	gotoxy(sc.x, sc.y);
	player.Display();
	gotoxy(sc.x, sc.y);
}

//This routine assumes that the location x, y is inside the level.
void Gameview::Show_Tile(int x, int y)
{
	//if not yet explored, show gray slab
	if (level->Is_Explored(y, x)==false)
	{
		put_char('`', CH_DGRAY);
		return;		
	}

	//show either object or terrain from level
	if (data[(y*level->sizex)+x].Show()==false)
		level->Display_Tile(y, x);
}

void Gameview::Land_Item(invnode *item, const Coord &c)
{
	item->x=c.x;
	item->y=c.y;
	item->slot = -1; //clear gear slot if it's still active

	level->inv.Add_Item(item);
}

void Gameview::Refresh_Item_Map(const Coord &c)
{
	//refresh item map by finding top item
	Put_Item(level->inv.Top_Item(c));	
}
