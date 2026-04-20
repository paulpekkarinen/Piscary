//Legend of Saladir - ailments.h

//Unit ailments: List of conditions.

#ifndef AILMENTS_H
#define AILMENTS_H

#include <list>
#include "types.h"

struct Condition;

//List of conditions.
class Ailments
{
	std::list<Condition*> conds;

	typedef std::list<Condition*> cndlist;
	typedef cndlist::iterator citr;

	/* struct for condition description list */
	struct cond_desc
	{
		const char *name;
		const char *desc;
		int16u flags;
		int group;
		int color;
	};

	static const cond_desc cond_list[];

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
