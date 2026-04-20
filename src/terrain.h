//terrain.h

//Unit terrain: Functions per terrain tile.

#ifndef TERRAIN_H
#define TERRAIN_H

#include "types.h"

#define TYPE_DARK		0	// Dark square
#define TYPE_PATH		1	// corridor path
#define TYPE_SLOWPATH	2	// corridor
#define TYPE_STAIRUP	3
#define TYPE_STAIRDOWN	4
#define TYPE_DOORCLOS	5 // unlocked closed door
#define TYPE_DOORLOCK	6	// locked door
#define TYPE_DOOROPEN	7	// open door
#define TYPE_DOORSECR	8 // secret door (printed as wall)
#define TYPE_DOORTRAP	9 // trapped door
#define TYPE_WALLIP		10	// impassable wall
#define TYPE_WALLPA		11	// passable wall, digging
#define TYPE_ROOMFLOOR	12 // floor for room
#define TYPE_PASSAGE	13 // passage(dungeon)
#define TYPE_GRASS		14
#define TYPE_MOUNTAIN   22
#define TYPE_HMOUNTAIN  23
#define TYPE_VOLCANO    24
#define TYPE_HILLS      25
#define TYPE_WATER		26
#define TYPE_SEA        27
#define TYPE_FOREST     28
#define TYPE_SWAMP      29
#define TYPE_PLAINS     30
#define TYPE_GRASSLAND  31
#define TYPE_ROAD       32
#define TYPE_CORNFIELD  33
#define TYPE_FIELD      34
#define TYPE_SNOW       35
#define TYPE_SNOWTREE   36
#define TYPE_SNOWPLAIN  37
#define TYPE_ICE        38
#define TYPE_LAVA       39
#define TYPE_HOTGROUND	40
#define TYPE_TREE		41
#define TYPE_BRIDGEV	42
#define TYPE_BRIDGEH	43
#define TYPE_DUNGEON1   60
#define TYPE_DUNGEON2   61
#define TYPE_DUNGEON3   62
#define TYPE_VILLAGE    63
#define TYPE_TOWN1		64
#define TYPE_TOWN2		65

/* these are ????? */ //note: These are trap tiles that don't match the traps,
//but are used later to show the revealed trap
#define TYPE_FIRSTTRAP  70
#define TYPE_TRAP_BOULDER 70
#define TYPE_TRAP_BOMB	71
#define TYPE_TRAPWATER	72
#define TYPE_TRAPROCK	73
#define TYPE_TRAPPIT	74

//Terrain data, ascii and color of tile etc.
struct Terradata
{
	char out;			// char to output
	int color;			// color of char
	int16u flags;		// terrain modifier flags
	int timemod;		// walk time modifier (percentage 1-xxx)
	const char *desc;	// short description
};

//Type of a terrain tile as defined in TYPE_...
struct Terratype
{
	int type;

	Terratype& operator=(const Terratype& right)
	{
		if(this == &right) return *this;
		type=right.type;
		return *this;
	}

	Terratype() : type(TYPE_DARK) { }
	explicit Terratype(int t) : type(t) { }

	bool Is_Door();
	bool Is_Portal();
	bool Is_Stairs();
};

extern const Terradata terrains[];

#endif
