/**************************************************************************
 * stash.cpp --                                                           *
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

//Refactored 23.6.2025 - 12.4.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "gameview.h"
#include "caves.h"
#include "display.h"
#include "invnode.h"
#include "message.h"
#include "names.h"
#include "randgen.h"
#include "salamath.h"
#include "stash.h"
#include "target.h"

using std::string;

int Stash::Count_Items_Flat(int x, int y)
{
	int rv=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *n=(*ii);
		if (n->x==x && n->y==y) rv++;
	}

	return rv;
}

invnode *Stash::Find_Largest_Item(const Coord &c)
{
	invnode *heaviest=0;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *ptr=(*ii);
		if (ptr->x==c.x && ptr->y==c.y)
		{
			if (heaviest==0)
				heaviest=ptr;
			else
			{
				if (ptr->i.Get_Weight() > heaviest->i.Get_Weight())
					heaviest=ptr;
			}
		}
	}

	return heaviest;
}

invnode *Stash::Top_Item(const Coord &c)
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *ptr=(*ii);
		if (ptr->x==c.x && ptr->y==c.y)
			return ptr;
	}
	return 0;
}

void Stash::crush_items(int x, int y, invnode *nothis)
{
	bool breakone=false;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *crush=(*ii);

		if (nothis!=crush && crush->x==x && crush->y==y)
		{
			if (!breakone)
			{
				if (get_distance(player.Get_Location(),
					crush->Get_Location()) < player.sight)
						msg.add("You hear a nasty crushing sound.", C_WHITE);
			}
			breakone=true;
			crush->i.icond=COND_BROKEN;
		}
	}
}

invnode *Stash::Pick_Up(invnode *src)
{
	invnode *i=detach(src);

	if (i==0) return 0;

	Coord c=i->Get_Location();
	gameview.Refresh_Item_Map(c);

	return i;
}

void Stash::Refresh_Item_Map()
{
	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
		gameview.Put_Item(*ii);
}

void Stash::teleport_items(level_type *level, Target *target)
{
	int tx, ty;

	for (oitr ii = items.begin() ; ii != items.end() ; ++ii)
	{
		invnode *iptr=(*ii);
		Coord ic=iptr->Get_Location();

		if (ic == target->pos)
		{
			set_randomcoord(level, &tx, &ty);
			string iname=item_name(iptr);

			string s=iname;
			s.append(" disappears.");

			msg.add_dist(level, iptr->x, iptr->y, s.c_str(), C_CYAN,
				"You hear a distant \"swoosh\"-sound.", C_CYAN);

			iptr->x=tx;
			iptr->y=ty;

			string q=iname;
			q.append(" appears in your sight.");

			msg.add_dist(level, iptr->x, iptr->y, q.c_str(), C_CYAN,
				"You hear a distant \"Zap!\".", C_CYAN);

			//gameview.Show(); //note: test if needed
		}
	}
}
