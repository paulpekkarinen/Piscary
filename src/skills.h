//Legend of Saladir - skills.h

//Unit skills: Skills data and list of current skills.

#ifndef SKILLS_H
#define SKILLS_H

#include <list>
#include "items.h"
#include "types.h"

#define SKILLGRP_ALL		0
#define SKILLGRP_WEAPON 	1
#define SKILLGRP_GENERIC 	2
#define SKILLGRP_MAGIC  	3
#define SKILLGRP_ENDLIST	0xffff

/* these must be same as the weapon groups!!! */
#define SKILL_HAND      WPN_HAND
#define SKILL_DAGGER    WPN_DAGGER
#define SKILL_SWORD     WPN_SWORD
#define SKILL_AXE       WPN_AXE
#define SKILL_BLUNT     WPN_BLUNT
#define SKILL_POLEARM   WPN_POLEARM
#define SKILL_STAFF     WPN_STAFF
#define SKILL_BOW       WPN_BOW
#define SKILL_CROSSBOW  WPN_CROSSBOW
#define SKILL_THROW     9
#define SKILL_1HWEAP    10
#define SKILL_2HWEAP	11
#define SKILL_2WEAPCOMB	12
#define SKILL_SHIELD    13
#define SKILL_ENDLIST   0xffff
#define NUM_WEAPONSKILLS SKILL_1HWEAP

/* general skills */
#define SKILL_FOODGATHER        0
#define SKILL_OBSERVATION       1 /* magic skill */
#define SKILL_ALTERATION        2 /* magic skill */
#define SKILL_MYSTICISM         3 /* magic skill */
#define SKILL_DESTRUCTION       4 /* magic skill */
#define SKILL_CONCENTRATION     5 /* magic */
#define SKILL_MANAFLOW          6
#define SKILL_HEALING           7
#define SKILL_FINDWEAKNESS      8
#define SKILL_SEARCHING         9
#define SKILL_DISARMTRAP        10

#define SKILL_MAXLEV	10	/* levels 0..10 per skill */

#define MAX_SKILLSCORE	100	/* skills are 0..100% */
#define MIN_SKILLSCORE	5

/* flags 0x01 - 0x08 are taken by magic */
#define SKILLAUTO       0x00000010 /* automatic skill */
#define SKILLAPPLY      0x00000020 /* manual use skill */

struct skill
{
	int group;	// skill group
	int type;	// skill type inside that group

	int ini;		// initial value
	int cur;		// learned value
	int min;		// minimum value
	int max;		// maximum value

	int raise;	// raise counter
	int dice_t;	// raise dice
	int dice_s;
	int level;	// skill level

	void reset(int g, int t, int v);
	void learn(int v);
	void clamp(int &v);

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

struct skilltype
{
	const char *name;
	const char *desc;
	int32u flags;
};

//List of skills the character has.
class skillset
{
private:
	std::list<skill*> skills;

	skill *find_skill(int group, int type);
	int Needmarks(int group, int value);

	typedef std::list<skill*>::iterator sitr;

public:
	static const int Max_Skill_Adv=101;

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

extern int SKILL_ADV[];
extern skilltype skills_general[];
extern skilltype skills_illegal[];
extern skilltype skills_magic[];
extern skilltype skills_weapon[];
extern const char *skillgroupnames[];
extern const char *txt_listinstru;

#endif
