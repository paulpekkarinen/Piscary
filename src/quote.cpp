/**************************************************************************
 * quote.cpp --                                                           *
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

//Refactored 3.3.2023 - 26.3.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "currency.h"
#include "dice.h"
#include "lexicon.h"
#include "message.h"
#include "names.h"
#include "quote.h"
#include "script.h"

using std::string;

std::string do_bay_bill_to(being *mptr, const Currency &c)
{
	string s("Do you want to pay your bill (");
	s+=get_coppervalue_string(c);
	s.append(" to ");
	s.append(mptr->m.name);
	return s;
}

string get_coppervalue_string(const Currency &c)
{
	string s=to_string(c.gold);
	s.append("g, ");
	s.append(to_string(c.silver));
	s.append("s, ");
	s.append(to_string(c.copper));
	s.append("c");

	return s;
}

std::string get_ordinal_of_time(int n)
{
	if (n==0)
		return "st";
	else if (n==1)
		return "nd";
	else if (n==2)
		return "rd";

	return "th";
}

void keeper_farewell(level_type *level, being *owner, being *creat)
{
	if (creat==owner)
		return;

	const char *ownername=owner->m.name.c_str();

	if (creat->Is_Player()==false)
	{
		if (creat->bill>0)
		{
			if (owner->m.status & MST_GUARDDOOR)
				owner->m.status^=MST_GUARDDOOR;

			creat->m.status |= MST_KEEPERHATES;

			string s=someone_screams(owner->Get_Name(),
				"THIEEEEEEEF! WHERE'S THE SHERIFF!?!");

			msg.add_dist(level, owner->x, owner->y,
				s.c_str(), C_GREEN, "You hear someone calling for sheriff!", C_WHITE);
			creat->bill=0;
			creat->inv.mark_unpaid();

			return;
		}

		if (!(creat->m.status & MST_KEEPERHATES))
		{
			string s=someone_says(owner->Get_Name(), "Come again soon, stranger!");
			msg.add_dist(level, owner->x, owner->y, s.c_str(), C_GREEN,
				"You hear from distance: \"Come again soon, stranger!\".", C_WHITE);
		}
	}
	else
	{
		/* if bill>0 then it's a successfull steal */
		if (player.bill>0)
		{
			/* mark owner to hate player */
			owner->m.status |= MST_HATEPLAYER;

			if (owner->m.status & MST_GUARDDOOR)
				owner->m.status^=MST_GUARDDOOR;

			// clear the bill, succesfull steal
			player.bill=0;

				/* handle all player items and remove unpaid mark */
			player.inv.mark_unpaid();

			msg.newmsg(CH_RED,
				"%s screams: \"THIEEEEEEEF! WHERE'S THE SHERIFF!?!\".",
				owner->Get_Name());
			return;
		}

		/* friendly bye-msg if doesn't hate player */
		if (!(owner->m.status & MST_HATEPLAYER))
		{
			msg.newmsg(C_WHITE, "%s says: \"Come again soon, %s!\".",
				ownername, player.Get_Name());
		}
		else
		{
			//if keeper is in attack mode yell an unfriendly phrase when
			//target leaves shop
			const char *throw_mess=texts->Get_Random(Script::Keeper_Throw);

			string distant("Someone ");
			distant.append(throw_mess);

			string seen(owner->m.name);
			seen.append(" ");
			seen.append(throw_mess);

			msg.add_dist(level, owner->x, owner->y, seen.c_str(), CH_MAGENTA,
				distant.c_str(), C_MAGENTA);
		}
		//	 owner->m.status^=MST_ATTACKMODE;
	}
}

void keeper_greet(level_type *level, being *owner, being *creat)
{
	if (creat==owner)
		return;

	const int ronum=owner->roomnum;
	const char *shopname=level->rooms[ronum].kauppa.Get_Name();
	const char *ownername=owner->m.name.c_str();

	if (creat->Is_Player()==false)
	{
		string moname=monster_sprintf(creat, false, true);
		string s(owner->m.name);

		if (creat->m.status & MST_KEEPERHATES)
		{
			s.append(" yells to ");
			s.append(moname);
			s.append(" :\"Get out of here...NOW!\"");

			msg.add_dist(level, owner->x, owner->y, s.c_str(), CH_MAGENTA,
				"You hear from distance: \"Get out of here...NOW!\".",
				C_MAGENTA);
			owner->m.status|=MST_ATTACKMODE;
			owner->target.Set(creat);
		}
		else
		{
			s.append(" welcomes ");
			s.append(moname);

			string welcome(" :\"Welcome to ");
			welcome.append(owner->m.name);
			welcome.append("'s ");
			welcome.append(shopname);
			welcome.append(".\"");

			s.append(welcome);

			string dist("You hear from distance");
			dist.append(welcome);

			msg.add_dist(level, owner->x, owner->y,
				s.c_str(), C_WHITE, dist.c_str(), C_WHITE);
		}
	}
	else
	{
		if (owner->m.status & MST_HATEPLAYER)
		{
			owner->m.status|=MST_ATTACKMODE;
			owner->target.Clear();
			msg.newmsg(CH_MAGENTA,
				"%s yells: \"Get OUT of here, thief!\".",
				ownername);
			return;
		}
		else
		{
			bool visited;
			if (!(owner->m.status & MST_KNOWN))
			{
				// make shopkeeper known!
				owner->m.status|=MST_KNOWN;
				visited=false;
			}
			else
			{
				// if hates player, attack !
				visited=true;
			}
			string s=shopkeeper_greeting(owner->Get_Name(), shopname, visited);
			msg.newmsg(s, CH_GREEN);
		}
	}
}

string noun_verbs_something(string &n, const char *verb, string &smt)
{
	string s(n);
	s.append(" ");
	s.append(verb);
	s.append(" ");
	s.append(smt);
	s.append("!");
	return s;
}

string shopkeeper_greeting(const char *owner, const char *shopname, bool visited)
{
	string s;
	if (visited)
	{
		s.append("Hello again, ");
		s.append(player.Get_Name());
		s.append(", w");
	}
	else s.append("W");

	s.append("elcome to ");
	s.append(owner);
	s.append("'s ");
	s.append(shopname);

	if (visited==false) s.append(", stranger");
	s.append("!");

	return s;
}

string someone_says(const char *actor, const char *what)
{
	string s(actor);
	s.append(" says: \"");
	s.append(what);
	s.append("\"");
	return s;
}

string someone_screams(const char *actor, const char *what)
{
	string s(actor);
	s.append(" screams: \"");
	s.append(what);
	s.append("\"");
	return s;
}
