//Legend of Saladir - aim.h

//Unit aim: Target something on gameview.

#ifndef AIM_H
#define AIM_H

#include "geometry.h"

class Actor;
struct level_type;

struct Aim_Data
{
	const char *info_line;
	const char *monster_prefix;
	const char *monster_suffix;
	const char *item_prefix;
	const char *itempile;
	const char *terrain_suffix;
	bool show_targetline;
	char target_char;
};

class Aim
{
public:
	enum Aim_Modes
	{
		Look_Around,
		Target,
		Amt_Of_Modes
	};
	
private:
	level_type *level;
	Actor *olento;
	Coord pos; //location on level
	int mode;
	int lastidx; //-1 if no monster found

	void Show_Info();
	void Show_Spot_Info();
	void Starting_Location();
	
public:
	Aim(level_type *lvl, int tm);

	Coord Get_Target_Location();

	bool Select();
};

#endif
