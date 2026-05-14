//rooms.cpp - routines for rooms

//Refactored 25.9.2021 - 2.4.2026 Paul K. Pekkarinen

#include "being.h"
#include "caves.h"
#include "dice.h"
#include "gameview.h"
#include "message.h"
#include "names.h"
#include "output.h"
#include "rooms.h"
#include "spawner.h"
#include "storage.h"

using std::string;

const char *roomnames[]=
{
	"a dungeon room",
	"a lair",
	"a shop",
	"a vault",
	"a castle",
	"a farm",
	"a house",
	"a small cave"
};

roomdef::roomdef()
	: type(0), flags(0), doorx(0), doory(0), owner(0)
{

}

roomdef::roomdef(int rt, int x, int y, int a, int b)
	: Area(x, y, a, b), type(rt), flags(0), doorx(0), doory(0), owner(0)
{

}

const char *roomdef::Get_Name()
{
	return roomnames[type];
}

bool roomdef::Is_Visited()
{
	return ((flags & ROOM_IS_VISITED) == 1);
}

void roomdef::Check_Visit()
{
	if (Is_Visited()==false)
	{
		flags|=ROOM_IS_VISITED;
		msg.vnewmsg(C_WHITE, "This looks like %s.", Get_Name());
	}
}

void roomdef::set_door(int a, int b)
{
	doorx=a;
	doory=b;
}

void roomdef::make_shop(level_type *level)
{
	//determine randomly which shop type
	int shoptype=RANDU(100);

	if (shoptype<20)
		shoptype=Shoppe::Armour;
	else if (shoptype<40)
		shoptype=Shoppe::Weapon;
	else if (shoptype<70)
		shoptype=Shoppe::Food;
	else
		shoptype=Shoppe::General;

	//change this room to a shop
	kauppa.Shopify(shoptype);

	flags=0;
	Spawner spw(level);

	for (int j=nw.y; j<se.y; j++)
	{
		for (int i=nw.x; i<se.x; i++)
		{
			Coord c(i, j);

			if (level->Is_Passable(c))
				kauppa.Create_Item(spw, c);
		}
	}
}

//Copy this room's id to gameview.
void roomdef::Project_Room_Id(int id)
{
	Coord c;
	
	for (c.y=nw.y; c.y<=se.y; c.y++)
	{
		for (c.x=nw.x; c.x<=se.x; c.x++)
		{
			gameview.Set_Room_Id(c, id);
		}
	}	
}

void roomdef::Display_Data(int rindex)
{
	string s;

	if (kauppa.Is_Open())
	{
		s.append(kauppa.Get_Name());
		s.append(", owner: ");
		if (owner==0)
			s.append("missing");
		else
			s.append(owner->Get_Name());
	}
	else
		s.append(" room");

	string vis;
	if (Is_Visited())
		vis.append("(visited)");
	else
		vis.append("(unknown)");

	my_printf("%d: '%s' (%d, %d, %d, %d) %s %s\n",
		rindex, Get_Name(), nw.x, nw.y, se.x, se.y, s.c_str(), vis.c_str());
}

void roomdef::Save(Tar_Ball &tb, level_type *lvl)
{
	Area::Save(tb);
	
	tb.Put(type);
	tb.Put_Char(flags);

	kauppa.Save(tb);

	tb.Put(doorx);
	tb.Put(doory);

	//save index of room owner if exists, to restore in loading
	int i;
	if (owner==0)
		i=-1;
	else
		i=lvl->crew.Get_Index(owner);

	tb.Put(i);
}

void roomdef::Load(Tar_Ball &tb, level_type *lvl)
{
	Area::Load(tb);
	
	type=tb.Get_Next_Value();
	flags=tb.Get_Next_Char();

	kauppa.Load(tb);

	doorx=tb.Get_Next_Value();
	doory=tb.Get_Next_Value();

	//restore room owner
	const int i=tb.Get_Next_Value();
	if (i==-1)
		owner=0;
	else
		owner=lvl->crew.Find_By_Index(i);
}
