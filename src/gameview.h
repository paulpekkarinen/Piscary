//Legend of Saladir - gameview.h

//Unit gameview: 2D view array for the level data like room id's, top item etc.

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "fov.h"
#include "geometry.h"
#include "types.h"

class Bytegrid;
struct Viewtile;

//Gameview makes some operations faster, like finding out in what room
//the player is or what is the topmost item visible on level. It doesn't
//own or contain any objects and when new level is entered the gameview is
//re-created for that level.
class Gameview
{
private:
	friend class Pathlist;

	//how close to the edge until gameview is scrolled
	static const int Horizontal_Limit=9;
	static const int Vertical_Limit=6;
	
	level_type *level; //points to current level
	Viewtile *data; //level-sized array for gameview data
	Bytegrid *dirgrid; //data for pathfinder
	Rectangle view; //view window location and size
	Coord camera; //view location
	Fov fov; //field-of-vision routine

	int Get_Index(const Coord &c) const;
	void Clamp_Camera_X();
	void Clamp_Camera_Y();
	void Clamp_Camera();
	void Show_Tile(int x, int y);

public:
	Gameview();
	~Gameview();

	bool Cansee(const Coord &mc, const Coord &pc, int dist);
	int Count_Items(const Coord &c);
	Viewtile *Get(const Coord &c); //get viewtile at this location
	Actor *Get_Actor(const Coord &c); //returns monster & player base class
	int Get_Room_Id(const Coord &c);
	Coord Get_Screen_Location(const Coord &c);
	invnode *Get_Item(const Coord &c); //returns topmost item here
	being *Get_Monster(const Coord &c); //returns only monsters, not the player
	int Get_Trap_Type(const Coord &c); //return TRAP... type of trap here

	bool Is_Outside_View(const Coord &c);
	bool Is_Visible(const Coord &c);
	bool Is_Visible(int x, int y);

	void Center(const Coord &c);
	void Clear_Fov(const Coord &c);
	void Clear_Item(const Coord &c);
	void Enter_New_Level(level_type *kentta);
	void Land_Item(invnode *item, const Coord &c);
	void Monster_Torchlos(being *b);
	bool Notice_Something(being *b, bool items_too);
	void Set_Fov(const Coord &c);
	void Put_Item(invnode *i);
	void Put_Monster(being *b, const Coord &c);
	void Put_Trap(const Coord &c, int tt);
	void Refresh_Item_Map(const Coord &c);
	void Set_Room_Id(const Coord &c, int rid);
	void Show();
	void Show_Data();
	void Show_Tile_Data(const Coord &c);
};

extern Gameview gameview;

#endif
