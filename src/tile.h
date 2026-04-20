//Legend of Saladir - tile.h

//Unit tile: Tile of a dungeon map.

#ifndef TILE_H
#define TILE_H

#include "types.h"

class Tar_Ball;

//passage carving doesn't carve locations with sval of this
#define GENERATE_DONOTCARVE	99

//'flags' values of cave_type
#define CAVE_LIGHT			0x0001 // lit by default (static light source)
#define CAVE_EXPLORED		0x0002 // grid is explored (seen before)
#define CAVE_TRAPIDENT		0x0004 // is trap identified
#define CAVE_WATER			0x0008 // water
#define CAVE_MOUNTAIN		0x0010 // mountain
#define CAVE_PASSABLE		0x0020 // this grid can be walked into
#define CAVE_DOOR			0x0080
//#define CAVE_MONSTERLIGHT	0x2000 //note: these can be removed later
//#define CAVE_TMPLIGHT		0x4000 // temporary light
//#define CAVE_LOSTMP			0x8000

//game object types of 'object' flag
#define OBJECT_NONE 0x00
#define OBJECT_TRAP 0x01
#define OBJECT_DOOR 0x02

//flags of 'doorfl'
#define DOOR_OPEN		0x01 /* open door */
#define DOOR_CLOSED		0x02 /* closed but not locked */
#define DOOR_LOCKED		0x04 /* locked and closed */
#define DOOR_TRAPPED	0x08 /* trapped */
#define DOOR_STUCK		0x10 /* stuck door */

/* single grid in cave */
struct cave_type
{
	int16u flags;	//CAVE_... flags
	int type;		//TYPE_... terrain types in terrain.h
	int sval;		/* secret door level etc...*/

	//for staircases, doorfl is the staircase number (or some kind of direction)
	int8u doorfl;	/* door flags, see DOOR_... -values */

	int8u object;	//object type this tile has, using OBJECT_... defines

	void initialize(int tt);
	void jam_door();
	void stairs_down(int8u number);
	void stairs_up(int8u number);

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
