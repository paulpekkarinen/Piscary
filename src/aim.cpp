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
#include "names.h"
#include "output.h"
#include "terrain.h"
#include "way.h"

using std::string;

Aim::Aim(level_type *lvl)
	: level(lvl), olento(0)
{
	
}

bool Aim::Select()
{
	bool looping=true;
	bool update=false;
	bool rv=false;
	Coord pc=player.Get_Location();

	Show_Info();

	while (looping)
	{
		if (update)
		{
			gameview.Show();
			update=false;
		}
	
		Coord e=gameview.Get_Screen_Location(pc);

		gotoxy(e.x, e.y);
		put_char('*', CH_RED);

		Show_Spot_Info(pc);

		const int k=my_getch();

		switch (k)
		{
			case ' ':
				if (olento!=0)
				{
					display->Monster_Description((being*)olento);
					display->Redraw(level);
					Show_Info();
				}
			break;
			default:
			{
				const int dir=Way::Get_From_Keycode(k);
				if (dir!=-1)
				{
					Coord saved=pc;
					pc.Move_Direction(dir);
					if (gameview.Is_Outside_View(pc))
						pc=saved;
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

	print_toc(0, MSGLINE,
		"Use movement keys for location, 'x' to quit.", C_WHITE);
}

void Aim::Show_Spot_Info(const Coord &c)
{
	gotoxy(0, MSGLINE+1);
	set_color(CH_WHITE);
	
	if (gameview.Is_Visible(c)==false)
	{
		print_text("Unknown");
		return;
	}

	string s;
	
	olento=gameview.Get_Actor(c);
	if (olento!=0)
	{
		s=monster_sprintf(olento, true, true);
		s.append(" (More info with SPACE)");		
	}
	else
	{
		invnode *iptr=gameview.Get_Item(c);
		if (iptr!=0)
		{
			const int items=level->inv.Count_Items_Flat(c.x, c.y);
			if (items==1)
			{
				display->Item_Info(
					&iptr->i, iptr->i.weight, iptr->count, "It's");
			}
			else
			{
				s="A pile of items.";
			}			
		}
		else
		{
			const int tt=level->Get_Terrain(c);
			s=terrains[tt].desc;
		}				
	}

	if (s.empty()==false)
		print_text(s.c_str());

	clrtoeol();
}
