/**************************************************************************
 * skill.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 12.6.2022 - 8.5.2023 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include "magic.h"
#include "skills.h"
#include "storage.h"

int SKILL_ADV[skill::Max_Adv+1];

const char *skillgroupnames[]=
{
	"all",
	"weapon",
	"generic",
	"magic",
	NULL
};

skilltype skills_weapon[]=
{
	{"weaponless combat",
	"Weaponless combat skill affects your fighting ability when you fight "
	"without weapons, using your hands and feet only.",
	SKILLAUTO},

	{"daggers",
	"Daggers are small but deadly blades. Apart from their small size, "
	"they can be lethal when used properly. This skill tells how good "
	"you're in handling daggers.",
	SKILLAUTO},

	{"swords",
	"Swords are blades bigger and heavier than daggers. Swords are slower "
	"to use but they issue superior damage to daggers. This skill is for "
	"both one handed and two handed swords.",
	SKILLAUTO},

	{"axes",
	"Axes and battle axes.",
	SKILLAUTO},

	{"blunt weapons",
	"Blunt weapons are weapons of force. Hammers and clubs.",
	SKILLAUTO},

	{"polearms",
	"Polearms are long, sharp poles. Proper use requires good handling.",
	SKILLAUTO},

	{"staffs",
	"Staffs are often used by mages.",
	SKILLAUTO},

	{"bows",
	"Bows and crossbows. These weapons are not melee weapons, they are used "
	"to attack targets from a distance.",
	SKILLAUTO},

	{"crossbows", 0, SKILLAUTO},

	{"throwing",
	"Not only bows can be used to attack distant foes, you can also throw "
	"items with your bare hands.",
	SKILLAUTO},

	{"two handed weapons",
	"If you possess this skill, you gain bonuses when using two handed "
	"weapons. These bonuses depend on the level of this skill.",
	SKILLAUTO},

	{"one handed weapons",
	"If you possess this skill, you gain bonuses when using one handed "
	"weapons. These bonuses depend on the level of this skill.",
	SKILLAUTO},

	{"two weapon combat",
	"Fighting successfully with two weapons, one in both of your hands, "
	"requires good handling of the weapon you're using and also a skill "
	"of using two weapons simultaneously.",
	SKILLAUTO},

	{"shield",
	"A shield or a buckler is a powerful tool in the hands of a skilled "
	"warrior. This skill directly affects your attempts to use shield "
	"items as protective equipment. Train it well and survive.",
	SKILLAUTO},

	{0, 0, 0}
};

skilltype skills_general[]=
{
	{"food gathering",
	"Food gathering skill allows you to get more food out of corpses and "
	"several other sources. The better your knowledge is in this area, "
	"the more you are able to gather food.",
	SKILLAUTO},

	/* magic group skills */
	{"observation",
	"Observation skill is learned at the Magic School of Observation. "
	"The school of Observation teaches ways to examine the real nature "
	"of things. The skill itself affects directly to your Observation "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Observation. It also can affect your ability "
	"to learn new Observation spells.",
	SKILLAUTO},

	{"alteration",
	"Alteration skill is learned at the Magic School of Alteration. "
	"The school teaches magical methods and ways to alter the nature "
	"of things. The skill itself affects directly to your Alteration "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Alteration. It also can affect your ability "
	"to learn new Alteration spells.",
	SKILLAUTO},

	{"mysticism",
	"Mysticism skill is learned at the Magic School of Mysticism. "
	"The school teaches methods to create illusions and temporary "
	"alterations of nature. The skill itself affects directly to "
	"your Mysticism spells. Owning a high skill in this area "
	"produces more successes when casting spells of Mysticism. "
	"It also can affect your ability to learn new Mysticism spells.",
	SKILLAUTO},

	{"destruction",
	"Destruction skill is learned at the Magic School of Destruction. "
	"The school teaches methods to permanently harm or destruct things "
	"of nature. The skill itself affects directly to your Destruction "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Destruction. It also can "
	"affect your ability to learn new Destruction spells.",
	SKILLAUTO},

	{"concentration",
	"Concentration is an important skill for mages and other "
	"spellcasters. It makes possible to cast hard spells more easily. "
	"As your concentration skill increases, it's more likely to get "
	"successes when casting spells.",
	SKILLAUTO},

	{"mana flow",
	"The Mana flow skill tells how closely your mind and the forces of the "
	"Universe are tied together. You regenerate magic points faster on high "
	"levels of this skill.",
	SKILLAUTO},

	{"healing",
	"The skill of healing tells how well you can take care your wounds. "
	"Every being has it's own healing rate, but this skill will improve it if "
	"the skill is in your knowledge.",
	SKILLAUTO},

	{"find weakness",
	"With this skill you can find weak spots from your enemies. High skill "
	"causes more critical hits.",
	SKILLAUTO},

	{"searching",
	"\007The searching skill helps you to find hidden items, traps and "
	"other special locations. The skill will be used when you issue the "
	"search command.\n\n"
	"You can also move with care and be more observant. To do this, turn "
	"the searching mode on. This will slow you down, but increases the "
	"chances for you to find something hidden. "
	"(Use the command \001:s\007).",
	0},

	{"disarm trap",
	"The skill makes it possible to disarm traps whose location is "
	"known to you. If you fail to disarm a trap, it will most probably "
	"activate and cause much unhappiness for you and your body. "
	"You've been warned.",
	0},

	/* skills not used yet below */
	{"Survival", NULL, SKILLAUTO},
	{"Haggling", NULL, SKILLAUTO},
	{"Farming", NULL, SKILLAUTO},
	{"Hunting", NULL, SKILLAUTO},
	{"Preservation", NULL, SKILLAUTO},
	{"Mining", NULL, SKILLAUTO},
	{"Observation", NULL, SKILLAUTO},
	{"Swimming", NULL, SKILLAUTO},
	{"Climbing", NULL, SKILLAUTO},
	{"Hiding", NULL, SKILLAUTO},
	{"Sneaking", NULL, SKILLAUTO},
	{"Concentration", NULL, SKILLAUTO},
	{"Dodging", NULL, SKILLAUTO},
	{0, 0, 0}
};

skilltype skills_magic[]=
{
	{list_spells[SPELL_HEALING].name,
	"Healing spell makes it possible to heal wounded people, creatures "
	"and yourself.",
	0},

	{list_spells[SPELL_TELEPORT].name,
	"Teleporting spell creates a temporary dimension door allowing items "
	"and/or creatures to move between different planes of existence "
	"(locations).\n\n"
	"The power of the teleporting effect depends on your skill level, "
	"higher levels allow you to use teleporting on selected targets as "
	"well as on yourself.",
	0},

	{list_spells[SPELL_IDENTIFY].name,
	"Identify is a very useful and powerfull spell. It allows you to "
	"reveal the real identity of unknown items. It goes without saying "
	"that it can be used to reveal cursed items before using them.",
	0},

	{list_spells[SPELL_BLESS].name,
	"Bless spell allows you to bless items. Blessed item can be more "
	"powerful in some situations. Higher levels of the bless spell allows "
	"you to bless creatures as well.",
	0},

	{list_spells[SPELL_CURSE].name,
	"Curse spell is the exact and violent opposite of bless spell. "
	"It allows you to curse items and/or creatures depending on your "
	"skill level.",
	0},

	{list_spells[SPELL_UNCURSE].name,
	"The spell of remove curse is often a spell of relief. It makes "
	"possible to uncurse items and/or creatures.",
	0},

	{list_spells[SPELL_CONFUZE].name,
	"This spell allows you to temporarily confuze other creatures, "
	"thus making them a better target for your attacks. This cloud of "
	"confuzion can also be used for fleeing the battle area, if so "
	"desired.\n\n"
	"Higher skill level makes it possible to select your target more "
	"freely.",
	0},

	{0, 0, 0}
};

skilltype skills_illegal[]=
{
	{"Illegal skill group!", 0, 0},
	{0, 0, 0}
};

skilltype *skill::Get_Data()
{
	skilltype *ptr;

	if (group==SKILLGRP_WEAPON)
		ptr=&skills_weapon[type];
	else if (group==SKILLGRP_MAGIC)
		ptr=&skills_magic[type];
	else if (group==SKILLGRP_GENERIC)
		ptr=&skills_general[type];
	else
		ptr=&skills_illegal[0];

	return ptr;
}

const char *skill::Get_Name()
{
	skilltype *ptr=Get_Data();

	return ptr->name;
}

void skill::clamp(int &v)
{
	//test min and max values
	if (v < min)
		v=min;
	if (v > max)
		v=max;
}

void skill::reset(int g, int t, int v)
{
	max=MAX_SKILLSCORE;
	min=MIN_SKILLSCORE;
	group=g;
	type=t;
	cur=v;
	ini=v;
	level=0;
	raise=0;
	dice_t=1;     /* 1d6 */
	dice_s=6;

	clamp(cur);
	clamp(ini);
}

void skill::learn(int v)
{
	cur+=v;
	clamp(cur);
}

void skill::save(Tar_Ball &tb)
{
	tb.Put(group);
	tb.Put(type);

	tb.Put(ini);
	tb.Put(cur);
	tb.Put(min);
	tb.Put(max);

	tb.Put(raise);
	tb.Put(dice_t);
	tb.Put(dice_s);
	tb.Put(level);
}

void skill::load(Tar_Ball &tb)
{
	group=tb.Get_Next_Value();
	type=tb.Get_Next_Value();

	ini=tb.Get_Next_Value();
	cur=tb.Get_Next_Value();
	min=tb.Get_Next_Value();
	max=tb.Get_Next_Value();

	raise=tb.Get_Next_Value();
	dice_t=tb.Get_Next_Value();
	dice_s=tb.Get_Next_Value();
	level=tb.Get_Next_Value();
}
