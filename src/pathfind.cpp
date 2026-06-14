/**************************************************************************
 * pathfind.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 02.08.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 02.08.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 * All rights reserved.                                                   *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************
 * eventually this will handle pathfinding for monsters                   *
 **************************************************************************/

//Refactored 19.9.2021 - 20.9.2025 by Paul K. Pekkarinen

#include "avatar.h"
#include "bytegrid.h"
#include "caves.h"
#include "gameview.h"
#include "input.h"
#include "message.h"
#include "output.h"
#include "pathfind.h"
#include "storage.h"
#include "way.h"

/*
 * directions and their reverse directions
 * these tables are used by the pathfinder
 */
//const int dir_normal[]= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
const int dir_invers[]= { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

/************************************************/
/* basic routines for handling path linked list */
/************************************************/

bool Pathlist::has_path()
{
	if (coords.empty()) return false;
	return true;
}

/*
 * Insert item in the list
 * most recent item takes the first place in the list
 *
 */
void Pathlist::insert(Coordist *data)
{
	coords.push_front(Coordist(data));
}

/*
 * Same as above but inserts to the list end
 */
void Pathlist::insert_end(Coordist *data)
{
	coords.push_back(Coordist(data));
}

bool Pathlist::movealong(int *tx, int *ty)
{
	Coordist *pos = get_first();

	/* check for existing path */
	if (pos == 0)
		return false;

	(*tx)=pos->x;
	(*ty)=pos->y;

	//   msg.vnewmsg(C_GREEN, "moved to %d,%d!", *tx, *ty);

	/* remove the first pathnode */
	remove_first();
	return true;
}

void Pathlist::remove_first()
{
	coords.pop_front();
}

Coordist *Pathlist::get_first()
{
	if (coords.empty()) return 0;
	return &coords.front();
}

/*
 * Get the best item from the list. Item will also be REMOVED from the
 * list. This routine is used when the path is under construction
 */
Coordist Pathlist::getbest(const Coordist &end)
{
	if (coords.empty())
		return Coordist(-1, -1);

	int edist = 9999;
	std::list<Coordist>::iterator tbr = coords.begin();

	for (std::list<Coordist>::iterator ii = coords.begin() ; ii != coords.end() ; ++ii)
	{
		int estim = (*ii).dist + (*ii).Distance(end);
		if (estim < edist)
		{
			edist = estim;
			tbr = ii;
		}
	}

	Coordist c(*tbr);

	coords.erase(tbr);

	return c;
}

void Pathlist::clear()
{
	coords.clear();
}

void Pathlist::Show_Debug_Info(char ch, int len)
{
	msg.vnewmsg(CH_GREEN, "Found path len=%d!", len);

	for (std::list<Coordist>::iterator ii = coords.begin() ; ii != coords.end() ; ++ii)
	{
		Coord c=gameview.Get_Screen_Location(*ii);
		put_char_to(ch, c);
	}

	wait_key();
}

/**********************************************************
 * Let's get into the business, here's the actual routine *
 * for pathfinding                                        *
 **********************************************************
 * This routine will try to find a way from (x1,y1) to    *
 * (x2,y2). The result will be a linked list of path nodes*
 * or NULL if no path can be found.                       *
 *                                                        *
 * After you receive a pointer to the first node of a path*
 * list you just need to get the first item of the list to*
 * advance along the existing path, after getting the node*
 * remove it from this list or your monster will stand    *
 * still. :-)                                             *
 **********************************************************/
void Pathlist::find_route(level_type *level, int x1, int y1, int x2, int y2)
{
	/* check level boundaries first, 1 block border in the edges */
	if (level->Is_Outside_Border(x1, y1) || level->Is_Outside_Border(x2, y2))
	{
		msg.newmsg("Pathfinder error, out of boundaries!", CHB_RED);
		return;
	}

	/* list for pathfinder to hold nodes to be examined */
	Pathlist checklist;

	Coord c(0, 0);
	Coordist end(x2, y2), now(x1, y1), next;

	/* result indicator */
	bool pathexists = false;

	/* pathfinder needs a dirtable which is exactly the same size as
	 *  the level map
	 */

	/* clear the memory, needs to be 0 */
	gameview.dirgrid->Clear(0);

	/* init dir grid */
	gameview.dirgrid->Set(x1, y1, 5);

	/* insert the starting position to the checklist */
	checklist.insert(&now);

	int search_limit=50000; //try this many times, then give up

	while (checklist.has_path())
	{
		now=checklist.getbest(end);

		for (int i=1; i<10; i++)
		{
			if(i==DIR_SELF)
				continue;

			next = now;

			/* diagonal movements should be lengthier */
	   //	 if(i==1 || i==3 || i==7 || i==9)
	   //	    next.d = now.d + 2;
	   //	 else
			next.dist = now.dist+1;

			c=now;
			c.Move_Direction(i);

			if (level->Is_Outside_Border(c.x, c.y))
				continue;

			if (level->Is_Passable(c) && gameview.dirgrid->Get(c.x, c.y) == 0)
			{
				gameview.dirgrid->Set(c.x, c.y, dir_invers[i]);
				next.Move_Direction(i);
				checklist.insert(&next);
			}
		}

		if (now.x == end.x && now.y == end.y)
		{
			pathexists=true;
			break;
		}

		//safety mechanism to avoid endless search loop
		search_limit--;
		if (search_limit<0) break;
	}

	/* clear temporary checklist, no need anymore */
	checklist.clear();

	int len=0;

	if (pathexists)
	{
		now.dist = 0;
		/* build the ACTUAL pathlist */
		while(!(now.x == x1 && now.y == y1))
		{
			insert(&now);
			len++;
			c = now;
			const int8u d=gameview.dirgrid->Get(c.x, c.y);
			if (d!=Bytegrid::Outside_Value)
				now.Move_Direction(d);
		}

		Show_Debug_Info('$', len);
	}
}

void Pathlist::Save(Tar_Ball &tb)
{
	//save size of the list and then Coordists in the list
	const int sz=(int)coords.size();
	tb.Put(sz);

	for (std::list<Coordist>::iterator ii = coords.begin() ; ii != coords.end() ; ++ii)
	{
		(*ii).Save(tb);
	}
}

void Pathlist::Load(Tar_Ball &tb)
{
	//load the size of list and push saved Coordists in it
	const int sz=tb.Get_Next_Value();

	for (int t=0; t<sz; t++)
	{
		Coordist c;
		c.Load(tb);
		coords.push_back(c);
	}
}
