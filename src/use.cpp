/**************************************************************************
 * use.cpp --                                                             *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 28.2.2023 - 7.4.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "codex.h"
#include "dice.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "output.h"
#include "script.h"
#include "trapdash.h"
#include "use.h"

using std::string;

bool is_handle_stuck(Actor *tonttu, level_type *level, const Coord &c);

void close_door(playerinfo *plr, level_type *level)
{
	int dir=dir_askdir("Close door -", true);
	if (dir>9) return;

	Coord nc=move_to_direction(dir, plr);

	if (plr->Is_At(nc))
	{
		msg.newmsg("Close your mouth.", C_WHITE);
	}
	else if (close_door(plr, level, nc)==DOORSTAT_SUCCESS)
	{
		plr->Spend_Time(TIME_CLOSEDOOR);
	}
}

int close_door(Actor *tonttu, level_type *level, const Coord &c)
{
	const bool is_plr=tonttu->Is_Player();

	if (level->Is_Door(c)==false)
	{
		if (is_plr)
			msg.newmsg("That isn't a door.", C_WHITE);

		return DOORSTAT_NODOOR;
	}
	else if (level->Is_Closed_Door(c))
	{
		if(is_plr)
			msg.newmsg("The door is closed!", C_WHITE);

		return DOORSTAT_ALREADY_OPEN;
	}

	if (level->Is_Open_Door(c))
	{
		/* if doorway blocked by an item */
		if(level->inv.Count_Items_Flat(c.x, c.y)>0)
		{
			if (is_plr)
				msg.newmsg("The door won't stay closed.", C_WHITE);
			return DOORSTAT_STUCK;
		}

		Actor *mptr=gameview.Get_Actor(c);
		if (mptr && is_plr)
		{
			string s=monster_sprintf(mptr, true, true);
			s.append(" is there, slam the door");
			if(confirm_yn(s.c_str(), false, true))
			{
				/* maybe stun the monster when door slams shut ? */
				msg.add("You slam the door.", C_CYAN);

				/* note: issue some damage here!!! */

				/* make monster angry to player */
				mptr->Getangry(level, tonttu, true);
			}
			else
				return DOORSTAT_STUCK;
		}

		/* if action by player */
		if (is_plr)
		{
			texts->Chain_Message(Script::Door_Sound, C_YELLOW);
		}
		/* if action by monster */
		else
		{
			string s=monster_sprintf(tonttu, true, true);
			s.append(" closed a door.");
			msg.add_dist(level, tonttu->x, tonttu->y, s.c_str(), C_YELLOW,
					 "You hear a distant squeak...", C_YELLOW);
		}
		/* update level with opened door */
		level->Close_Door(c);
		return DOORSTAT_SUCCESS;
	}
	else
		msg.newmsg("Paska", C_RED);

	return DOORSTAT_NODOOR;
}

bool is_handle_stuck(Actor *tonttu, level_type *level, const Coord &c)
{
	int ustr=tonttu->stat[STAT_STR].Get() + tonttu->stat[STAT_TGH].Get();

	ustr=ustr/2;

	int sv=level->Get_Svalbard(c);
	const bool is_plr=tonttu->Is_Player();
	bool rv=true;

	if (sv > ustr)
	{
		sv-=ustr;
		if (is_plr)
		{
			msg.newmsg("The door seems to loose up a bit, but it's still stuck.",
				   C_WHITE);
		}
		else
		{
			string monsname=monster_sprintf(tonttu, true, true);
			msg.vnewdist(level, tonttu->x, tonttu->y, C_YELLOW, C_YELLOW,
					 "You hear distant banging.",
					 "%s seems to have some problems with a door.",
					 monsname.c_str());
		}
	}
	else
	{
		sv=0;
		if (is_plr)
		{
			msg.newmsg("With a forceful push you manage to get the door open!",
				   C_WHITE);
		}

		rv=false;
	}

	if (sv<0)
		sv=0;

	level->Set_Svalbard(c, sv);

	return rv;
}

bool disarm_trap(playerinfo *plr, level_type *level)
{
	const bool is_plr=true;
	int dir;
	int tid=TRAP_DISARMED;
	Coord tc;

	for(dir=1; dir<10; dir++)
	{
		tc=move_to_direction(dir, plr);
		Trap &t=level->Get_Trap(tc);
		tid=t.Get_Type();
		if(tid>TRAP_DISARMED)
			break;
	}

	if(dir>9)
	{
		msg.newmsg("You don't know any traps around here.",C_WHITE);
		return false;
	}

	if (tid<=TRAP_DISARMED || tid>TRAP_MAXNUM)
	{
		msg.newmsg("The trap needs no disarming.",C_WHITE);
		return false;
	}

	msg.vadd(C_WHITE,"You try to disarm the %s trap...", list_traps[tid].name);

//   val=skill_check(player.skills, SKILLGRP_GENERIC, SKILL_DISARMTRAP);
	//note: get screen location later
	//gotoxy(MAPWIN_RELX+player.spos.x+move_dx[dir],
	//	  MAPWIN_RELY+player.spos.y+move_dy[dir]);

	put_char('X', CHB_RED);

	refresh();
	delay(1000);

	if(player.Skill_Testsuccess(SKILLGRP_GENERIC, SKILL_DISARMTRAP))
	{
		msg.newmsg("Success!",CH_GREEN);
		level->Remove_Trap(tc);

		plr->skills.modify_raise(SKILLGRP_GENERIC,SKILL_DISARMTRAP, 1, 5, is_plr);
	}
	else
	{
		if (throwdice(1,100,0) < 50)
		{
			msg.newmsg("Oh no, the trap goes on!", CH_RED);
			/* if fails to disarm, launch the trap */
			handletrap(level, tc, plr);
		}
		else
			msg.newmsg("You fail, fortunately nothing bad happens!",C_RED);
	}
	return true;
}

void open_door(playerinfo *plr, level_type *level)
{
	int dir=dir_askdir("Open door -", true);
	if (dir>9) return;

	Coord nc=move_to_direction(dir, plr);

	if (plr->Is_At(nc))
	{
		msg.newmsg("You have lost the key...", C_WHITE);
	}
	else if (open_door(plr, level, nc)==DOORSTAT_SUCCESS)
	{
		plr->Spend_Time(TIME_OPENDOOR);
	}
}

int open_door(Actor *tonttu, level_type *level, const Coord &c)
{
	const bool is_plr=tonttu->Is_Player();

	if (level->Is_Door(c)==false)
	{
		if (is_plr)
			msg.newmsg("That isn't a door.", C_WHITE);

		return DOORSTAT_NODOOR;
	}

	if (level->Is_Closed_Door(c))
	{
		const int8u dflag=level->Get_Door_Flag(c);

		/* is the door stuck? */
		if (dflag & DOOR_STUCK)
		{
			if (is_plr)
			{
				if(!confirm_yn("The door is stuck, do you want to force it", false, true))
					return DOORSTAT_STUCK;
			}

			if (is_handle_stuck(tonttu, level, c))
				return DOORSTAT_STUCK;
		}
		/* is the door locked? */
		else if (dflag & DOOR_LOCKED)
		{
			if (is_plr)
			{
				msg.newmsg("The door is locked tight.", C_CYAN);
			}
			return DOORSTAT_LOCKED;
		}

		/* is the door trapped? */
		if (level->Has_Object(c, OBJECT_TRAP))
		{
			/* blow a huge bomb or something here */
			if (handletrap(level, c, tonttu))
			{
				return DOORSTAT_SUCCESS;
			}
			else
			{
				if (is_plr)
				{
					msg.newmsg("The door stands unharmed.", C_CYAN);
				}
				return DOORSTAT_STUCK;
			}
		}

		/**********************************************/
		/* it's a normal closed door with no features */
		/**********************************************/

		/* if action by player */
		if (is_plr)
		{
			texts->Chain_Message(Script::Door_Sound, C_YELLOW);
		}
		/* if action by monster */
		else
		{
			string s=monster_sprintf(tonttu, true, true);

			s.append("opened a door.");
			msg.add_dist(
				level, tonttu->x, tonttu->y, s.c_str(), C_YELLOW,
				"You hear a distant creak...", C_YELLOW);
		}

		level->Open_Door(c);

		return DOORSTAT_SUCCESS;
	}
	else if (level->Is_Open_Door(c))
	{
		if (is_plr)
			msg.newmsg("The door is already wide open.", C_WHITE);

		return DOORSTAT_ALREADY_OPEN;
	}

	return DOORSTAT_NODOOR;
}
