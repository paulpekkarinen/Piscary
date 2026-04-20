//qskills.h

//Unit qskills: Quick skills.

#ifndef QSKILLS_H
#define QSKILLS_H

class Tar_Ball;

#define NUM_QUICKSKILLS 10

struct quickskill
{
	int group;
	int type;
	bool select;

	void Reset();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
