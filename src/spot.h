//Legend of Saladir - spot.h

//Unit spot: Find random places for objects.

#ifndef SPOT_H
#define SPOT_H

#include <deque>
#include "geometry.h"
#include "types.h"

struct Area;

//Coordinate spots in a list.
class Places
{
public:
	static const int Panic_Limit=50000;

	enum Spot_Types
	{
		Stairs_Sites,
		First_Level_Downstairs
	};

private:
	std::deque<Coord> spots; //list of recorded coordinates
	Rectangle rect; //area where to scan for places
	level_type *level; //which level to look for places
	int type; //what type of location to search

	bool Is_Spot(const Coord &c);

	void Add_Place(const Coord &c);
	void Clear();

public:
	Places();

	Coord Get_Random();
	int Get_Size();
	bool Is_Empty();

	void Change_Area(level_type *cave);
	void Change_Area(int sx, int sy, int sw, int sh);
	void Scan(int what);
};

Coord find_random_location(level_type *level, int border);
Coord find_random_location(level_type *level, int border, bool skip_player);
Coord find_random_terrain_location(level_type *level);
Coord get_random_location(const Plane &p, int border);
Coord get_random_location(const Area &a);

#endif
