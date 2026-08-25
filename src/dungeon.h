//Legend of Saladir - dungeon.h

//Unit dungeon: A set of levels or just one level.

#ifndef DUNGEON_H
#define DUNGEON_H

//List of dungeons or places in the game world.
namespace dng
{
	enum Dungeon_Types
	{
		Mountains,
		Primitive,
		Thanthol,
		Abyss,
		Santhel,
		Max_Dungeons
	};

	//where to put portal in the set of levels
	enum Portal_Types
	{
		Top, //first level
		Middle, //anything between
		Bottom //last level
	};
};

//Static data for a dungeon (a set of levels).
struct Dungeon
{
	const char *name;	/* name of this dungeon */
	const char *level_name; //default name for a level of this dungeon
	const char *desc; //description of the place
	int min_levels; //how many levels this dungeon has
	int max_levels; //if min==max then only that exact amount is created

	//where you can go from this dungeon, this is a list with stairs terrain,
	//portal type and dungeon type to go
	int portals[32];
	int danger;			/* danger level of this dungeon */

	int Get_Amount_Of_Levels();
};

extern Dungeon dungeonlist[dng::Max_Dungeons];

#endif
