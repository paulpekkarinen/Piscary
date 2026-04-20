//Legend of Saladir - school.h

//Unit school: Where the character studied.

#ifndef SCHOOL_H
#define SCHOOL_H

struct playerinfo;

struct School
{
	//int selected_teachers[NUM_TEACHERS+1];

	int Study(playerinfo &plr, bool automatic);
};

#endif
