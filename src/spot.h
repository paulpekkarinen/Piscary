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
private:
	std::deque<Coord> spots;

	void Add_Place(const Coord &c);
	void Add_Place(int dx, int dy);
	void Clear();

public:
	Coord Get_Random();
	int Get_Size();
	bool Is_Empty();
};

Coord find_downstairs_place(level_type *level, bool first_town_level);
Coord find_random_location(level_type *level, int border);
Coord find_random_location(level_type *level, int border, bool skip_player);
Coord get_random_good_location(level_type *level);
Coord get_random_location(const Plane &p, int border);
Coord get_random_location(const Area &a);
Coord get_random_coord(level_type *level);

#endif
