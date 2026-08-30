//Legend of Saladir - skillset.h

//Unit skillset: List of current skills.

#ifndef SKILLSET_H
#define SKILLSET_H

#include <list>

struct skill;

//List of skills the character has.
class skillset
{
private:
	std::list<skill*> skills;

	skill *find_skill(int group, int type);
	int Needmarks(int group, int value);

	typedef std::list<skill*>::iterator sitr;

	bool Has_Skills(int group);

public:
	~skillset();

	int check(int group, int type); //check current value of a skill
	skill *get_skill_handle(int index); //get skill of index from the list

	skill *add_new_skill(int group, int type, int value);
	void add_skill(skill *s);
	bool apply();
	int build_skillarray(int group, skillset &mylist);
	bool list_skills(int group);
	void melee_learnskills(item_def *iptr, int amount);
	bool modify(int group, int type, int value, bool addit);
	bool modify_raise(int group, int type, int amount, int addval, bool plr);
	bool modify_raise(int group, int type, int amount, int addval);
	int listselect(int &group, const char *prompt);
	void raiselevel();

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
