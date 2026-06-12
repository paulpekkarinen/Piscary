//Legend of Saladir - aim.h

//Unit aim: Target something on gameview.

#ifndef AIM_H
#define AIM_H

#include "target.h"

struct level_type;
struct Spell;

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
		Missile_Target,
		Spell_Target,
		Amt_Of_Modes
	};

private:
	level_type *level;
	Target target;
	int mode;
	int lastidx; //-1 if no monster found
	Spell *spell; //only for spell targetting

	bool Can_Target_Creature();
	bool Can_Target_Item();
	bool Can_Target_Yourself();
	void Show_Info();
	void Show_Spot_Info();
	void Starting_Location();

public:
	Aim(level_type *lvl, int tm);
	Aim(level_type *lvl, Spell *sp);

	Coord Get_Target_Location();
	Target Get_Target() { return target; }

	bool Select();
};

#endif
