/**************************************************************************
 * aim.cpp --                                                             *
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

//Refactored 14.7.2022 - 4.5.2026 by Paul K. Pekkarinen

#include "aim.h"
#include "avatar.h"
#include "caves.h"
#include "display.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "magic.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "ranged.h"
#include "terrain.h"
#include "textdata.h"
#include "way.h"

using std::string;

const Aim_Data aimdata[Aim::Amt_Of_Modes]=
{
	{
		"Use movement keys for location ['x' to quit].",
		"",
		" (More info with SPACE)",
		"It's",
		"A pile of items.",
		".",
		false,
		'*'
	},
	{
		"Target with movement keys [n,t,SPACE, 'x' to quit or ?].",
		"Target = ",
		".",
		"Target = ",
		"Target = a pile of items.",
		" (no target).",
		true,
		'X'
	},
	{
		"Target with movement keys [n,t,SPACE, 'x' to quit or ?].",
		"Target = ",
		".",
		"Target = ",
		"Target = a pile of items.",
		" (no target).",
		false,
		'*'
	}
};

Aim::Aim(level_type *lvl, int tm)
	: level(lvl), mode(tm), lastidx(-1), spell(0)
{

}

Aim::Aim(level_type *lvl, Spell *sp)
	: level(lvl), mode(Spell_Target), lastidx(-1), spell(sp)
{

}

//Returns current target location, this can be without an actual target.
Coord Aim::Get_Target_Location()
{
	return target.pos;
}

bool Aim::Select()
{
	bool looping=true;
	bool update=false;
	bool rv=false;

	Coord pc=player.Get_Location();

	Starting_Location();
	Show_Info();

	while (looping)
	{
		if (update)
		{
			gameview.Show();

			if (CONFIGVARS.targetline==true &&
				aimdata[mode].show_targetline==true)
			{
				//note: wonder how these nulls work?
				ranged_line(level, 0, 0, false,
					pc.x,
					pc.y,
					pos.x, pos.y, NULL, NULL, 0);
			}

			update=false;
		}

		Coord e=gameview.Get_Screen_Location(pos);

		gotoxy(e.x, e.y);
		put_char(aimdata[mode].target_char, CH_RED);

		Show_Spot_Info();

		const int k=my_getch();

		switch (k)
		{
			case ' ':
			case 't':
				if (target.olento!=0)
				{
					if (mode==Look_Around)
					{
						display->Monster_Description((being*)target.olento);
						display->Redraw(level);
						Show_Info();
						continue;
					}
				}

				rv=true; //target selected
				looping=false;
			break;
			case 'n': //choose next target
			{
				lastidx=level->crew.Target_Nearest(level, pos, lastidx);
				if (lastidx<0) //note: what is this...
					lastidx=level->crew.Target_Nearest(level, pos, lastidx);
			}
			break;
			case '?':
				if (mode==Missile_Target)
				{
					text_data->View(Text_Data::Missile_Help);
					Show_Info();
				}
			break;
			default:
			{
				const int dir=Way::Get_From_Keycode(k);
				if (dir!=-1)
				{
					Coord saved=pos;
					pos.Move_Direction(dir);
					if (gameview.Is_Outside_View(pos))
						pos=saved;
					else
						update=true;
				}
				else
				{
					if (is_quit_key(k))
						looping=false;
				}
			}
			break;
		}
	}

	return rv;
}

void Aim::Show_Info()
{
	clearline(MSGLINE);
	clearline(MSGLINE+1);

	print_toc(0, MSGLINE, aimdata[mode].info_line, C_WHITE);
}

void Aim::Show_Spot_Info()
{
	gotoxy(0, MSGLINE+1);
	set_color(CH_WHITE);
	target.Clear();

	if (gameview.Is_Visible(pos)==false)
	{
		print_text("Unknown");
		clrtoeol();
		return;
	}

	string s;

	Actor *o=gameview.Get_Actor(pos);
	if (o!=0)
	{
		if (o->Is_Player())
		{
			if (Can_Target_Yourself())
			{
				target.Set(o);
				s.append(aimdata[mode].monster_prefix);
				s.append("yourself");
				s.append(aimdata[mode].monster_suffix);
			}
			else
				s="Self targetting not allowed.";
		}
		else
		{
			if (Can_Target_Creature())
			{
				target.Set(o);
				s.append(aimdata[mode].monster_prefix);
				s.append(monster_sprintf(o, true, true));
				s.append(aimdata[mode].monster_suffix);
			}
			else
				s="Monster targetting not allowed.";
		}
	}
	else
	{
		invnode *iptr=gameview.Get_Item(pos);
		if (iptr!=0)
		{
			if (Can_Target_Item())
			{
				target.Set(iptr, pos);
				const int items=level->inv.Count_Items_Flat(pos.x, pos.y);
				if (items==1)
				{
					display->Item_Info(
						iptr, aimdata[mode].item_prefix);
				}
				else
				{
					s=aimdata[mode].itempile;
				}
			}
			else
				s="Item targetting not allowed.";
		}
		else
		{
			target.Set(pos);

			const int tt=level->Get_Terrain(pos);
			s=terrains[tt].desc;
			s.append(aimdata[mode].terrain_suffix);
		}
	}

	if (s.empty()==false)
		print_text(s.c_str());

	clrtoeol();
}

void Aim::Starting_Location()
{
	switch (mode)
	{
		case Spell_Target:
		case Look_Around: pos=player.Get_Location(); break;
		case Missile_Target:
		{
			lastidx=level->crew.Target_Nearest(level, pos, 0);
			//no monsters found, use player's location
			if (lastidx==-1)
				pos=player.Get_Location();
		}
		break;
		default: break;
	}
}

bool Aim::Can_Target_Creature()
{
	if (mode==Spell_Target)
	{
		if (spell->other >= 0)
			return true;

		return false;
	}

	return true;
}

bool Aim::Can_Target_Item()
{
	if (mode==Spell_Target)
	{
		if (spell->levitem >= 0)
			return true;

		return false;
	}

	return true;
}

bool Aim::Can_Target_Yourself()
{
	if (mode==Spell_Target)
	{
		if (spell->self >= 0)
			return true;

		return false;
	}

	return true;
}
