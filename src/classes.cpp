//classes.cpp for Legend of Saladir
//(C)1997/1998 Erno Tuomainen

//Refactored 25.9.2021 - 8.6.2023 Paul K. Pekkarinen

#include "being.h"
#include "birth.h"
#include "classes.h"
#include "cornucop.h"
#include "skills.h"

Class_Skill mage_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_DAGGER, 25},
	{SKILLGRP_WEAPON, SKILL_HAND, 25},
	{SKILLGRP_WEAPON, SKILL_STAFF, 30},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill fighter_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_SWORD, 25},
	{SKILLGRP_WEAPON, SKILL_AXE, 15},
	{SKILLGRP_WEAPON, SKILL_HAND, 15},
	{SKILLGRP_WEAPON, SKILL_2HWEAP, 15},
	{SKILLGRP_WEAPON, SKILL_1HWEAP, 10},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill rogue_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_DAGGER, 25},
	{SKILLGRP_WEAPON, SKILL_SWORD, 10},
	{SKILLGRP_WEAPON, SKILL_HAND, 20},
	{SKILLGRP_WEAPON, SKILL_2WEAPCOMB, 10},
	{SKILLGRP_WEAPON, SKILL_1HWEAP, 15},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill hunter_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_BOW, 25},
	{SKILLGRP_WEAPON, SKILL_POLEARM, 15},
	{SKILLGRP_WEAPON, SKILL_THROW, 20},
	{SKILLGRP_WEAPON, SKILL_SWORD, 10},
	{SKILLGRP_WEAPON, SKILL_1HWEAP, 10},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill ninja_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_HAND, 30},
	{SKILLGRP_WEAPON, SKILL_SWORD, 15},
	{SKILLGRP_WEAPON, SKILL_2HWEAP, 15},
	{SKILLGRP_WEAPON, SKILL_THROW, 10},
	{SKILLGRP_WEAPON, SKILL_DAGGER, 10},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill knight_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_SWORD, 25},
	{SKILLGRP_WEAPON, SKILL_AXE, 15},
	{SKILLGRP_WEAPON, SKILL_HAND, 15},
	{SKILLGRP_WEAPON, SKILL_2HWEAP, 15},
	{SKILLGRP_WEAPON, SKILL_1HWEAP, 10},
	{SKILLGRP_ENDLIST, 0, 0}
};

Class_Skill merchant_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_BOW, 75},
	{SKILLGRP_WEAPON, SKILL_POLEARM, 15},
	{SKILLGRP_WEAPON, SKILL_THROW, 60},
	{SKILLGRP_WEAPON, SKILL_SWORD, 45},
	{SKILLGRP_WEAPON, SKILL_1HWEAP, 10},
	{SKILLGRP_ENDLIST, 0, 0}
};

classdef class_data[]=
{
	/* first class is for classless monsters */
	{"", 0 },
	{"ninja", ninja_skills},
	{"fighter", fighter_skills},
	{"knight", knight_skills},
	{"mage", mage_skills},

	{"priest", 0},
	{"paladin", 0},
	{"shaman", 0},
	{"necromancer", 0},
	{"caveman", 0},

	{"ranger", 0},
	{"farmer", 0},
	{"rogue", rogue_skills},
	{"merchant", merchant_skills},
	{"tourist", 0},

	{"healer", 0},
	{"hunter", hunter_skills}
};

/* note: 80 points to distribute between weapon skills ?? */

void initialize_class(being *b, int c)
{
	//add starting skills if any found
	Class_Skill *sk=class_data[c].starting_skills;

	if (sk!=0)
	{
		int i=0;
		while (sk[i].group!=SKILLGRP_ENDLIST)
		{
			b->skills.add_new_skill(
				sk[i].group, sk[i].type, sk[i].amount);
			i++;
		}
	}

	//add class specific items
	Cornucopia cc(b->inv);
	switch (c)
	{
		case CLASS_FIGHTER:
		case CLASS_MAGE:
			cc.Giveskillweapon(b->skills);
		break;
		case CLASS_ROGUE:
			cc.Rogue_Items();
		break;
		case CLASS_HUNTER:
			cc.Hunter_Items();
		break;
		case CLASS_NINJA:
			cc.Ninja_Items();
		break;
		case CLASS_KNIGHT:
			cc.Knight_Items();
		break;
		case CLASS_MERCHANT:
			cc.Merchant_Items();
		break;
		default: break;
	}
}
