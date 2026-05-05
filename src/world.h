//Legend of Saladir - world.h

//Unit world: Game world structure.

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "gametime.h"
#include "weather.h"

class Dungnode;
class Levelnode;
class Outworld;

#define STAIRUP1     0x01
#define STAIRUP2     0x02
#define STAIRDOWN1   0x03
#define STAIRDOWN2   0x04
#define STAIROUT     0x05

/* dungeon locations, mainly for NPC appearing list */
#define PLACE_MOUNTAINS		0
#define PLACE_DPRIMITIVE	1
#define PLACE_DTHANTHOL		2
#define PLACE_DABYSS		3
#define PLACE_TSANTHEL		4

#define MAX_DLEVELS  50

#define NUM_DUNGEONTYPES 4	//number of randomly selectable types (outworld is excluded)
#define DTYPE_MAZE   0x00	/* maze type level */
#define DTYPE_ROOMY  0x01	/* room type level */
#define DTYPE_ROOMY2 0x02	/* room type level with lots of doors */
#define DTYPE_MAZE2	0x03	/* deteriorated maze */
#define DTYPE_OUTWORLD 0x04 //outworld
#define DTYPE_TOWN	0x80	/* Town level! */
#define DTYPE_WILDHUNT 0xfe /* wilderness hunt */
#define DTYPE_RANDOM 0xff	/* level is randomly selected */

//Static data for a level.
struct Level
{
	const char *name;	/* name for this level */
	int index;			/* special index, needed for savefiles */
	int danger;			/* danger level for this level */
	int dtype;			/* required dungeon type */
	int outx;			/* if exiting to OUTSIDE here are the coordinates */
	int outy;			/* otherwise these are zero */

	Level *linkfrom1;	// which level exits to this level
	Level *linkto1;		// where does this level exit to
	Level *linkfrom2;
	Level *linkto2;
};

//Static data for a dungeon (a set of levels).
struct Dungeon
{
	const char *name;	/* name of this dungeon */
	const char *desc; //description of the place
	int placetype;		/* dungeon type */
	Level *levels;
	int8u staircase;	/* which staircase to enter */
	int danger;			/* danger level of this dungeon */
	int out;			/* terrain type to output if cave found */
};

//Game world data of the current world.
class World
{
private:
	friend class Debug;
	friend class Display;

	Levelnode *currnode; //current level node (in what level the player is)
	int dungeon; //in which dungeon player is, 0 is OUTWORLD
	std::vector<Dungnode*> dungeons; //main 'places' in the world, each has a list of levels
	std::vector<Coord> dunglocs; //list of dungeon locations in the overworld map
	Weather weather;
	Gametime worldtime;

	typedef std::vector<Dungnode*>::iterator dngitr;
	typedef std::vector<Coord>::iterator locsitr;

	void Display_Time_Events(bool v);
	Dungeon *Get_Dungeon(int dng);
	const char *Get_Dungeon_Name();
	void Jump_To_Stairs(level_type *level, int stairs_type, int8u number);
	void New_Dungeon_Location(const Plane &lvl); //determines a dungeon location
	void Set_Current_Node(Level *dest);

public:
	World();
	~World();

	level_type *Get_Current_Level();
	int Get_Dungeon_Index() const { return dungeon; }
	Level *Get_Level_Data(); //this is needed for stairs creation
	int Get_Level_Index();
	const char *Get_Level_Name();
	int Get_Level_Type(); //returns current level's type

	bool Is_First_Level_Of_Town();
	bool Is_Matching_Place(int ptype, int index); //check when creating a monster to a level
	bool Is_Night();
	bool Is_Outside() const;

	void Advance_Time(int ticks);
	void Connect_Dungeons(Outworld *owo, float v);
	void Create_Dungeon_Entrances(level_type *level); //put entrances to the level map
	void Determine_Dungeon_Locations(const Plane &lvl);
	void Enter_New_Dungeon(int d);
	bool Player_Go_Down(level_type *level);
	void Player_Go_Outworld();
	bool Player_Go_Up();
	void Show_Birth_Time();
	void Show_Time();
	void Visit_Dungeon(int index); //marks all dungeons with index 'index' visited
	void Weather_Report();

	void Display_Data();
	void Display_Level_Data(); //of current level
	void Display_Return_Message(const char *plrname); //when player is loaded show the world location

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern Dungeon dungeonlist[];
extern World *world;

#endif
