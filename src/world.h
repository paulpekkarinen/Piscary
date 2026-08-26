//Legend of Saladir - world.h

//Unit world: Game world structure.

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>
#include "dungeon.h"
#include "gametime.h"
#include "types.h"
#include "weather.h"

class Levelnode;

//Game world data of the current world.
class World
{
private:
	friend class Debug;
	friend class Display;

	int index; //current level node (in what level the player is)
	std::vector<Levelnode*> levels; //list of levels in the game world
	Weather weather;
	Gametime worldtime;
	bool visited_dungs[dng::Max_Dungeons]; //keep track of visited dungeons

	typedef std::vector<Levelnode*>::iterator levitr;

	void Display_Time_Events(bool v);
	int Find_First_Level_Index(int dung);
	int Find_Level_By_Portal(int id);
	Levelnode *Find_Node_By_Location(int loc, int dung);
	const char *Get_Dungeon_Name();
	int Number_Of_Levels(int dung);

public:
	World();
	~World();

	level_type *Get_Current_Level();
	Levelnode *Get_Current_Node();
	int Get_Dungeon(); //which dungeon the level is
	int Get_Level_Index() { return index; };
	std::string Get_Level_Name();
	int Get_Level_Type(); //returns current level's type

	bool Has_Weather();
	bool Is_Matching_Place(int dung, int depth);
	bool Is_Night();
	bool Is_Outside() const;
	int Num_Levels();
	int Num_Places();

	void Advance_Time(int ticks);
	void Arrival();
	void Enter_Portal(int8u number);
	bool Leave_Town();
	void Remake_Current_Level();
	void Show_Birth_Time();
	void Show_Time();
	void Weather_Report();

	void Display_Overview();
	void Display_Level_Data(); //of current level
	void Display_Levels();
	void Display_Location();
	void Display_Return_Message(const char *plrname);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern World *world;

#endif
