/**************************************************************************
 * move.cpp --                                                            *
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

//Refactored 18.9.2021 - 7.4.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "condit.h"
#include "dice.h"
#include "display.h"
#include "foodstat.h"
#include "game.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "inquire.h"
#include "invnode.h"
#include "melee.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "pack.h"
#include "quote.h"
#include "salamath.h"
#include "script.h"
#include "spot.h"
#include "terrain.h"
#include "use.h"
#include "way.h"
#include "world.h"

using std::string;

int sur_countpass(level_type *level, int x, int y);

/*
 * table for keypad movements. They are x and y delta values.
 * this table is also used for monster movement (random)
 * (actually these are used in *many* places )
 */

const int move_dx[] = { 0, -1, 0, 1, -1, 0, 1, -1,  0,  1, 0, 0, 0 };
const int move_dy[] = { 0,  1, 1, 1,  0, 0, 0, -1, -1, -1, 0, 0, 0 };

const int move_dirpriority_flee[8][8]=
{
	{ 7, 8, 4, 9, 1, 3, 6, 2 },	/* flee direction priority */
	{ 3, 6, 2, 9, 1, 7, 8, 4 },
	{ 9, 8, 6, 7, 3, 1, 4, 2 },
	{ 1, 4, 2, 7, 3, 9, 8, 6 },
	{ 1, 3, 2, 4, 6, 8, 7, 8 },
	{ 7, 9, 8, 4, 6, 2, 1, 3 },
	{ 9, 3, 6, 8, 2, 4, 7, 1 },
	{ 7, 1, 4, 8, 2, 6, 9, 3 },
/*
	{ 7, 8, 4, 9, 1, 6, 2, 3 },
   { 3, 6, 2, 9, 1, 8, 4, 7 },
   { 9, 8, 6, 7, 3, 4, 2, 1 },
   { 1, 4, 2, 7, 3, 8, 6, 9 },
   { 1, 3, 2, 4, 6, 7, 8, 9 },
   { 7, 9, 8, 4, 6, 1, 3, 2 },
   { 9, 3, 6, 8, 2, 7, 1, 4 },
   { 7, 1, 4, 8, 2, 9, 3, 6 },
*/
};

const int move_dirpriority_attack[8][9]=
{
	{ 3, 6, 2, 9, 1, 8, 4, 7 },
	{ 7, 8, 4, 9, 1, 6, 2, 3 },
	{ 1, 4, 2, 7, 3, 8, 6, 9 },
	{ 9, 8, 6, 7, 3, 4, 2, 1 },

	{ 8, 7, 9, 4, 6, 1, 3, 2 },
	{ 2, 1, 3, 4, 6, 7, 8, 9 },
	{ 4, 1, 7, 8, 2, 9, 3, 6 },
	{ 6, 3, 9, 8, 2, 7, 1, 4 },
};

const int move_dirpriority_circulate[8][9]=
{
	{ 3, 6, 2, 9, 1, 8, 4, 7 },
	{ 7, 8, 4, 9, 1, 6, 2, 3 },	/* flee direction priority */
	{ 1, 4, 2, 7, 3, 8, 6, 9 },
	{ 9, 8, 6, 7, 3, 4, 2, 1 },

	{ 8, 7, 9, 4, 6, 1, 3, 2 },
	{ 2, 1, 3, 4, 6, 7, 8, 9 },
	{ 4, 1, 7, 8, 2, 9, 3, 6 },
	{ 6, 3, 9, 8, 2, 7, 1, 4 },
/*
   { 2, 1, 3, 4, 6, 7, 8, 9 },
   { 8, 7, 9, 4, 6, 1, 3, 2 },
   { 4, 1, 7, 8, 2, 9, 3, 6 },
   { 6, 3, 9, 8, 2, 7, 1, 4 },

   { 9, 8, 6, 7, 3, 4, 2, 1 },
   { 1, 4, 2, 7, 3, 8, 6, 9 },
	{ 7, 8, 4, 9, 1, 6, 2, 3 },
   { 3, 6, 2, 9, 1, 8, 4, 7 },
*/
};

/* defines how far the shopkeeper will chase its targets
 * (a distance to the shop door!
 */
constexpr int SHOPKEEPER_MAXDOORDIST=15;

int monster_moveAROUNDtarget(level_type *level, int *mx, int *my, int tx, int ty)
{
	int dir1=0;

	const int *prioptr;

	prioptr=move_dirpriority_circulate[0];

	if (tx>*mx && ty>*my)
	{
		prioptr=move_dirpriority_circulate[0];
	}
	if (tx<*mx && ty<*my)
	{
		prioptr=move_dirpriority_circulate[1];
	}
	if (tx<*mx && ty>*my)
	{
		prioptr=move_dirpriority_circulate[2];
	}
	if (tx>*mx && ty<*my)
	{
		prioptr=move_dirpriority_circulate[3];
	}
	if (tx==*mx && ty<*my)
	{
		prioptr=move_dirpriority_circulate[4];
	}
	if (tx==*mx && ty>*my)
	{
		prioptr=move_dirpriority_circulate[5];
	}
	if (tx<*mx && ty==*my)
	{
		prioptr=move_dirpriority_circulate[6];
	}
	if (tx>*mx && ty==*my)
	{
		prioptr=move_dirpriority_circulate[7];
	}

	// priodir array now has all directions in priority order
	// all we now need to do is to move to FIRST possible direction
	// in that table, easy...
	for (int i=0; i<8; i++)
	{
		dir1=prioptr[i];
		Coord c(*mx+move_dx[dir1], *my+move_dy[dir1]);

		if (gameview.Get_Actor(c)!=0)
		{
			dir1=5;
			continue;
		}

		if (level->Free_To_Walk(c))
		{
			*mx=c.x;
			*my=c.y;
			break;
		}

	}
	return dir1;
}

int monster_moveFROMtarget(level_type *level, int *mx, int *my, int tx, int ty)
{
	int dir1=0;

	const int *prioptr;

	prioptr=move_dirpriority_flee[0];

	if (tx>*mx && ty>*my)
	{
		prioptr=move_dirpriority_flee[0];
	}
	if (tx<*mx && ty<*my)
	{
		prioptr=move_dirpriority_flee[1];
	}
	if (tx<*mx && ty>*my)
	{
		prioptr=move_dirpriority_flee[2];
	}
	if (tx>*mx && ty<*my)
	{
		prioptr=move_dirpriority_flee[3];
	}
	if (tx==*mx && ty<*my)
	{
		prioptr=move_dirpriority_flee[4];
	}
	if (tx==*mx && ty>*my)
	{
		prioptr=move_dirpriority_flee[5];
	}
	if (tx<*mx && ty==*my)
	{
		prioptr=move_dirpriority_flee[6];
	}
	if (tx>*mx && ty==*my)
	{
		prioptr=move_dirpriority_flee[7];
	}

	// priodir array now has all directions in priority order
	// all we now need to do is to move to FIRST possible direction
	// in that table, easy...
	for (int i=0; i<8; i++)
	{
		dir1=prioptr[i];

		Coord c;

		c.y=*my+move_dy[dir1];
		c.x=*mx+move_dx[dir1];

		if (level->Free_To_Walk(c))
		{
			*mx+=move_dx[dir1];
			*my+=move_dy[dir1];
			break;
		}
	}

	return dir1;
}

int monster_moveTOtarget(level_type *level, int *mx, int *my, int tx, int ty, bool getpast)
{
	int dir1=0;
	being *mptr;
	const int *prioptr=move_dirpriority_attack[0];

	if (tx>*mx && ty>*my)
	{
		prioptr=move_dirpriority_attack[0];
	}
	if (tx<*mx && ty<*my)
	{
		prioptr=move_dirpriority_attack[1];
	}
	if (tx<*mx && ty>*my)
	{
		prioptr=move_dirpriority_attack[2];
	}
	if (tx>*mx && ty<*my)
	{
		prioptr=move_dirpriority_attack[3];
	}
	if (tx==*mx && ty<*my)
	{
		prioptr=move_dirpriority_attack[4];
	}
	if (tx==*mx && ty>*my)
	{
		prioptr=move_dirpriority_attack[5];
	}
	if (tx<*mx && ty==*my)
	{
		prioptr=move_dirpriority_attack[6];
	}
	if (tx>*mx && ty==*my)
	{
		prioptr=move_dirpriority_attack[7];
	}

	// priodir array now has all directions in priority order
	// all we now need to do is to move to FIRST possible direction
	// in that table, easy...
	for (int i=0; i<8; i++)
	{
		dir1=prioptr[i];

		Coord c(*mx+move_dx[dir1], *my+move_dy[dir1]);

		if (getpast)
		{
			mptr=gameview.Get_Monster(c);
			if (mptr!=0)
			{
				dir1=5;
				continue;
			}
		}
		if (level->Free_To_Walk(c))
		{
			*mx=c.x;
			*my=c.y;
			break;
		}

	}
	return dir1;
}

/*
 * Tries to move an item 'item' to direction given in 'dir'
 * if 'checkmonster' is true then does not allow items to move
 * if grid blocked by monster (items which are not walkable, like large
 * boulders with ITEM_NOTPASSABLE bit set)
 *
 * Returns 0 on successful move, >0 if not
 *
 */
int move_item(level_type *level, invnode *item, int dir, bool checkmonster)
{
	if (dir>9)
		return false;

	Coord ic(item->Get_Location());

	//set old location passable
	if (item->i.status & ITEM_NOTPASSABLE)
		level->Set_Flag(ic, CAVE_PASSABLE);

	/* move the item */
	ic.Move_Direction(dir);

	being *mptr;
	if (checkmonster && (item->i.status & ITEM_NOTPASSABLE))
		mptr=gameview.Get_Monster(ic);
	else
		mptr=0;

	int res=0;

	/* if the new location is passalbe, actually update the coords */
	if (mptr==0 && level->Is_Passable(ic))
	{
		item->Set_Location(ic);
	}
	else if (mptr)
		res=BLOCKED_MONSTER;
	else
		res=BLOCKED_WALL;

	/* if item is not walkable, mark the level location not walkable */
	if (item->i.status & ITEM_NOTPASSABLE)
	{
		/* mask the passable flag out from current level location */
		level->Clear_Flag(ic, CAVE_PASSABLE);
	}

	return res;
}

/*
** A very intelligent monster move routine :-)
**
** 1.7.-97 HEY! Now monsters can do nicer target follow and flee.
**              it's not perfect but works nicely for now.
**
** This routine returns the amount of timeunits taken by monster move
*/
int move_monster(being *monster, level_type *level)
{
	being *mptr;

	int ticks=0; //how much time is spent
	int nx=monster->x;
	int ny=monster->y;

	/* if monster is in attackmode, let him attack his target (STUPID!)*/
	/* with this stupid code, hostile monsters just need to move into */
	/* players position... :-) */

	/* get coordinates for the target */
	const Coord t=monster->target.Get_Location();
	int tx=t.x;
	int ty=t.y;

	/* confusion walk */
	int dir;
	if (monster->conditions.get_value(CONDIT_CONFUSED) > 0)
	{
		dir=monster->lastdir=1+RANDU(9);
		tx=nx - move_dx[monster->lastdir];
		ty=ny - move_dy[monster->lastdir];
	}
	else dir=0;

	/* first check the FLEEMODE */
	if ((monster->m.status & MST_FLEEMODE))
	{
		monster_moveFROMtarget(level, &nx, &ny, tx, ty);
	}
	else if ((monster->m.status & MST_ATTACKMODE))
	{
		//if (!monster->target)
		//	monster_moveTOtarget(level, &nx, &ny, tx, ty, true);
		//else
		//note: getpast parameter not used
		monster_moveTOtarget(level, &nx, &ny, tx, ty, false);
	}
	else if ((monster->m.status & MST_PURSUEITEM))
	{
		monster_moveTOtarget(level, &nx, &ny, tx, ty, true);
	}
	/* otherwise move randomly :-) */
	else
	{
		/* if the monster has a path */
		if (monster->path.has_path())
		{
			monster->path.movealong(&nx, &ny);
		}
		else
			/* if monster is in corridor mode, move from last position */
			if ((monster->lastdir>0) && (monster->lastdir<10))
			{
				tx=nx - move_dx[monster->lastdir];
				ty=ny - move_dy[monster->lastdir];
				monster->lastdir=monster_moveFROMtarget(level, &nx, &ny, tx, ty);

				/* end corridor mode when over 2 possible directions to walk */
				if (sur_countall(level, nx, ny)>4)
					monster->lastdir=0;
			}
			else
			{
				dir=1+RANDU(9);
				tx=monster->x + move_dx[dir];
				ty=monster->y + move_dy[dir];

				monster->lastdir=monster_moveFROMtarget(level, &nx, &ny, tx, ty);

				if (nx < 1) nx=1;
				if (ny < 1) ny=1;
				if (nx > level->sizex-2) nx=level->sizex-2;
				if (ny > level->sizey-2) ny=level->sizey-2;


				if ((sur_countall(level, nx, ny)>4))
					monster->lastdir=0;
			}
	}

	Coord nc(nx, ny);

	/* open closed doors */
	if (level->Get_Terrain(nc) == TYPE_DOORCLOS)
	{
		if (npc_races[monster->m.race].behave & BEHV_ANIMAL)
		{
			if (!(npc_races[monster->m.race].behave & BEHV_CANUSEITEM))
			{
				monster->lastdir=0;
				return 0;
			}
		}

		open_door(monster, level, nc);

		return Gametime::Calculate(monster->stat[STAT_SPD].Get(),
			TIME_CLOSEDOOR, monster->tactic);
	}

	if (player.Is_At(nc))
	{
		if ((monster->m.status & MST_ATTACKMODE))
		{
			ticks=monster->Calculate_Time(TIME_MELEEATTACK);

			monster_meleeattack(monster, level, &player);

			/* return after attack */
			return ticks;
		}
		else
		{
			ticks=monster->Calculate_Time(TIME_MOVEAROUND);

			/* forget corridor mode */
			monster->lastdir=0;

			nx=monster->x;
			ny=monster->y;
			const Coord pc=player.Get_Location();

			if (monster_moveAROUNDtarget(level, &nx, &ny,
				pc.x, pc.y)==5)
			{
				monster->target.Clear();

				if (monster->m.status & MST_PURSUEITEM)
					monster->m.status^=MST_PURSUEITEM;

				/* make the monster a bit more angry */
				monster->m.attitude++;

				string s;
				if (gameview.Is_Visible(monster->x, monster->y))
				{
					s.append("You are standing on ");
					s.append(monster_sprintf(monster, false, true));
					s.append("'s way!");
				}
				else
					s.append("Someone tries to get past of you.");

				msg.newmsg(s, C_WHITE);

			}
			/* take a move from players coord */

			/* forget target */

		}
	}
	else
	{
		if ((nx!=monster->x) || (ny!=monster->y))
		{
			mptr=gameview.Get_Monster(nc);
		}
		else mptr=0;

		if (mptr)
		{
			if ((monster->m.status & MST_ATTACKMODE) &&
				(mptr==monster->target.olento))
			{
				/* do the attack */
				monster_meleeattack(monster, level, mptr);

				/* timeunits taken by monster attack */
				ticks=monster->Calculate_Time(TIME_MELEEATTACK);
			}
			else
			{
				/* angrify monster */
				ticks=monster->Calculate_Time(TIME_MOVEAROUND);
				monster->m.attitude++;
			}

			monster->lastdir=0;

			/* return after attack */
			return ticks;
		}
	}

	/* set new coordinates */
	if (level->Is_Passable(nx, ny))
	{
		/* monster is able to move if LEGS HP > 0 or it doesn't have
			legs, legless monsters move with their body! */
		if (!(monster->m.status & MST_CANTMOVE))
		{
			monster->Move_To(nc);
		}
		else
		{
			if (RANDU(100) < 20)
			{
				string s=monster_sprintf(monster, true, true);
				s.append(" seems unable to move!");
				msg.add_dist(level, monster->x, monster->y,
					s.c_str(), C_WHITE, NULL, CHB_CYAN);
			}
		}

		/* timeunits taken by monster move */
		ticks=monster->Calculate_Time(TIME_MOVEAROUND);

		if ((monster->m.status & MST_PURSUEITEM) &&
			monster->Is_Spotting())
		{
			const Coord mc=monster->Get_Location();

			if (mc==t) //arrived at target location
			{
				//note: SOMEHOW SHOULD DECIDE TO GET OR NOT TO GET

				invnode *iptr=gameview.Get_Item(mc);
				while (iptr)
				{
					/* take an item */
					if (!monster->Pick_Up_Item(level, iptr))
						break;

					/* check if there's another item to take */
					iptr=gameview.Get_Item(mc);
				}

				monster->m.status^=MST_PURSUEITEM;
				monster->target.Clear();

				ticks=monster->Calculate_Time(TIME_PICKUP);
			}
		}
	}
	else
	{
		monster->lastdir=0;
	}

	return ticks;
}

/*
** handles player moves
** value for 'dir is from 0 to 9. Direction of move is actually
** taken from tables move_dx[] and move_dy[]
*/
int moveplayer(playerinfo *plr, int dir, level_type *level, bool autowalk)
{
	Coord pc=plr->Get_Location();

	int nx=pc.x;
	int ny=pc.y;

	//note: this is quite slow way to get confusion
	if (plr->conditions.get_value(CONDIT_CONFUSED) > 0)
		dir=1+RANDU(9);

	nx+=move_dx[dir];
	ny+=move_dy[dir];
	const bool is_outside=level->Is_Outside(nx, ny);

	if (world->Get_Level_Type()==DTYPE_TOWN)
	{
		if (is_outside)
		{
			string s("Do you want to leave ");
			s.append(world->Get_Level_Name());

			if (confirm_yn(s.c_str(), false, true))
			{
				plr->lastdir=STAIROUT;
				world->Player_Go_Up();
			}
			return 0;
		}
	}

	if (plr->huntmode)
	{
		if (is_outside)
		{
			if (confirm_yn("Leave this encounter", false, true))
			{
				world->Player_Go_Outworld();
			}
			return 0;
		}
	}

	if (is_outside)
	{
		msg.newmsg("Are you going to the void?", C_RED);
		msg.newmsg("I wouldn't mind but it would cause some nasty pagefaults!",
			C_RED);
		msg.newmsg("So leave it!", C_RED);
		return 0;
	}

	Coord nc(nx, ny);

	being *mptr=gameview.Get_Monster(nc);

	if (mptr)
	{
		plr->repeatwalk=false;

		if (!autowalk)
			meleeattack(plr, level, mptr);

		return 0;
	}
	else if (plr->conditions.get_value(CONDIT_BADLEGS) > 0)
	{
		msg.newmsg("You can't walk now.", C_RED);
		msg.newmsg("Your legs are badly injured!", C_RED);

		plr->Spend_Time(TIME_BADLEGS);

		return 0;
	}

	int temptime=level->Checkterraineffects(nx, ny);

	/* issue automatic searching skill */
	if (plr->searchmode)
		search_surroundings(plr, level, true);

	if (temptime>0)
	{
		/* substract moving time */
		plr->Spend_Time(temptime);

		if (plr->searchmode)
			plr->timetaken-=TIME_AUTOSEARCH;

		plr->Set_Location(nx, ny);
		plr->Noticestuff(level);
	}
	else
	{
		if (autowalk)
			return 0;

		int dstat=DOORSTAT_NODOOR;

		/* automatic doors ? */
		if (CONFIGVARS.autodoor)
		{
			if (level->Is_Door(nc))
			{
				if (confirm_yn("There's a door, open it", true, true))
				{
					dstat=open_door(plr, level, nc);
				}
			}
		}

		if (dstat==DOORSTAT_NODOOR)
		{
			bool pushres=false;
			if (CONFIGVARS.autopush)
				pushres=player_push(plr, level, dir);

			if (!pushres && !plr->repeatwalk && temptime<0)
				plr->Hitwall();
		}
		else
			plr->Spend_Time(TIME_OPENDOOR);

		return 1;
	}

	return 1; // successible move
}

Coord move_to_direction(int dir, playerinfo *plr)
{
	Coord rv=plr->Get_Location();
	rv.y+=move_dy[dir];
	rv.x+=move_dx[dir];
	return rv;
}

Coord move_to_direction(int dir, const Coord &c)
{
	Coord rv(c);
	rv.y+=move_dy[dir];
	rv.x+=move_dx[dir];
	return rv;
}

bool player_push(playerinfo *plr, level_type *level, int dir)
{
	bool automode=false;

	if (!dir)
		dir=dir_askdir("Push", false);
	else
		automode=true;

	if (dir==5 || dir>9 || dir<1)
	{
		if (!automode)
			msg.newmsg("Not a good idea.", C_WHITE);
		return false;
	}

	Coord mc=move_to_direction(dir, plr);

	/* is there a monster? */
	being *mptr=gameview.Get_Monster(mc);
	if (mptr)
	{
		string s=monster_sprintf(mptr, true, true);
		s.append(" doesn't really want to be pushed around!");
		msg.vnewmsg(C_WHITE, s.c_str());
		return false;
	}

	/* push the largest item always */
	invnode *iptr=level->inv.Find_Largest_Item(mc);
	if (iptr)
	{
		int moveres=move_item(level, iptr, dir, true);

		if (!moveres)
		{
			display->Item_Info(&iptr->i, iptr->i.weight, iptr->count, "You push");

			/* large items crush */
			if (iptr->i.weight > 300000)
				level->inv.crush_items(iptr->x, iptr->y, iptr);
		}
		else
		{
			msg.vnewmsg(C_WHITE, "You push the %s, but it won't move.",
				iptr->i.name.c_str());
			if (moveres==BLOCKED_MONSTER)
			{
				msg.newmsg("You hear some noises on the other side, maybe"
					" that's the reason.", C_WHITE);
			}
		}
		/* idea!
		 * note: Kun tyontaa ison lohkareen esineiden paalle -> ruskis
		 * pyorea lohkare voisi lahtea pyorimaan?!?
		 */
		return true;
	}

	return false;
}

void repeatmove(playerinfo *plr, int dir, level_type *level)
{
	if (CONFIGVARS.foodwarn && plr->nutr==FOOD_FAINTING)
	{
		msg.newmsg("Not now, you're starving!", C_RED);
		return;
	}

	if (CONFIGVARS.monster_disturb && plr->monsterinsight)
	{
		msg.newmsg("Not now, there're possible hostile monsters around",
			C_RED);
		return;
	}

	Coord pc=move_to_direction(dir, plr);

	int numdir=sur_countpass(level, pc.x, pc.y);
	int movres=moveplayer(plr, dir, level, true);

	if (movres)
		Game.Passturn(CONFIGVARS.repeatupdate, true);
	else
		return;

	int repeats=0;
	plr->repeatwalk=true;
	bool END_REPEAT=false;

	while (!END_REPEAT) //note: this condition is always false
	{
		pc=plr->Get_Location();

		/* get new surrounding conditions */
		int oldnumdir=numdir;
		numdir=sur_countpass(level, pc.x, pc.y);

		/* if surrounding conditions changed */
		if (numdir!=oldnumdir)
			break;

		/* if repeat walk somehow got disabled */
		if (!plr->repeatwalk)
			break;

		/* if walked into an item */
		if (CONFIGVARS.item_disturb)
		{
			if (gameview.Count_Items(pc)>0)
				break;
		}

		/* if walked into a stair */
		if (CONFIGVARS.stair_disturb)
		{
			if (level->Has_Stairs(pc))
				break;
		}

		/* if monster in vision range */
		if (CONFIGVARS.monster_disturb && plr->monsterinsight)
			break;

		/* if repeats exeeded */
		if (repeats>CONFIGVARS.repeatcount)
			break;

		/* if still alive, move on */
		movres=moveplayer(plr, dir, level, true);

		Game.Passturn(CONFIGVARS.repeatupdate, true);

		/* walk corridors */
		if (movres)
		{
			//	 END_REPEAT=true;
			if ((dir==DIR_LEFT) || (dir==DIR_RIGHT) ||
				(dir==DIR_UP) || (dir==DIR_DOWN))
			{
				if (sur_countpass(level, pc.x, pc.y) == 2)
				{
					if (level->Is_Passable(pc.x, pc.y-1) &&
						dir!=DIR_DOWN) dir=DIR_UP;
					else if (level->Is_Passable(pc.x-1, pc.y) &&
						dir!=DIR_RIGHT) dir=DIR_LEFT;
					else if (level->Is_Passable(pc.x, pc.y+1) &&
						dir!=DIR_UP) dir=DIR_DOWN;
					else if (level->Is_Passable(pc.x+1, pc.y) &&
						dir!=DIR_LEFT) dir=DIR_RIGHT;
					END_REPEAT=false;
				}
				else
					END_REPEAT=false;
			}
			//	 END_REPEAT=false;
		}
		else
			break;

		//      if((dir==DIR_LEFT) || (dir==DIR_RIGHT) ||
		//	 (dir==DIR_UP) || (dir==DIR_DOWN))
		repeats++;
	}
	plr->repeatwalk=false;
}

int shopkeeper_move(level_type *level, being *keeper)
{
	int tx, ty;
	int gx1=-1, gy1=-1, gx2=-1, gy2=-1, gx3=-1, gy3=-1, gx4=-1, gy4=-1;

	int drx=level->rooms[keeper->roomnum].doorx;
	int dry=level->rooms[keeper->roomnum].doory;

	Coord pc=player.Get_Location();
	const int player_distance=distance(drx, dry, pc.x, pc.y);

	/* current coords */
	int nx=keeper->x;
	int ny=keeper->y;

	if (level->Get_Terrain(drx-1, dry-1) == TYPE_ROOMFLOOR)
	{
		gx1=drx-1;
		gy1=dry-1;
	}
	else if (level->Get_Terrain(drx-1, dry+1) == TYPE_ROOMFLOOR)
	{
		gx2=drx-1;
		gy2=dry+1;
	}
	else if (level->Get_Terrain(drx+1, dry-1) == TYPE_ROOMFLOOR)
	{
		gx3=drx+1;
		gy3=dry-1;
	}
	else if (level->Get_Terrain(drx+1, dry+1) == TYPE_ROOMFLOOR)
	{
		gx4=drx+1;
		gy4=dry+1;
	}

	/* if attacking then move after the target */
	if ((keeper->m.status & MST_ATTACKMODE) || keeper->path.has_path())
	{
		/* evaluate path back to the door */
		if (distance(drx, dry, nx, ny) > SHOPKEEPER_MAXDOORDIST
			&& keeper->path.has_path()==false)
		{
			keeper->path.find_route(level, keeper->x, keeper->y,
				drx, dry);
			if (keeper->m.status & MST_ATTACKMODE)
				keeper->m.status ^= MST_ATTACKMODE;

		}

		if (drx == keeper->x && dry == keeper->y)
			keeper->path.clear();

		keeper->target.Clear();

		return move_monster(keeper, level);

		/* move if the door is not reached */
  /*
		if(distance(drx, dry, nx, ny) < 5) {
	   keeper->targetx=0;
	   keeper->targety=0;
	   return move_monster(keeper, level);
		}
		else {
	   keeper->m.status ^= MST_ATTACKMODE;
	   keeper->targetx=drx;
	   keeper->targety=dry;
		}
  */
	}

	const int keeperoom=keeper->roomnum;
	Coord tarpos=keeper->target.Get_Location();

	/* check for items in the door area */
	if (keeper->Is_Spotting()==false)
	{
		for (int dir=1; dir<10; dir++)
		{
			tx=drx+move_dx[dir];
			ty=dry+move_dy[dir];

			Coord tc(tx, ty);

			if (level->Get_Terrain(tc) == TYPE_ROOMFLOOR)
			{
				invnode *iptr=gameview.Get_Item(tc);
				if (iptr)
				{
					if (level->Inside_Room(keeperoom, tc)==false)
						keeper->target.Set(iptr, tc);
					break;
				}
			}
		}
	}
	else
	{
		Coord kc=keeper->Get_Location();

		if (kc==tarpos)
		{
			keeper->target.Clear();

			invnode *iptr=gameview.Get_Item(kc);
			if (iptr)
				shopkeeper_get(level, keeper, iptr);
		}
	}

	const char *keepername=keeper->m.name.c_str();

	if (keeper->Is_Spotting() && player_distance>1)
	{
		//      iptr=level->Get_Top_Item(keeper->targetx, keeper->targety);
		//      if(iptr) {
		monster_moveTOtarget(
			level, &nx, &ny, tarpos.x, tarpos.y, false);
		//      }
		//      else {
		//	 keeper->targetx=drx;
		//	 keeper->targety=dry;
		//      }
	}
	/* move to block the door area if player has taken something */
	else if ((player.bill > 0) &&
		(level->rooms[keeperoom].flags & ROOM_PLAYERHERE))
	{
		if (RANDU(100) < 3)
		{
			msg.newmsg(C_WHITE, "%s %s",
				keepername, texts->Get_Random(Script::Keeper_Bill_Message));
		}

		if (nx!=drx || ny!=dry)
			monster_moveTOtarget(level, &nx, &ny, drx, dry, true);
	}
	else
	{
		/* if player is closing the shop door, move near the door area */
		if (player_distance < 8)
		{
			if (gx1>0 && gy1>0)
			{
				if (nx!=gx1 || ny!=gy1)
					monster_moveTOtarget(level, &nx, &ny, gx1, gy1, true);
			}
			else if (gx2>0 && gy2>0)
			{
				if (nx!=gx2 || ny!=gy2)
					monster_moveTOtarget(level, &nx, &ny, gx2, gy2, true);
			}
			else if (gx3>0 && gy3>0)
			{
				if (nx!=gx3 || ny!=gy3)
					monster_moveTOtarget(level, &nx, &ny, gx3, gy3, true);
			}
			else if (gx4>0 && gy4>0)
			{
				if (nx!=gx4 || ny!=gy4)
					monster_moveTOtarget(level, &nx, &ny, gx4, gy4, true);
			}
			if (level->rooms[keeper->roomnum].flags & ROOM_PLAYERHERE)
			{
				if (RANDU(100) < 3)
				{
					msg.newmsg(C_WHITE, "%s %s",
						keepername, texts->Get_Random(Script::Keeper_Nobill_Message));
				}
			}
		}
		else
		{
			const int dir=1+RANDU(9);
			tx=keeper->x + move_dx[dir];
			ty=keeper->y + move_dy[dir];

			/* drop items, but not next to the door */
			if (distance(drx, dry, nx, ny) > 1)
				shopkeeper_drop(level, keeper);

			if (level->Get_Terrain(tx, ty) != TYPE_ROOMFLOOR)
			{
				nx=keeper->x;
				ny=keeper->y;
			}
			else
			{
				monster_moveTOtarget(level, &nx, &ny, tx, ty, true);

				if (RANDU(500) < 3)
					texts->Random_Message(Script::Keeper_Unseen);
			}
		}
	}

	const int ticks = keeper->Calculate_Time(TIME_MOVEAROUND);

	if (nx!=pc.x || ny!=pc.y)
	{
		Coord c(nx, ny);
		keeper->Move_To(c);
	}

	return ticks;
}

// counts passable horizontal and vertical directions
// around given location
int sur_countpass(level_type *level, int x, int y)
{
	int count=0;

	for (int d=0; d<4; d++)
	{
		Coord c(x, y);
		c.Move_Direction(Way::Get_Main_Direction(d));
		if (level->Is_Passable(c)) count++;
	}

	if (CONFIGVARS.door_disturb)
	{
		for (int d=0; d<4; d++)
		{
			Coord c(x, y);
			c.Move_Direction(Way::Get_Main_Direction(d));
			if (level->Has_Door(c)) count++;
		}
	}

	return count;
}

/* counts all directions for passable directions */
int sur_countall(level_type *level, int x, int y)
{
	int count=0;

	for (int d=0; d<8; d++)
	{
		Coord c(x, y);
		c.Move_Direction(Way::Get_Direction(d));
		if (level->Is_Passable(c)) count++;
	}

	return count;
}

// counts passable diagonal directions around given location
int sur_countpass_dia(level_type *level, int x, int y)
{
	int count=0;

	for (int d=0; d<4; d++)
	{
		Coord c(x, y);
		c.Move_Direction(Way::Get_Diagonal_Direction(d));
		if (level->Is_Passable(c)) count++;
	}

	if (CONFIGVARS.door_disturb)
	{
		for (int d=0; d<4; d++)
		{
			Coord c(x, y);
			c.Move_Direction(Way::Get_Diagonal_Direction(d));
			if (level->Has_Door(c)) count++;
		}
	}

	return count;
}

//Returns true if did teleport.
bool teleport_item(level_type *level, invnode *iptr)
{
	Coord c=get_random_coord(level);
	Coord old=iptr->Get_Location();

	//in rare cases if the location is the old one
	if (c==old)
		return false;

	string iname=item_name(iptr);

	string s=iname;
	s.append(" disappears.");

	msg.add_dist(level, iptr->x, iptr->y, s.c_str(), C_CYAN,
		"You hear a distant \"swoosh\"-sound.", C_CYAN);

	gameview.Refresh_Item_Map(old); //remove from old location
	iptr->Set_Location(c);
	gameview.Refresh_Item_Map(c); //add to new location

	string q=iname;
	q.append(" appears in your sight.");

	msg.add_dist(level, iptr->x, iptr->y, q.c_str(), C_CYAN,
		"You hear a distant \"Zap!\".", C_CYAN);

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
	return true;
}

//Returns true if did teleport.
bool teleport_monster(level_type *level, being *mptr)
{
	Coord c=get_random_coord(level);

	//in rare cases if the location is the old one
	if (c==mptr->Get_Location())
		return false;

	string moname=monster_sprintf(mptr, true, true);
	string s=moname;
	s.append(" disappears.");

	msg.add_dist(level, mptr->x, mptr->y, s.c_str(), C_CYAN,
		"You hear a distant \"swoosh\"-sound.", C_CYAN);

	mptr->Move_To(c);

	string q=moname;
	q.append(" appears in your sight.");
	msg.add_dist(level, mptr->x, mptr->y, q.c_str(), C_CYAN,
		"You hear a distant \"Zap!\".", C_CYAN);

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
	return true;
}

void teleport_player(level_type *level, bool inform, bool not_in_room)
{
	Coord c;
	Coord old=player.Get_Location();

	if (not_in_room) //skip room floors
		c=get_random_good_location(level);
	else
		c=get_random_coord(level);
		
	if (inform)
	{
		if (old==c)
			msg.newmsg("You feel disoriented for a moment.", C_GREEN);
		else
			msg.newmsg("You're displaced by strange forces.", C_GREEN);
	}

	player.Jump_To(c);

	Game.noticeevents(level);	
}
