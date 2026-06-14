/**************************************************************************
 * crew.cpp --                                                            *
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

//Refactored 12.12.2021 - 12.4.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "crew.h"
#include "dice.h"
#include "display.h"
#include "gameview.h"
#include "input.h"
#include "message.h"
#include "options.h"
#include "output.h"
#include "pack.h"
#include "quote.h"
#include "roleplay.h"
#include "salamath.h"
#include "storage.h"
#include "trapdash.h"
#include "uncover.h"

void Crew::Add_Monster(being *b)
{
	monsters.push_back(b);
}

void Crew::Cleantargets(being *remove)
{
	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		being *mptr=(*i);

		if (mptr->target.olento==remove)
		{
			//if monster is targetting the removed monster, clean
			mptr->target.Clear();

			if (mptr->m.status & MST_ATTACKMODE)
				mptr->m.status^=MST_ATTACKMODE;
			if (mptr->m.status & MST_FLEEMODE)
				mptr->m.status^=MST_FLEEMODE;
		}
	}
}

being *Crew::Find_By_Index(int index)
{
	int n=0;

	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		if (n==index)
			return (*i);
		n++;
	}

	return 0; //monster not in this list
}

being *Crew::Find_Monster_At(const Coord &c)
{
	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		being *b=(*i);
		if (b->x == c.x && b->y == c.y)
			return b;
	}

	return 0;
}

int Crew::Get_Index(being *b)
{
	int index=0;

	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		if ((*i)==b)
			return index;
		index++;
	}

	return -1; //monster not in this list
}

void Crew::Refresh_Creature_Map(Gameview *gv)
{
	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		being *b=(*i);
		Coord c=b->Get_Location();
		gv->Put_Monster(b, c);
	}
}

void Crew::handle_monsters(level_type *lvl, int slots)
{
	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		being *ptr=(*i);

		/* dead monsters don't do much */
		if (ptr->Is_Alive())
		{
			int tslots=slots;
			while(tslots)
			{
				ptr->Checkturn(lvl);
				tslots--;
			}

			const Coord monpos=ptr->Get_Location();

			//note: player's speed is used, could be a bug?
			ptr->Regenerate(lvl, player.stat[STAT_SPD].Get(), slots);
			ptr->Handle_Conditions(slots);

			if(ptr->light>0)
				gameview.Monster_Torchlos(ptr);

			handletrap(lvl, monpos, ptr);

			/* check monster status, hp and stuff and die if necessary */
			ptr->Checkstat(lvl);
		}
	}
}

/* Remove all monsters contained in the level 'level' and free memory */
void Crew::removeall()
{
	for (mon_iter i=monsters.begin(); i!=monsters.end(); ++i)
	{
		delete (*i);
	}

	monsters.clear();
}

void Crew::Remove_Dead(level_type *level)
{
	if (clean_up==false)
		return;

	bool found=true;

	while (found)
	{
		found=false;
		mon_iter i=monsters.begin();

		while (i!=monsters.end())
		{
			being *b=(*i);
			if (b->Is_Alive()==false)
			{
				Cleantargets(b); //if someone is targetting this one
				Coord c=b->Get_Location();
				gameview.Put_Monster(0, c);
				drop_loot(b, level);
				delete b;

				i=monsters.erase(i);
				found=true;
				break;
			}
			else ++i;
		}
	}

	clean_up=false;
}

int Crew::Target_Nearest(level_type *level, Coord &c, int lastidx)
{
	Coord pc=player.Get_Location();
	int shortest_dist = 10000;
	Coord dest;
	int i=0;
	int rv=lastidx;

	for (mon_iter ii=monsters.begin(); ii!=monsters.end(); ++ii)
	{
		Coord mc=(*ii)->Get_Location();

		if (gameview.Is_Visible(mc) && gameview.Is_Outside_View(mc)==false
			&& i>lastidx)
		{
			const int d=get_distance(pc, mc);
			if (d<shortest_dist)
			{
				shortest_dist=d;
				dest=mc;
				rv=i;
			}
		}

		i++;
	}

	if (shortest_dist<10000)
		c=dest;

	return rv;
}

void Crew::Debug_List()
{
	const int amt=(int)monsters.size();

	if (amt==0)
		my_printf("No monsters in this level.\n");
	else
	{
		my_printf("%d monsters in this level.\n", amt);

		//search for special monsters
		for (mon_iter ii = monsters.begin() ; ii != monsters.end() ; ++ii)
		{
			being *b=(*ii);
			const int sp=b->Get_Special_Id();
			if (sp>0)
			{
				my_printf("'%s' (sp id: %d) at %d, %d.\n", b->Get_Name(), sp, b->x, b->y);
			}
		}
	}
}

void Crew::Full_Debug_List()
{
	clear_screen();
	int y=1;

	for (mon_iter ii = monsters.begin() ; ii != monsters.end() ; ++ii)
	{
		being *b=(*ii);

		my_printf("%u: '%s' ", b->id, b->Get_Name());
		uncover.Monsterdef_Info(b->m.race);

		standend();

		if (b->roomnum!=-1)
			my_printf(" Shopkeeper");

		my_printf(" (%d, %d) ", b->x, b->y);
		display->Attribute_As("HP", b->health);
		b->target.Show_Data();

		if (list_more(y)==false)
			break;
	}
}

void Crew::Unalive_Monsters()
{
	int a=0;
	for (mon_iter ii = monsters.begin() ; ii != monsters.end() ; ++ii)
	{
		(*ii)->Death();
		a++;
	}

	if (a>0)
	{
		Sentenced();
		msg.newmsg("You feel a disturbance in the force!");
	}
	else
		msg.newmsg("Nothing seems to happen.");
}

void Crew::save(Tar_Ball &tb)
{
	const int amt=(int)monsters.size();
	tb.Put(amt);

	for (mon_iter ii = monsters.begin() ; ii != monsters.end() ; ++ii)
	{
		(*ii)->Save(tb);
	}
}

void Crew::load(Tar_Ball &tb, level_type *lvl)
{
	const int amt=tb.Get_Next_Value();

	for (int t=0; t<amt; t++)
	{
		being *b=new being;
		b->Load(tb, lvl);
		monsters.push_back(b);
	}
}
