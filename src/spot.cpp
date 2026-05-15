//Legend of Saladir - spot.cpp

//Refactored 26.9.2021 - 15.5.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "spot.h"
#include "terrain.h"

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
