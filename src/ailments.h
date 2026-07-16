//Legend of Saladir - ailments.h

//Unit ailments: List of conditions.

#ifndef AILMENTS_H
#define AILMENTS_H

#include <list>

struct Actor;
class Condition;
class Tar_Ball;

//List of conditions.
class Ailments
{
	std::list<Condition*> conds;

	typedef std::list<Condition*> cndlist;
	typedef cndlist::iterator citr;

public:
	~Ailments();

	Condition* add(int cond, int value);
	void delete_group(int group);
	Condition* find(int cond);
	int get_value(int cond);
	void handle(Actor *actor, int slots);
	void init();
	void remove(int cond);
	void statshow(int y);

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
