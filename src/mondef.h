//mondef.h

//Unit mondef: Static and dynamic creature data.

#ifndef MONDEF_H
#define MONDEF_H

#include <string>
#include "ability.h"
#include "types.h"

struct Npcrace;
class Tar_Ball;

//Static data for special npcs and shopkeepers, and dynamic data for all creatures.
struct monsterdef
{
	std::string desc; // short description, or title for the player
	std::string name; // if named monster or player, name
	int longdesc; // long description number (from desclist), zero if none

	int weight; // 1000 is 1kg
	int level; // creatures level
	int align; // alignment of the monster
	int special; // special monster type (zero if normal)

	int race; // human, elf... etc
	int mclass; // fighter, mage etc...
	int gender; //gender of the creature
	int attitude; //note: seems to be always zero, not sure what this is for
	int32u behave; // behaviour & status flags

	Ability stats; //starting stats (str, dex, int, etc.)

	int32u status; // status flags (is he alive, confuzed etc)

	monsterdef& operator=(const monsterdef& right)
	{
		if(this == &right) return *this;
		desc=right.desc;
		name=right.name;
		longdesc=right.longdesc;

		weight=right.weight;
		level=right.level;
		align=right.align;
		special=right.special;

		race=right.race;
		mclass=right.mclass;
		gender=right.gender;
		attitude=right.attitude;
		behave=right.behave;

		stats=right.stats;
		status=right.status;

		return *this;
	}

	void randomize(Npcrace *stdmon, int monrace); //initialize random values for this npc
	void playerize(); //make it the player
	void steer_alignment(char dir, int amount); //move alignment to some direction
	const char *Get_Name();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern monsterdef shopkeeper_list[];
extern monsterdef npc_list[];

#endif
