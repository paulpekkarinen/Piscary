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
#include "window.h"
#include "world.h"

using std::string;

bool trap_bomb(level_type *level, Trap &t);
bool trap_boulder(level_type *level, Trap &t, int tx, int ty);
bool trap_eshock(level_type *level, Trap &t);

//Create a trap.
void entrap_location(level_type *level)
{
	const int dir=dir_askdir("Entrap", false);
	if (dir==0) return;

	Window menuwin("Which trap?", 30, 10, 27, TRAP_MAXNUM+2, CH_GREEN, CH_WHITE);

	string s;
	char ch='a';
	for (int i=TRAP_BOULDER; i<TRAP_MAXNUM; i++)
	{
		s.push_back(ch);
		s.append(") ");
		s.append(list_traps[i].name);
		s.append("\n");

		ch++;
	}

	menuwin.Draw(s.c_str());

	const int t=my_getch()-(int)'a'+TRAP_BOULDER;
	display->Redraw(world->Get_Current_Level());

	if (t<TRAP_BOULDER || t>=TRAP_MAXNUM)
	{
		msg.newmsg("You cancel creating the trap.");
		return;
	}

	Coord c=move_to_direction(dir, &player);

	const int rv=level->Create_Trap(t, c);

	switch (rv)
	{
		case 0: msg.newmsg("You created a trap."); break;
		case -1: msg.newmsg("There already is a trap in there!"); break;
		case -2: msg.newmsg("You fail to create trap there."); break;
		default: break;
	}
}

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
		gameview.Show();
		msg.add("You trigger a trap...", C_RED);
	}

	bool tres;
	Trap &t=level->Get_Trap(d);
	const int tid=t.Get_Type();

	msg.newmsg(C_WHITE, "The trap is %s.", list_traps[tid].name);

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
	Coord pos=t.Get_Location();

	Actor *mptr=gameview.Get_Actor(pos);

	Damage d(t.Get_Damage(), ELEMENT_ELEC);

	if (player.Is_At(pos))
	{
		msg.add("Zap...", CH_RED);
		if (damage_issue(level, &player, d, 0)<=0)
			player_killer("Got zapped by an electric trap.");
	}
	else if (mptr)
	{
		if (gameview.Is_Visible(pos))
		{
			string moname=monster_sprintf(mptr, true, true);
			msg.vadd(C_WHITE, "%s got zapped.",
				moname.c_str());
		}
		damage_issue(level, mptr, d, 0);
	}

	Coord c=gameview.Get_Screen_Location(pos);

	for (int i=0; i<8; i++)
	{
		my_setcolor(RANDU(16));
		put_char_to('*', c);

		refresh();
		delay(200);
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

		Damage d(t.Get_Damage(), ELEMENT_NOTHING, -1);

		if (gameview.Is_Visible(tx, ty))
		{
			/* crush items */
			level->inv.crush_items(tx, ty, boulder);

			if (player.Is_At(tx, ty))
			{
				if (damage_issue(level, &player, d,
					"You're crushed by a boulder.")<=0)
						player_killer("a nasty rolling boulder.");

				hitsome=true;
			}
			else
			{
				being *mptr=gameview.Get_Monster(tc);

				if (mptr)
				{
					string moname=monster_sprintf(mptr, true, true);
					msg.vnewmsg(C_WHITE, "%s is crushed by a rolling boulder.",
						moname.c_str());
					damage_issue(level, mptr, d, 0);
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
	int color_index=0;
	bool somevis;

	const char bomb_seq[]="-+x*X*x+-";
	const int16u bomb_col[]={C_YELLOW, C_RED, CH_YELLOW,
		CH_RED, CH_RED, 0xffff};

	Coord traploc=t.Get_Location();
	Coord b;

	/* destroy some dungeon first */
	for (int i=1; i<10; i++)
	{
		b.x=traploc.x + move_dx[i];
		b.y=traploc.y + move_dy[i];
		level->Set_Terrain(b, TYPE_PASSAGE);
	}

	for (int j=0; j<(int)strlen(bomb_seq); j++)
	{
		somevis=false;

		for (int i=1; i<10; i++)
		{
			b.x=traploc.x + move_dx[i];
			b.y=traploc.y + move_dy[i];

			/* make some damage for monsters and level structures */
			if (!j)
			{
				Damage d(t.Get_Damage(), ELEMENT_FIRE, -1);

				if (player.Is_At(b))
				{
					if (damage_issue(level, &player, d,
						"You're burned by the soaring flames..")<=0)
							player_killer("Roasted by a fire trap.");
				}

				being *mptr=gameview.Get_Monster(b);

				if (mptr)
				{
					string moname=monster_sprintf(mptr, true, true);
					msg.vnewdist(level, mptr->x, mptr->y, C_RED, C_RED,
						NULL,
						"%s is burned by the flames.",
						moname.c_str());

					damage_issue(level, mptr, d, 0);
				}
			}

			/* display it if player sees */
			if (gameview.Is_Visible(b))
			{
				somevis=true;
				set_color(bomb_col[color_index]);
				Coord c=gameview.Get_Screen_Location(b);

				put_char_to(bomb_seq[j], c);
			}
		}

		if (somevis)
		{
			msg.notice();
			delay(70);
		}

		color_index++;
		if (bomb_col[color_index]==0xffff)
			color_index=0;
	}

	return true;
}
