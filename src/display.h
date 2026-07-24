//Legend of Saladir - display.h

//Unit display: Shows stuff.

#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

struct Attribute;
struct statpack;
class World;

//Shows stuff, the plan is to keep this part abstract, without curses stuff
//in this header file.
class Display
{
private:
	static const int CONDITION_MAXNUM=10;
	static const int NUM_LOGOLIN=8;

	void Stat_Value(int wy, const char *txt, statpack *pstat);
	void World_Location(World *myworld); //show location in the game world

public:
	void Attribute_As(const char *what, Attribute &a);
	void Draw_Terrain(int ctype, int fov);
	void Error(const char *txt);
	void Footer_Error(const char *txt);
	void Footer_Failure(const char *txt);
	void Footer(const char *txt, int color);
	void Header(const char *txt, int color);
	void Hiscore_Header();
	void Item_Info(item_def *iptr, int weight, int count, const char *acttxt);
	void Journey_Log();
	void Monster_Description(being *mptr);
	void More_Inventory(const int y);
	void Player_Status(Actor &tonttu); //status screen
	void Redraw(level_type *level); //redraw everything on the gameplay screen
	void Selection_Prompt(int weight);
	void Stats(bool showmove); //print side and bottom line stats
	void Story();
	void Title();
	void Underline_Header(const char *txt, int color, int ucolor);
};

extern Display *display;

#endif
