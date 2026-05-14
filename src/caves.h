//Legend of Saladir - caves.h

//Unit caves: level_type contains level tiles, items, rooms and monsters.

#ifndef CAVES_H
#define CAVES_H

#include "crew.h"
#include "rooms.h"
#include "stash.h"
#include "tile.h"
#include "traps.h"

struct playerinfo;

//maximum level sizes x and y
#define MAXSIZEX	160
#define MAXSIZEY	60

//door creation chances
#define CHANCE_DOORTRAP   20
#define CHANCE_STUCKDOOR  25
#define CHANCE_SECRETDOOR 15

struct level_type
{
	int sizex;		// level width
	int sizey;		// level height
	int basetile;	//tile the level is cleared with
	int danglev;	// level danger level

	std::vector<roomdef> rooms; //array of rooms in the level
	Stash inv; //level items
	Crew crew; //level monsters
	Traps traps; //level traps

	// memory locations for every horizontal row in the level map
	cave_type *loc[MAXSIZEY+1];

	level_type(int width, int height, int bt, int danger);
	level_type(Tar_Ball &tb); //loader constructor
	~level_type();

	int Checkterraineffects(int x, int y);
	bool Find_Stairs(Coord &here, int stair_type, int8u number);
	bool Free_To_Walk(const Coord &c); //can walk to this tile

	int get_amount_of_rooms();
	roomdef& get_last_created_room();
	const char *Get_Name(); //does just return a "level"
	Coord Get_Random_Good_Location();
	int Get_Terrain(const Coord &c);
	Trap &Get_Trap(const Coord &c);

	bool Has_Door(const Coord &c);
	bool Has_Object(const Coord &c, int8u flag);
	bool Has_Static_Light(const Coord &c);
	bool Has_Stairs(const Coord &c);

	bool Inside_Room(int roomnum, const Coord &c);
	bool Is_Blocking_Los(const Coord &c);
	bool Is_Carveable(const Coord &c);
	bool Is_Closed_Door(const Coord &c);
	bool Is_Door(const Coord &c);
	bool Is_Explored(int y, int x);
	bool Is_Open_Door(const Coord &c);
	bool Is_Outside(int x, int y);
	bool Is_Outside(const Coord &c);
	bool Is_Outside_Border(int x, int y); //includes a border around the level
	bool Is_Passable(const Coord &c);
	bool Is_Passable(int x, int y);
	bool Is_Secretdoor(const Coord &c);
	bool Is_Wall(const Coord &c);
	bool Is_Wall(int x, int y);
	bool max_rooms_created();

	void allocate_terrain_map();
	void clear_data();
	void Clear_Flag(const Coord &c, int16u flag);
	void Clear_Object(const Coord &c, int8u flag);
	void Clear_Terrain(int terraintype); //clear the entire level's terrain
	bool Close_Door(const Coord &c);
	void Refresh_Gameview(); //copy current level data to gameview
	bool Open_Door(const Coord &c);
	void Plot_Maze(int x, int y);
	void Remove_Dead_Monsters(); //if any died during a turn
	void remove_room_owner(being *b);
	void Remove_Trap(const Coord &c);
	void reveal(); //make the whole level visible
	void Reveal_Trap(const Coord &c);
	void Set_Flag(const Coord &c, int16u flag);
	void Set_Object(const Coord &c, int8u flag);
	void set_room_owner(int roomid, being *b);
	void Set_Seen(const Coord &c);
	void Set_Terrain(int x, int y, int terratype);
	void Set_Terrain(const Coord &c, int terratype);
	void Shop_Init(); //put shopkeeper and items in shop rooms

	void Display_Tile(int y, int x);
	void Display_Data();
	void List_Rooms();
	void List_Terrain();
	void Show_Tile_Description(const Coord &c);
	void Show_Leveltile_Data(const Coord &c);

	void save(Tar_Ball &tb);
};

extern level_type *c_level; //points to current level

#endif
