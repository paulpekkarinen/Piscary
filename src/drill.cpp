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

#include <cstring>
#include "avatar.h"
#include "dice.h"
#include "drill.h"
#include "game.h"
#include "gametime.h"
#include "inquire.h"
#include "magic.h"
#include "message.h"
#include "skills.h"
#include "use.h"

using std::string;

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
