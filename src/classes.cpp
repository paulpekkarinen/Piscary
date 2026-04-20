//classes.cpp for Legend of Saladir
//(C)1997/1998 Erno Tuomainen

//Refactored 25.9.2021 - 8.6.2023 Paul K. Pekkarinen

#include "birth.h"
#include "classes.h"
#include "cornucop.h"
#include "skills.h"

void init_mage(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_fighter(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_rogue(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_hunter(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_ninja(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_knight(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);
void init_merchant(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp);

classdef classes[]=
{
   /* first class is for classless monsters */
   { "", NULL },
   { "ninja", init_ninja },
   { "fighter", init_fighter },
   { "knight", init_knight },
   { "mage", init_mage },
   { "priest", NULL },
   { "paladin", NULL },
   { "shaman", NULL },
   { "necromancer", NULL },
   { "caveman", NULL },
   { "ranger", NULL },
   { "farmer", NULL },
   { "rogue", init_rogue},
   { "merchant", init_merchant },
   { "tourist", NULL },
   { "healer", NULL },
   { "hunter", init_hunter },
   { NULL }
};

/* 80 points to distribute between weapon skills ?? */

void init_mage(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	/* fighter, good weapon skills */
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_DAGGER, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_STAFF, 30);

	Cornucopia cc(inv);
	cc.Giveskillweapon(skills);
}

void init_fighter(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	/* fighter, good weapon skills */
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_AXE, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

	Cornucopia cc(inv);
	cc.Giveskillweapon(skills);
}

void init_rogue(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_DAGGER, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 10);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 20);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_2WEAPCOMB, 10);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_1HWEAP, 15);

	Cornucopia cc(inv);
	cc.Rogue_Items();
}

void init_hunter(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_BOW, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_POLEARM, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_THROW, 20);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 10);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

	Cornucopia cc(inv);
	cc.Hunter_Items();
}

void init_ninja(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 30);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_THROW, 10);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_DAGGER, 10);

	Cornucopia cc(inv);
	cc.Ninja_Items();
}

void init_knight(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 25);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_AXE, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_HAND, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_2HWEAP, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

	Cornucopia cc(inv);
	cc.Knight_Items();
}

void init_merchant(skillset &skills, inventory &inv, statpack *stats, hpslot *hpp)
{
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_BOW, 75);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_POLEARM, 15);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_THROW, 60);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_SWORD, 45);
	skills.add_new_skill(SKILLGRP_WEAPON, SKILL_1HWEAP, 10);

	Cornucopia cc(inv);
	cc.Merchant_Items();
}
