/**************************************************************************
 * school.cpp --                                                          *
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

//Refactored 23.2.2023 Paul K. Pekkarinen

#include "avatar.h"
#include "birth.h"
#include "dice.h"
#include "input.h"
#include "magic.h"
#include "output.h"
#include "roleplay.h"
#include "school.h"
#include "skills.h"

using std::vector;

//Skill data for each teacher.
struct School_Skill
{
	int group;
	int type;
	int divisor;
};

School_Skill alredor_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_DAGGER, 0},
	{SKILLGRP_MAGIC, SPELL_CURSE, 4},
	{SKILLGRP_ENDLIST, 0, 0}
};

School_Skill albinia_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_STAFF, 0},
	{SKILLGRP_MAGIC, SPELL_CONFUZE, 4},
	{SKILLGRP_ENDLIST, 0, 0}
};

School_Skill mandalin_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_SWORD, 0},
	{SKILLGRP_WEAPON, SKILL_SHIELD, 2},
	{SKILLGRP_ENDLIST, 0, 0}
};

School_Skill ferdinand_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_SWORD, 0},
	{SKILLGRP_ENDLIST, 0, 0}
};

School_Skill eriol_skills[]=
{
	{SKILLGRP_WEAPON, SKILL_BOW, 2},
	{SKILLGRP_WEAPON, SKILL_DAGGER, 2},
	{SKILLGRP_GENERIC, SKILL_FOODGATHER, 2},
	{SKILLGRP_GENERIC, SKILL_FINDWEAKNESS, 4},
	{SKILLGRP_ENDLIST, 0, 0}
};

School_Skill thorndarr_skills[]=
{
	{SKILLGRP_GENERIC, SKILL_HEALING, 2},
	{SKILLGRP_WEAPON, SKILL_DAGGER, 4},
	{SKILLGRP_GENERIC, SKILL_FINDWEAKNESS, 4},
	{SKILLGRP_ENDLIST, 0, 0}
};

#define TEACHER_ALBINIA   0
#define TEACHER_ALREDOR   1
#define TEACHER_THORNDARR 2
#define TEACHER_MANDALIN  3
#define TEACHER_FERDINAND 4
#define TEACHER_ERIOL     5
#define NUM_TEACHERS      6

struct Teacher
{
	const char *name;
	char alignment;
	bool has_academy;
	School_Skill *skills;
};

const Teacher teacher_data[NUM_TEACHERS]=
{
	{"Albinia, the Sorceror of Dalmn", 'n', true, albinia_skills},
	{"Al'redor, the Black Wizard", 'c', true, alredor_skills},
	{"Thorndarr, the Elf Healer", 'l', true, thorndarr_skills},
	{"Mandalin, the Fighter", 'n', false, mandalin_skills},
	{"Ferdinand, the High Paladin", 'l', false, ferdinand_skills},
	{"Eriol, the Ranger.", 'n', false, eriol_skills}
};

//===

#define SCHOOL_ALTERATION  0
#define SCHOOL_DESTRUCTION 1
#define SCHOOL_MYSTICISM   2
#define SCHOOL_OBSERVATION 3
#define NUM_SCHOOLS 4

struct Academy
{
	const char *name;
	int related_skill;
};

Academy magic_schools[NUM_SCHOOLS]=
{
	{"School of Alteration", SKILL_ALTERATION},
	{"School of Destruction", SKILL_DESTRUCTION},
	{"School of Mysticism", SKILL_MYSTICISM},
	{"School of Observation", SKILL_OBSERVATION}
};

//===

const char *txt_teach1=
"\007It's time to create your background. You have a choice of selecting "
"teachers you want to study with. Each teacher have a set of skills it "
"can teach you. Each phase of learning takes 3 years of your life time.\n\n";

const char *txt_teach2=
"\007You can choose a path for your magic studies.\n\n";

School::School(bool a)
	: teacher(0), school(0), learnvalue(0), automatic(a)
{
	//create list of teacher names
	for (int i=0; i<NUM_TEACHERS; i++)
		names.push_back(teacher_data[i].name);

	//create list of school names
	for (int i=0; i<NUM_SCHOOLS; i++)
		school_names.push_back(magic_schools[i].name);
}

void School::Orientation(playerinfo &plr)
{
	if (teacher_data[teacher].has_academy)
	{
		if (automatic)
		{
			school=RANDU(4);
		}
		else
		{
			my_wordwraptext(txt_teach2, get_cursor_y(),
				SCREEN_LINES, 0, SCREEN_COLS);

			school=select_textlist("Select a direction for your studies",
				school_names, false);
		}

		//modify the skill of this magic school
		plr.skills.modify_raise(
			SKILLGRP_GENERIC, magic_schools[school].related_skill,
			learnvalue/2, true);
	}
}

bool School::Select_Teacher(int age)
{
	bool rv=true;

	if (automatic)
	{
		teacher=RANDU(NUM_TEACHERS);
	}
	else
	{
		clear_screen();
		my_wordwraptext(txt_teach1, get_cursor_y(),
			SCREEN_LINES, 0, SCREEN_COLS);

		my_printf("At the age of %d you decided to ... \n\n", age);

		teacher=select_textlist("Choose a teacher ('q' to end studies)",
			names, true);

		if (teacher<0)
			rv=false;
	}

	return rv;
}

int School::Study(playerinfo &plr)
{
	int yearstook=6+RANDU(4);

	clear_screen();

	while (yearstook < 20)
	{
		if (Select_Teacher(yearstook)==false)
		{
			my_printf("\nYou decide to end your studies.\n");
			break;
		}

		//after studies...
		yearstook+=3;
		learnvalue=150 + RANDU(150);

		//take possible extra studies
		Orientation(plr);

		//affect alignment of the teacher
		plr.Change_Alignment(teacher_data[teacher].alignment,
			100+RANDU(ALIGNMENT_LIMIT/2));

		plr.skills.modify_raise(
			SKILLGRP_WEAPON, SKILL_DAGGER,
			learnvalue, true);

		//modify skills you learned
		School_Skill *sk=teacher_data[teacher].skills;

		int i=0;
		while (sk[i].group!=SKILLGRP_ENDLIST)
		{
			int amount=learnvalue;
			const int d=sk[i].divisor;
			if (d>0) amount=amount/d;

			plr.skills.modify_raise(
				sk[i].group, sk[i].type, amount, true);
			i++;
		}

		if (automatic==false)
		{
			my_printf("\nYou studied 3 years with %s.\n",
				teacher_data[teacher].name);
			showmore(false, false);
		}
	}

	if (automatic==false && yearstook<20)
		showmore(false, false);

	return yearstook;
}
