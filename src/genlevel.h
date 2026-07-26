//genlevel.h

//Unit genlevel: Level generation.

#ifndef GENLEVEL_H
#define GENLEVEL_H

#include "caves.h"

//Genlevel inherits from basic level and contains level creation routines.
//If any creation routines are required later during gameplay they can be moved
//to the base class.
class Genlevel : public level_type
{
private:
	void Add_Room(roomdef &r);
	void Create_Stairs_Down(int8u number, bool first_town_level);
	void Create_Stairs_Up(int8u number);

protected:
	void Create_Stairs();

public:
	Genlevel(int width, int height, int bt, int danger);

	bool Create_Door(int x, int y, bool special);
	void Create_Floor(const Coord &c, int floor, int16u flags);
	void Create_Wall(const Coord &c, int wall, bool nocarve, bool dark);
	void Create_Wall(const Coord &c, int wall);
	void Protect_From_Carving(const Coord &c);
};

#endif
