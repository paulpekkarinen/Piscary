/**************************************************************************
 * saldebug.cpp --                                                        *
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

//Refactored 6.12.2021 - 10.4.2026 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <array>
#include <cstdarg>
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "messbuff.h"
#include "node.h"
#include "output.h"
#include "selpack.h"
#include "saldebug.h"
#include "terrain.h"
#include "uncover.h"
#include "viewtile.h"
#include "way.h"
#include "window.h"
#include "world.h"

Debug::Debug(bool m)
	: debug_mode(m)
{
	debug_messages=new Message_Buffer(60);
}

Debug::~Debug()
{
	delete debug_messages;
}

void Debug::Create_Level()
{
	world->currnode->Remake_Level();
	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}

void Debug::Identify()
{
	player.backpack->Set_Filter(-1);
	invnode *matti=player.backpack->Get_Handle("Identify what?");

	if (matti)
		matti->i.status|=ITEM_IDENTIFIED;
}

void Debug::Toggle_Light()
{
	if (player.light==0)
		player.light=15;
	else
		player.light=0;
}

void Debug::Menu()
{
	Window menuwin("Debug command", 30, 7, 27, 11, CH_GREEN, CH_WHITE);

	menuwin.Draw(
		"c - Test colors\n"
		"d - Current level data\n"
		"l - List objects\n"
		"o - Overview of world\n"
		"p - Program report\n"
		"r - Re-create level\n"
		"u - Unalive monsters\n"
		"v - View level map");

	const int ch=my_getch();

	switch (ch)
	{
		case 'c': debug_test_colors(); break;
		case 'd': View_Level_Data(); break;
		case 'l': List_Objects(); break;
		case 'o': Overview(); break;
		case 'p': uncover.Program_Data(); break;
		case 'r': Create_Level(); break;
		case 'u':
		{
			level_type *level=world->Get_Current_Level();
			level->crew.Unalive_Monsters();
			level->crew.Remove_Dead(level);
		}
		break;
		case 'v': View_Level(); break;
		default: break;
	}

	display->Redraw(world->Get_Current_Level());
}

void Debug::Message(const char *format, ...)
{
	static char buffer[80];
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(buffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(buffer, format, ap);
#endif
	va_end(ap);

	debug_messages->Add(buffer, C_BLUE, 0);
}

void Debug::Overview()
{
	clear_screen();

	world->Display_Data();

	wait_key();
}

void Debug::View_Level()
{
	Coord e;
	Coord saved;
	level_type *level=world->Get_Current_Level();
	Viewtile *vt=0;

	Coord c=player.Get_Location();
	c.x-=SCREEN_COLS/2;
	c.y-=(SCREEN_LINES-1)/2;

	bool looping=true;

	while (looping)
	{
		set_color(C_WHITE);

		//show the object or terrain name at the cursor
		e.Set_Location(c.x+SCREEN_COLS/2, c.y+(SCREEN_LINES-1)/2);

		const char *nimi=0;
		
		vt=gameview.Get(e);
		if (vt!=0)
		{
			if (vt->tonttu!=0)
			{
				nimi=vt->tonttu->Get_Name();
			}
			else if (vt->esine!=0)
			{
				nimi=vt->esine->Get_Name();		
			}
		}

		if (nimi==0)
		{
			const int et=level->Get_Terrain(e);
			nimi=terrains[et].desc;
		}
				
		mvprintw(0, 0, "Viewing level '%s' at %d, %d (%s) (t=teleport, d=tile data, x=exit)",
			world->Get_Level_Name(), e.x, e.y, nimi);
		clrtoeol(); //clear possible trails

		for (int y=1; y<=SCREEN_LINES; y++)
		{
			for (int x=0; x<=SCREEN_COLS; x++)
			{
				Coord d(c.x+x, c.y+y);
				move(y, x);

				vt=gameview.Get(d);
				bool displayed_object=false;
				
				if (vt!=0)
					displayed_object=vt->Show_Object();

				//show empty areas around level or terrain tile
				if (displayed_object==false)
				{
					const int tt=level->Get_Terrain(d);
					display->Draw_Terrain(tt, Viewtile::Visible);
				}
			}
		}

		gotoxy(SCREEN_COLS/2, (SCREEN_LINES-1)/2);
		
		const int k=my_getch();

		switch (k)
		{
			case 'x': looping=false; break;
			case 't':
				player.Jump_To(e);
				looping=false;
			break;
			case 'd':
				gotoxy(0, 1);
				set_color(C_WHITE);
				gameview.Show_Tile_Data(e);
				wait_key();
			break;
			default:
			{
				const int dir=Way::Get_From_Keycode(k);
				if (dir!=-1)
				{
					//this funky code section prevents cursor going over the
					//level edges, for visual reasons
					saved=e;
					e.Move_Direction(dir);
					if (level->Is_Outside(e))
						e=saved;
					else
						c.Move_Direction(dir);
				}
			}
			break;
		}
	}
}

void Debug::View_Level_Data()
{
	clear_screen();

	world->Display_Level_Data();

	wait_key();
}

void Debug::List_Objects()
{
	int key='?';
	level_type *level=world->Get_Current_Level();

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	while (key!='x')
	{
		clear_screen();
		my_printf(
			"List objects:\n"
			"1. Rooms\n"
			"2. Items\n"
			"3. Creatures\n"
			"4. Terrain\n"
			"x) Exit"
		);

		key=my_getch();
		bool listed=true;
		
		switch (key)
		{
			case '1': level->List_Rooms(); break;
			case '2': level->inv.List_Items(); break;
			case '3': level->crew.Full_Debug_List(); break;
			case '4': level->List_Terrain(); break;
			default: listed=false; break;
		}

		if (listed)
			wait_more();
	}
}

void Debug::View_Messages()
{
	debug_messages->Show_All("Debug messages");
}
