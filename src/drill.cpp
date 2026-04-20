/**************************************************************************
 * drill.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 14.7.2022 - 7.9.2025 by Paul K. Pekkarinen

#include <cmath>
#include <cstring>
#include "avatar.h"
#include "dice.h"
#include "display.h"
#include "drill.h"
#include "game.h"
#include "gametime.h"
#include "input.h"
#include "inquire.h"
#include "lexicon.h"
#include "magic.h"
#include "message.h"
#include "output.h"
#include "use.h"

using std::string;

void init_skills()
{
	//init skill table
	for (int z=0; z<skillset::Max_Skill_Adv; z++)
	{
		real k=(8.0/100 * z)+1.8;

		SKILL_ADV[z]=(int)(10+exp(k));
	}

	//copy flags to magic skills
	int i=0;

	while (skills_magic[i].name != 0)
	{
		skills_magic[i].flags = list_spells[i].flags;
		i++;
	}
}

void use_quickskill(playerinfo *plr, level_type *level, int slot)
{
	if (slot >= NUM_QUICKSKILLS)
	{
		msg.newmsg("error! Trying to access quickskill array over boundaries!",
			CHB_RED);
		return;
	}

	if (plr->qskills[slot].select == 0)
	{
		msg.vnewmsg(C_WHITE, "Quickskill number %d not selected yet.", slot);
		return;
	}

	use_skill(plr, level, plr->qskills[slot].group, plr->qskills[slot].type);
}

//note: this routine is similar to skillset::listselect, but both seems to be used,
//not sure yet what the difference is
int skill_listselect(int group, const char *prompt)
{
	bool endreach=false;
	bool topreach=false;
	bool endoflist=false;

	skilltype *skilllist;
	if (group==SKILLGRP_WEAPON)
		skilllist=skills_weapon;
	else if (group==SKILLGRP_MAGIC)
		skilllist=skills_magic;
	else if (group==SKILLGRP_GENERIC)
		skilllist=skills_general;
	else
		skilllist=skills_illegal;

	clear_screen();
	my_setcolor(C_WHITE);

	if (!skilllist)
	{
		display->Error("Error! No skill list provided for skill_listselect().\n");
		return -1;
	}

	hidecursor();
	my_setcolor(C_GREEN);

	int box_sy=11;  /* size y */
	int box_sx=20;  /* size x */
	int box_bx=2;
	int box_by=2;
	makeborder(box_bx-1, box_by-1, box_sx+2, box_sy+2);

	if (prompt)
	{
		my_wordwraptext(prompt, box_by, SCREEN_LINES, (box_bx+box_sx+2),
			SCREEN_COLS);
	}

	my_wordwraptext(txt_listinstru, box_by+box_sy+2, SCREEN_LINES, box_bx,
		SCREEN_COLS);

	int loffs=0-(box_sy/2);
	int soffs=loffs;
	int eoffs=box_sy;
	char skillname[25]={0};
	int sel=0;

	while (1)
	{
		skilltype *sptr=skilllist+loffs;

		endreach=false;

		for (int i=0; i<box_sy; i++, sptr++)
		{

			if ((loffs+i)<0)
				topreach=true;
			else
				topreach=false;

			skillname[0]=0;

			if (!topreach)
			{
				if (!sptr->name)
				{
					endreach=true;
					if (!endoflist)
						eoffs=loffs+box_sy/2 - 1;
					endoflist=true;
				}

				if (!endreach)
				{
					my_strcpy(skillname, sptr->name, sizeof(skillname));
					if (strlen(skillname) > (size_t)box_sx)
						my_strcpy(skillname+(box_sx-3), "...", sizeof(skillname));
					skillname[0]=toupper(skillname[0]);
				}
			}

			if (i==box_sy/2)
				my_setcolor(CH_YELLOW);
			else
				my_setcolor(CH_DGRAY);

			gotoxy(box_bx, box_by+i);
			my_printf("%20s", "");
			const int skill_len=(int)strlen(skillname);
			gotoxy(box_bx+((box_sx/2)-skill_len/2), box_by+i);
			my_printf("%s", skillname);
		}

		gotoxy(box_bx+box_sx+2, box_bx+box_sy-1);
		set_color(C_GREEN);
		my_printf("%s skill ", skillgroupnames[group]);

		if (skilllist[sel].flags & SPF_ALTERATION)
			my_printf("of alteration");
		if (skilllist[sel].flags & SPF_DESTRUCTION)
			my_printf("of destruction");
		if (skilllist[sel].flags & SPF_MYSTICISM)
			my_printf("of mysticism");
		if (skilllist[sel].flags & SPF_OBSERVATION)
			my_printf("of observation");

		if (skilllist[sel].flags & SKILLAUTO)
		{
			my_setcolor(CH_RED);
			my_printf("*Automatic*");
		}
		clrtoeol();

		const int ch=my_getch();

		if ((ch=='z' || ch==KEY_DOWN || ch=='2') && loffs<eoffs)
		{
			loffs++;
			sel++;
		}
		if ((ch=='a' || ch==KEY_UP || ch=='8') && loffs>soffs)
		{
			loffs--;
			sel--;
		}
		if (is_confirm_key(ch))
			break;

		if (ch=='?')
		{
			/* erase description area first */
			for (int i=0; i<box_sy; i++)
				drawline_limit(box_by+i, box_bx+box_sx+2, SCREEN_COLS, ' ');

			set_color(C_WHITE);
			gotoxy(box_bx+box_sx+2, box_by);
			if ((skilllist+sel)->desc)
			{
				my_wordwraptext((skilllist+sel)->desc,
					box_by, SCREEN_LINES, (box_bx+box_sx+2),
					SCREEN_COLS);
			}
			else
				my_printf("No description for the skill.");
		}
	}

	showcursor();
	return sel;
}

bool use_skill(playerinfo *plr, level_type *level, int group, int skill)
{
	int sucval=0;
	int bonus=0;
	int skill2;

	/***************************************************/
	/***************************************************/
	/* note: ERI SKILLEILLE ERI AJANKULUTUS!!!!!!!!!!!!!!    */
	/***************************************************/
	/***************************************************/

	plr->Spend_Time(TIME_USESKILL);

	if (group>SKILLGRP_MAGIC)
	{
		group=0;
		skill=plr->skills.listselect(group,
			"Select a skill to apply.");
	}
	else if (skill < 0)
	{
		skill=plr->skills.listselect(group,
			"Select a skill to apply.");
	}

	Game.noticeevents(level);

	if (group > SKILLGRP_MAGIC)
	{
		msg.newmsg("Trying to use skill from illegal group.", CHB_RED);
		return false;
	}

	if (skill < 0)
		return false;

	int value=plr->skills.check(group, skill);

	if (value == 0)
		return false;

	/* do magic skills */
	if (group==SKILLGRP_MAGIC)
	{
		string s("You concentrate for ");

		/* check which group of spell it is */
		if (list_spells[skill].flags & SPF_ALTERATION)
		{
			skill2=SKILL_ALTERATION;
			s.append("alteration");
		}
		else if (list_spells[skill].flags & SPF_MYSTICISM)
		{
			skill2=SKILL_MYSTICISM;
			s.append("mysticism");
		}
		else if (list_spells[skill].flags & SPF_DESTRUCTION)
		{
			skill2=SKILL_DESTRUCTION;
			s.append("destruction");
		}
		else if (list_spells[skill].flags & SPF_OBSERVATION)
		{
			skill2=SKILL_OBSERVATION;
			s.append("observation");
		}
		else
		{
			msg.newmsg(CHB_RED,"Error: Spell %s has no area of effect!",
				list_spells[skill].name);
			return false;
		}

		sucval=plr->skills.check(SKILLGRP_GENERIC, skill2);

		s.append(" spell ");
		s.append(list_spells[skill].name);
		s.append("...");

		msg.newmsg(s, C_GREEN);

		/* give 5% chance for success if no skill in that area */
		if (!sucval)
			sucval=5;

		bonus=plr->stat[STAT_LUC].Get() - STATMAX_LUCK/2;
		if (bonus>0)
			sucval+=bonus*2;

		if (throwdice(1, 100, 0) <= sucval)
			msg.newmsg("Success!", C_WHITE);
		else
		{
			msg.newmsg("Failure!", C_WHITE);
			//	 player.sp-=throwdice(1, list_spells[skill].sp, 0);
			plr->mana.Decrease(1);

			GAME_NOTIFYFLAGS|=GAME_HPSPCHG;

			return false;
		}
		if (!spell_zap(plr, level, skill, value, false))
		{
			msg.newmsg("Spell failed.", C_WHITE);
			return false;
		}

		plr->skills.modify_raise(SKILLGRP_MAGIC, skill,
			1, 1);
		plr->skills.modify_raise(SKILLGRP_GENERIC, skill2,
			1, 1);
	}
	else if (group==SKILLGRP_GENERIC)
	{
		switch (skill)
		{
			case SKILL_SEARCHING:
				search_surroundings(plr, level, false);
				break;
			case SKILL_DISARMTRAP:
				disarm_trap(plr, level);
				break;
			default: break;
		}
	}
	else
	{
		msg.newmsg("Function not yet implemented.", C_RED);
	}
	return true;
}
