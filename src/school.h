//Legend of Saladir - school.h

//Unit school: Where the character studied.

#ifndef SCHOOL_H
#define SCHOOL_H

#include <vector>

struct playerinfo;

class School
{
private:
	int teacher;
	int school;
	int learnvalue;

	//if automatic is true selects randomly without player input
	bool automatic;

	std::vector<const char*> names;
	std::vector<const char*> school_names;

	void Orientation(playerinfo &plr);
	bool Select_Teacher(int age);

public:
	School(bool a);

	int Study(playerinfo &plr);
};

#endif
