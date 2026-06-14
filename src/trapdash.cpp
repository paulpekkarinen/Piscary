/**************************************************************************
 * trapdash.cpp --                                                        *
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

//Refactored 14.7.2022 - 12.4.2026 by Paul K. Pekkarinen

#include <cstring>
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "codex.h"
#include "damage.h"
#include "dice.h"
#include "display.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "output.h"
#include "spawner.h"
#include "terrain.h"
#include "trapdash.h"
#include "way.h"

using std::string;

bool trap_bomb(level_type *level, Trap &t);
bool trap_boulder(level_type *level, Trap &t, int tx, int ty);
bool trap_eshock(level_type *level, Trap &t);

/* handle trapped door, return true if the trap removed the door */
bool handletrap(level_type *level, const Coord &d, Actor *monster)
{
	if (level->Has_Object(d, OBJECT_TRAP)==false)
		return false;

	const bool door=level->Is_Door(d);
	Coord monc=monster->Get_Location();

	if (monster->Is_Player()==false)
	{
		string moname=monster_sprintf(monster, true, true);
		msg.vnewdist(level, monc.x, monc.y, C_RED, C_RED,
			NULL,
			"%s triggers a trap.",
			moname.c_str());
	}
	else
	{
		//gameview.Show(); //note: test if needed
		msg.add("You trigger a trap...", C_RED);
	}

	bool tres;
	Trap &t=level->Get_Trap(d);
	const int tid=t.Get_Type();

	switch (tid)
	{
		case TRAP_BOULDER:
			tres=trap_boulder(level, t, monc.x, monc.y);
			break;
		case TRAP_BOMB:
			tres=trap_bomb(level, t);
			break;
		case TRAP_ESHOCK:
			tres=trap_eshock(level, t);
			break;
		default:
			tres=false;
	}

	/* remove if the trap is in a door or flagged as removable on
	 * blow up */
	if (!(list_traps[tid].flags & TRAPF_NOREMOVE) || door)
		level->Remove_Trap(d);
	else
		level->Reveal_Trap(d);

	return tres;
}

bool trap_eshock(level_type *level, Trap &t)
{
	Coord d=t.Get_Location();

	Actor *mptr=gameview.Get_Actor(d);

	if (gameview.Is_Visible(d))
	{
		const int trap_damage=t.Get_Damage();

		if (player.Is_At(d))
		{
			player_killer("Got zapped by an electric trap.");
			msg.add("Zap...", CH_RED);
			damage_issue(level, &player, NULL, ELEMENT_ELEC,
				trap_damage, RANDU(HPSLOT_MAX+1), NULL);
		}
		else if (mptr)
		{
			string moname=monster_sprintf(mptr, true, true);
			msg.vadd(C_WHITE, "%s got zapped.",
				moname.c_str());
			damage_issue(level, mptr, NULL, ELEMENT_ELEC,
				trap_damage, RANDU(HPSLOT_MAX+1), NULL);
		}

		Coord c=gameview.Get_Screen_Location(d);

		for (int i=0; i<8; i++)
		{
			my_setcolor(RANDU(16));
			put_char_to('*', c);

			refresh();
			delay(200);
		}
	}

	return true;
}

bool trap_boulder(level_type *level, Trap &t, int tx, int ty)
{
	Coord c=t.Get_Location();

	/* this is a door trap only */
	if (level->Is_Door(c)==false)
		return false;

	/* open the door */
	level->Open_Door(c);

	const int dir=Way::Get_Dir_To(level, c.x, c.y, tx, ty);
	Spawner spw(level);

	/* create a boulder */
	invnode *boulder = spw.Create_Item(c,
		IS_SPECIAL, SPECIAL_BOULDER, 1, -1);

	gameview.Show();

	if (!boulder)
		return false;
	else if (gameview.Is_Visible(c))
	{
		msg.add("A large boulder drops as the door opens!", C_WHITE);
	}

	/* do until hits a wall */

	int force=5+RANDU(10);
	bool moveres=false;
	bool hitsome;

	while (!moveres && force>0)
	{
		hitsome=false;
		/* move the boulder */
		moveres=move_item(level, boulder, dir, false);
		force--;

		gameview.Show();

		tx=boulder->x;
		ty=boulder->y;

		Coord tc(tx, ty);

		const int trap_damage=t.Get_Damage();

		if (gameview.Is_Visible(tx, ty))
		{
			/* crush items */
			level->inv.crush_items(tx, ty, boulder);

			if (player.Is_At(tx, ty))
			{
				player_killer("a nasty rolling boulder.");
				msg.newmsg("You're crushed by the boulder.", CH_RED);
				damage_issue(level, NULL, NULL, ELEMENT_NOTHING,
					trap_damage, -1,
					NULL);
				hitsome=true;
			}
			else
			{
				being *mptr=gameview.Get_Monster(tc);

				if (mptr)
				{
					string moname=monster_sprintf(mptr, true, true);
					msg.vnewmsg(C_WHITE, "%s is crushed by rolling boulder.",
						moname.c_str());
					damage_issue(level, mptr, NULL, ELEMENT_NOTHING,
						trap_damage, -1,
						NULL);
					hitsome=true;
				}
			}

			if (hitsome)
			{
				//note: items were output here
				msg.notice();
				showmore(false, true);
				msg.update();
			}
			else
				delay(200);
		}
	}

	return true;
}

bool trap_bomb(level_type *level, Trap &t)
{
	int j, i, ci=0;
	int bx, by;
	bool somevis;

	const char bomb_seq[]="-+x*X*x+-";
	const int16u bomb_col[]={C_YELLOW, C_RED, CH_YELLOW,
		CH_RED, CH_RED, 0xffff};

	Coord d=t.Get_Location();

	/* destroy some dungeon first */
	for (i=1; i<10; i++)
	{
		bx=d.x + move_dx[i];
		by=d.y + move_dy[i];
		if (bx>0 && bx<level->sizex && by>0 && by<level->sizey)
			level->Set_Terrain(bx, by, TYPE_PASSAGE);
	}

	/* calculate new los */
	//note: may need to set fov recalculation flag
	gameview.Show();

	for (j=0; j<(int)strlen(bomb_seq); j++)
	{
		somevis=false;

		for (i=1; i<10; i++)
		{
			bx=d.x + move_dx[i];
			by=d.y + move_dy[i];

			/* make some damage for monsters and level structures */
			if (!j)
			{
				Coord e(bx, by);
				const int trap_damage=t.Get_Damage();

				if (player.Is_At(e))
				{
					player_killer("Roasted by a fire trap.");
					msg.newmsg("You're burned by the soaring flames..", CH_RED);
					damage_issue(level, NULL, NULL, ELEMENT_FIRE,
						trap_damage, -1,
						NULL);
				}

				being *mptr=gameview.Get_Monster(e);

				if (mptr)
				{
					string moname=monster_sprintf(mptr, true, true);
					msg.vnewdist(level, mptr->x, mptr->y, C_RED, C_RED,
						NULL,
						"%s is burned by the flames.",
						moname.c_str());

					damage_issue(level, mptr, NULL, ELEMENT_FIRE,
						trap_damage, -1,
						NULL);
				}

			}

			/* display it if player sees */
			if (gameview.Is_Visible(bx, by))
			{
				somevis=true;
				set_color(bomb_col[ci]);
				Coord c; //=player.Screen_Location(bx, by); //note: fix later

				put_char_to(bomb_seq[j], c);
			}
		}

		if (somevis)
		{
			msg.notice();
			delay(70);
		}

		ci++;
		if (bomb_col[ci]==0xffff)
			ci=0;
	}

	return true;
}
