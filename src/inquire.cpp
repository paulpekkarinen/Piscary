/**************************************************************************
 * inquire.cpp --                                                         *
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

//Refactored 14.7.2022 - 19.10.2025 by Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "inquire.h"
#include "invnode.h"
#include "lexicon.h"
#include "magic.h"
#include "message.h"
#include "names.h"
#include "move.h"
#include "selpack.h"
#include "scrolls.h"
#include "terrain.h"

using std::string;

int search_location(level_type *level, const Coord &c);
bool scroll_readit(playerinfo *plr, level_type *level, item_def *item);
bool reveal_secret_door(level_type *level, const Coord &c);

void lookdir(playerinfo *plr, level_type *level)
{
	const int dir=dir_askdir("Look at", true);
	if (dir>9) return;

	Coord nc=move_to_direction(dir, plr);

	if (plr->Is_At(nc))
		msg.newmsg("What an ugly adventurer here!", C_WHITE);
	else
	{
		being *mptr=gameview.Get_Monster(nc);
		if (mptr)
		{
			display->Monster_Description(mptr);
			GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
		}
		else
			msg.newmsg("Nothing there!", C_WHITE);
	}
}

void read_scroll(playerinfo *plr, level_type *level)
{
	plr->backpack->Set_Filter(IS_SCROLL);
	invnode *scroll=plr->backpack->Get_Handle("Read which scroll");

	Game.noticeevents(level);

	bool sres=false;

	if (scroll)
	{
		if (scroll->i.type != IS_SCROLL)
		{
			msg.newmsg("It's not a scroll.", C_WHITE);
			return;
		}

		if (!(scroll->i.status & ITEM_UNPAID))
			sres = scroll_readit(plr, level, &scroll->i);
		else
			msg.newmsg("You can't quite make sense of it, the writing "
				"is oddly blurred here. ", C_MAGENTA);
	}
	else
	{
		msg.newmsg("Nothing read.", C_WHITE);
		return;
	}

	if (sres)
	{
		Game.noticeevents(level);
		const int sc_group=scroll->i.group;

		/* relabel unknown scroll */
		if (!(scroll->i.status & ITEM_IDENTIFIED))
		{
			if (!(list_scroll[sc_group].flags & SCFLAG_AUTOIDENTIFY) &&
				!(list_scroll[sc_group].flags & SCFLAG_NAMED))
			{
				list_scroll[sc_group].flags |= SCFLAG_TRIED;

				msg.add("What do you want to call it?", C_WHITE);
				string newname=get_string(false, ITEM_NAMEMAX);

				if (newname.size()>0)
				{
					if (is_same_string(newname, list_scroll[sc_group].name))
					{
						scroll->i.status |= ITEM_IDENTIFIED;
						list_scroll[sc_group].flags |= SCFLAG_IDENTIFIED;

						int expgain = RANDU(200) + RANDU(200);

						msg.vnewmsg(C_YELLOW,
							"You get %ld experience for correctly "
							"identifying the scroll of %s!",
							expgain, list_scroll[sc_group].name);

						plr->Gain_Experience(expgain);
					}
					else
					{
						list_scroll[sc_group].flags |= SCFLAG_NAMED;

						msg.vnewmsg(C_WHITE,
							"You just relabeled a scroll called \"%s\" "
							"to \"%s\"",
							list_scroll[sc_group].uname, newname.c_str());

						my_strcpy(list_scroll[sc_group].cname, newname.c_str(),
							ITEM_NAMEMAX);
					}
				}
				else
				{
					msg.newmsg("Cancelled!", C_RED);
				}

			}
		}

		if (sc_group != SCROLL_BLANK)
			plr->inv.Destroy_Item(plr->equips, scroll, 1);
	}
}

int search_location(level_type *level, const Coord &c)
{
	if (level->Is_Secretdoor(c))
		return FOUND_SECRETDOOR;

	if (level->Has_Object(c, OBJECT_TRAP))
	{
		if (level->Is_Door(c))
			return FOUND_DOORTRAP;
		else
			return FOUND_CAVETRAP;
	}

	return FOUND_NOTHING;
}

bool search_surroundings(Actor *tonttu, level_type *level, bool automatic)
{
	int skill=tonttu->skills.check(SKILLGRP_GENERIC, SKILL_SEARCHING);

	/* if automatic searching, no success if no skill
	 * if manual searching, allow some sort of a success
	 */
	if (!skill && automatic)
		return false;

	if (!skill)
		skill=5;

	string moname;
	Coord c=tonttu->Get_Location();
	const bool plr=tonttu->Is_Player();

	if (plr==false)
	{
		if (gameview.Is_Visible(c)==false)
			return false;

		tonttu->Spend_Time(TIME_SEARCH);

		moname=monster_sprintf(tonttu, true, true);
		if (RANDU(100)<20)
		{
			string s=moname;
			s.append(" seems to be searching for something...");
			msg.vnewdist(level, c.x, c.y,
				C_WHITE, C_WHITE,
				NULL,
				s.c_str());
		}
	}
	else
	{
		if (!automatic)
		{
			msg.newmsg("You search your surroundings...", C_WHITE);
			tonttu->Spend_Time(TIME_SEARCH);
		}
		else
		{
			if (RANDU(100) < 20)
				msg.newmsg("You're searching!", C_WHITE);
		}
	}

	int sres=0;
	Coord dest;
	for (int i=1; i<10; i++)
	{
		dest=move_to_direction(i, c);
		sres=search_location(level, dest);
		if (sres!=FOUND_NOTHING)
			break;
	}

	if (RANDU(100)>skill)
		sres=FOUND_NOTHING;

	if (automatic)
	{
		if (sres!=FOUND_NOTHING && plr)
		{
			msg.addwait("You notice that there's something hidden around here.",
				CH_WHITE);
		}
		return true;
	}

	switch (sres)
	{
		case FOUND_SECRETDOOR:
			if (plr==false)
			{
				string s=moname;
				s.append(" found a secret door.");
				msg.vnewdist(level, c.x, c.y,
					C_WHITE, C_WHITE,
					NULL,
					s.c_str());
			}
			else
				msg.addwait("a secret door!", C_WHITE);

			reveal_secret_door(level, dest);
			break;
		case FOUND_DOORTRAP:
			if (plr==false)
			{
				string s=moname;
				s.append(" found a trapped door.");
				msg.vnewdist(level, c.x, c.y,
					C_WHITE, C_WHITE,
					NULL,
					s.c_str());
			}
			else
				msg.addwait("this door is trapped!", C_WHITE);

			level->Reveal_Trap(dest);
			break;
		case FOUND_CAVETRAP:
			if (plr==false)
			{
				string s=moname;
				s.append(" found a trap.");
				msg.vnewdist(level, c.x, c.y,
					C_WHITE, C_WHITE,
					NULL,
					s.c_str());
			}
			else
				msg.addwait("a TRAP!", C_WHITE);

			level->Reveal_Trap(dest);
			break;

		default:
			if (plr)
				msg.newmsg("nothing found.", C_WHITE);
			return false;
			break;
	}

	tonttu->skills.modify_raise(SKILLGRP_GENERIC, SKILL_SEARCHING, 1, 5);

	return true;
}

bool scroll_readit(playerinfo *plr, level_type *level, item_def *item)
{
	//   item_info *iptr;

	if (item->type!=IS_SCROLL)
	{
		msg.newmsg("This item contains no writing.", C_WHITE);
		return false;
	}

	if (item->group != SCROLL_BLANK)
		msg.newmsg("The scroll disappears in a huge puff of smoke.",
			C_WHITE);
	/*
	 * Identify autoidentify scrolls
	 * but self written scrolls need not to be identified
	 *
	 * There's no special flag for self written scroll,
	 * just make sure that self written scrolls are identified
	 * when they are created!
	 *
	 */
	 //   if( !(item->status & ITEM_SELFSCROLL) ) {

	if (!(item->status & ITEM_IDENTIFIED))
	{
		if (list_scroll[item->group].flags & SCFLAG_AUTOIDENTIFY)
		{

			item->status |= ITEM_IDENTIFIED;
			list_scroll[item->group].flags |= SCFLAG_IDENTIFIED;

			/* now identify all similar items in the current level */
	   //	 iptr=level->items;
	   //	 while(iptr) {
	   //	    if(iptr->i.type == IS_SCROLL &&
	   //	       iptr->i.group == item->group)
	   //	       iptr->i.status |= ITEM_IDENTIFIED;
	   //
	   //	    iptr=iptr->next;
	   //
	   //	 }

			msg.vadd(C_YELLOW, "This is a scroll of %s.",
				list_scroll[item->group].name);
			showmore(true, true);
		}
	}

	/* for magical scrolls */
	if (item->pmod1==SCROLLGROUP_MAGIC)
	{
		spell_zap(plr, level, item->pmod2, item->pmod3, true);
	}
	else
	{
		/* for non magical scrolls */
		switch (item->group)
		{
			case SCROLL_BLANK:
				scroll_blank();
				break;
			case SCROLL_RUMOUR:
				scroll_rumour();
				break;
			case SCROLL_AMNESIA:
				scroll_amnesia(level);
				break;

			default:
				msg.newmsg("Woo-opsis! Scroll is not yet implemented!", CH_RED);
				break;
		}
	}

	return true;
}

bool reveal_secret_door(level_type *level, const Coord &c)
{
	/* is it a secret door */
	if (level->Get_Terrain(c) != TYPE_DOORSECR)
		return false;

	/* yes make a normal door in there */

	level->Set_Terrain(c.x, c.y, TYPE_DOORCLOS);

	return true;
}
