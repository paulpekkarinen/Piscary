//Legend of Saladir - spot.cpp

//Refactored 26.9.2021 - 15.5.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "saldebug.h"
#include "spot.h"
#include "terrain.h"

using std::deque;

Places::Places()
	: level(0)
{

}

Coord Places::Get_Random()
{
	Coord rv(-1, -1); //failed location

	const int amt=Get_Size();

	if (amt<1)
	{
		//if this routine fails, inform in debug message
		debug->Message("Out of coordinates in Places.");
		return rv;
	}

	const int r=RANDU(amt);
	rv=spots.at(r);

	//erase the place so it's not selected again
	deque<Coord>::iterator vi=spots.begin();
	spots.erase(vi+r);

	return rv;
}

int Places::Get_Size()
{
	return (int)spots.size();
}

bool Places::Is_Empty()
{
	return spots.empty();
}

bool Places::Is_Spot(const Coord &c)
{
	bool rv=false;

	switch (type)
	{
		case Stairs_Sites:
			rv=level->Is_Passable(c);
		break;
		case First_Level_Downstairs:
			if (level->Get_Terrain(c)==TYPE_ROOMFLOOR)
			{
				//don't create stairs in shops
				if (level->Is_Shop_Tile(c)==false)
					rv=true;
			}
		break;
		default: break;
	}

	return rv;
}

void Places::Add_Place(const Coord &c)
{
	//check if the last location was a duplicate
	if (spots.empty()==false)
	{
		deque<Coord>::iterator i=spots.end()-1;
		if ((*i)==c) return;
	}

	spots.push_back(c);
}

void Places::Clear()
{
	spots.clear();
}

void Places::Change_Area(level_type *cave)
{
	level=cave; //when changing level this is set
	Change_Area(0, 0, cave->sizex, cave->sizey);
}

void Places::Change_Area(int sx, int sy, int sw, int sh)
{
	rect.Reset(sx, sy, sw, sh);
	Clear();
}

void Places::Scan(int what)
{
	type=what;
	Clear();
	Coord c;
	int a=0;

	for (c.y=rect.y; c.y<rect.y+rect.height; c.y++)
	{
		for (c.x=rect.x; c.x<rect.x+rect.width; c.x++)
		{
			if (Is_Spot(c))
				Add_Place(c);

			a++;
		}
	}

	debug->Message("%d places scanned, %d locations found.", a, Get_Size());
}

//===

Coord find_downstairs_place(level_type *level, bool first_town_level)
{
	int panic_limit=0;
	Coord c;
	
	while (panic_limit<50000)
	{
		c=find_random_location(level, 1);

		if (first_town_level)
		{
			if (level->Get_Terrain(c)==TYPE_ROOMFLOOR)
				return c;
		}
		else
		{
			if (level->Is_Passable(c))
				return c;
		}

		panic_limit++;
	}

	//returns 0, 0 if place not found
	return c;
}

//Search floor place until it's passable for creation.
Coord find_random_location(level_type *level, int border)
{
	return find_random_location(level, border, false);
}

Coord find_random_location(level_type *level, int border, bool skip_player)
{
	Coord c;

	int panic_limit=0;
	while (panic_limit<50000)
	{
		c.Set_Location(border+RANDU(level->sizex-border-1),
			border+RANDU(level->sizey-border-1));
		if (level->Is_Passable(c))
		{
			if (skip_player)
			{
				if (player.Is_At(c))
				{
					panic_limit++;
					continue;
				}
			}
			return c;
		}

		panic_limit++;
	}

	return Coord(-1, -1);
}

Coord get_random_good_location(level_type *level)
{
	Coord c;

	for (int t=0; t<10000; t++)
	{
		c.Set_Location(RANDU(level->sizex), RANDU(level->sizey));
		if (level->Is_Passable(c) && level->Get_Terrain(c)!=TYPE_ROOMFLOOR)
			return c;
	}

	//failure returns 0,0
	return c;
}

Coord get_random_location(const Plane &p, int border)
{
	return Coord(border+RANDU(p.width-border-1), border+RANDU(p.height-border-1));
}

Coord get_random_location(const Area &a)
{
	const int x=random_number(a.nw.x, a.se.x);
	const int y=random_number(a.nw.y, a.se.y);

	return Coord(x, y);
}

//Find random location of passable tile type, this is the simplest one.
Coord get_random_coord(level_type *level)
{
	Coord c;
	
	while (1)
	{
		c.x=RANDU(level->sizex);
		c.y=RANDU(level->sizey);
		if (level->Is_Passable(c))
			return c;
	}

	//returns 0, 0 in case of error
	return c;
}
