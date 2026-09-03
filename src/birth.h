//Legend of Saladir - birth.h

//Unit birth: Adjust character skills, items. etc. based on education etc.

#ifndef BIRTH_H
#define BIRTH_H

#include <string>
#include "types.h"

class gamedata;
struct statpack;

#define NUM_RANDEFFECT 2
#define RANDEFF_INHERIT 0
#define RANDEFF_PLAGUE  1
#define RANDEFF_BANDITS 2

//if this is defined the character creation is faster (for testing the game)
#define debug_birth

class Birth
{
private:
	int used_effects[NUM_RANDEFFECT+1];

	std::string Give_Random_Name();

	void Ask_Skills();
	void Ask_Stats(bool automatic);
	void Randomeffect(int age);
	void Random_Stats(statpack *stats);
	int Skill_Listselect(int group, const char *prompt);

public:
	Birth() noexcept;

	int Fast_Action();
	int Player_Born(gamedata *mygame);
};

#endif
