//Legend of Saladir - qskills.h

//Unit qskills: Quick skills.

#ifndef QSKILLS_H
#define QSKILLS_H

struct skill;
class Tar_Ball;

#define NUM_QUICKSKILLS 10

struct quickskill
{
	int group;
	int type;
	bool select;

	bool Is_Selected(skill *src);
	void Reset();
	void Toggle(skill *src);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
