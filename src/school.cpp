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

#define TEACHER_ALBINIA   0
#define TEACHER_ALREDOR   1
#define TEACHER_THORNDARR 2
#define TEACHER_MANDALIN  3
#define TEACHER_FERDINAND 4
#define TEACHER_ERIOL     5
#define NUM_TEACHERS      6

const char *list_teachers[]=
{
   "Albinia, the Sorceror of Dalmn",
   "Al'redor, the Black Wizard",
   "Thorndarr, the Elf Healer",
   "Mandalin, the Fighter",
   "Ferdinand, the High Paladin",
   "Eriol, the Ranger.",
   0
};

#define SCHOOL_ALTERATION  0
#define SCHOOL_DESTRUCTION 1
#define SCHOOL_MYSTICISM   2
#define SCHOOL_OBSERVATION 3
const char *list_magicschools[]=
{
   "School of Alteration",
   "School of Destruction",
   "School of Mysticism",
   "School of Observation",
   0
};

const char *txt_teach1=
"\007It's time to create your background. You have a choice of selecting "
"teachers you want to study with. Each teacher have a set of skills it "
"can teach you. Each phase of learning takes 3 years of your life time.\n\n";

const char *txt_teach2=
"\007You can choose a path for your magic studies.\n\n";

int School::Study(playerinfo &plr, bool automatic)
{
	int teacher = 0, school = 0;
	int yearstook=6+RANDU(4);

	//   my_printf("You were %d years old at the start of your studies.\n",
	//	     yearstook);
	clear_screen();

	while (yearstook < 20)
	{
		if (automatic)
		{
			teacher=RANDU(NUM_TEACHERS);
		}
		else
		{
			clear_screen();
			my_wordwraptext(txt_teach1, get_cursor_y(), SCREEN_LINES, 0, SCREEN_COLS);

			my_printf("At the age of %d you decided to ... \n\n", yearstook);

			teacher=select_textlist("Choose a teacher (Q to end studies)",
				list_teachers, true);

			if (teacher<0)
			{
				my_printf("You decide to end your studies.\n");
				return yearstook;
			}
		}

		yearstook+=3;
		int learnvalue=150 + RANDU(150);

		if (teacher==TEACHER_ALBINIA || teacher==TEACHER_ALREDOR ||
			teacher==TEACHER_THORNDARR)
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
					list_magicschools, false);
			}
			switch (school)
			{
				case SCHOOL_ALTERATION:
					plr.skills.modify_raise(
						SKILLGRP_GENERIC, SKILL_ALTERATION,
						learnvalue/2, true);
					break;
				case SCHOOL_DESTRUCTION:
					plr.skills.modify_raise(
						SKILLGRP_GENERIC, SKILL_DESTRUCTION,
						learnvalue/2, true);
					break;
				case SCHOOL_OBSERVATION:
					plr.skills.modify_raise(
						SKILLGRP_GENERIC, SKILL_OBSERVATION,
						learnvalue/2, true);
					break;
				case SCHOOL_MYSTICISM:
					plr.skills.modify_raise(
						SKILLGRP_GENERIC, SKILL_MYSTICISM,
						learnvalue/2, true);
					break;
				default:
					break;
			}
		}

		if (teacher==TEACHER_ALREDOR)
		{
			plr.Change_Alignment('c',
				100+RANDU(ALIGNMENT_LIMIT/2));
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_DAGGER,
				learnvalue, true);
			plr.skills.modify_raise(
				SKILLGRP_MAGIC, SPELL_CURSE,
				learnvalue/4, true);
		}

		if (teacher==TEACHER_ALBINIA)
		{
			plr.Change_Alignment('n',
				100 + RANDU(ALIGNMENT_LIMIT/2));
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_STAFF,
				learnvalue, true);
			plr.skills.modify_raise(
				SKILLGRP_MAGIC, SPELL_CONFUZE,
				learnvalue/4, true);
		}

		if (teacher==TEACHER_MANDALIN)
		{
			plr.Change_Alignment('n',
				100 + RANDU(ALIGNMENT_LIMIT/2));
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_SWORD,
				learnvalue, true);
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_SHIELD,
				learnvalue/2, true);
		}

		if (teacher==TEACHER_FERDINAND)
		{
			plr.Change_Alignment('l',
				100 + RANDU(ALIGNMENT_LIMIT/2));

			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_SWORD,
				learnvalue, true);
		}

		if (teacher==TEACHER_ERIOL)
		{
			plr.Change_Alignment('n',
				100 + RANDU(ALIGNMENT_LIMIT/2));

			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_BOW,
				learnvalue/2, true);
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_DAGGER,
				learnvalue/2, true);
			plr.skills.modify_raise(
				SKILLGRP_GENERIC, SKILL_FOODGATHER,
				learnvalue/2, true);
			plr.skills.modify_raise(
				SKILLGRP_GENERIC, SKILL_FINDWEAKNESS,
				learnvalue/4, true);
		}

		if (teacher==TEACHER_THORNDARR)
		{
			plr.Change_Alignment('l',
				100 + RANDU(ALIGNMENT_LIMIT/2));

			plr.skills.modify_raise(
				SKILLGRP_GENERIC, SKILL_HEALING,
				learnvalue/2, true);
			plr.skills.modify_raise(
				SKILLGRP_WEAPON, SKILL_DAGGER,
				learnvalue/4, true);
			plr.skills.modify_raise(
				SKILLGRP_GENERIC, SKILL_FINDWEAKNESS,
				learnvalue/4, true);
		}

		my_printf("\nYou studied 3 years with %s.\n",
			list_teachers[teacher]);
	}

#ifndef debug_birth
	showmore(false, false);
#endif

	return yearstook;
}
