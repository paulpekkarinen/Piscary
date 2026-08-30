/**************************************************************************
 * test.cpp --                                                            *
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

//Refactored 7.8.2024 - 10.9.2025 by Paul K. Pekkarinen

#include <format>
#include "avatar.h"
#include "dice.h"
#include "game.h"
#include "input.h"
#include "items.h"
#include "message.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "school.h"
#include "script.h"
#include "spawner.h"
#include "test.h"
#include "window.h"
#include "world.h"

using std::format;
using std::string;

void test_hitting();
void test_keys();
void test_random_generator();
void test_scripts();
void test_studying();
void test_valuables();
void test_window();

void run_current_test()
{
	test_studying();

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}

void test_hitting()
{
	//note: can't use this with nulls
	//damage_issue(c_level, 0, 0, ELEMENT_FIRE, 12, HPSLOT_BODY,
	//	"The fireball hits!");
	//msg.newmsg("You miss.");
}

void test_keys()
{
	int k='?';
	msg.add("Test keys, Esc to exit.", CH_GREEN);

	while (k!=KEY_ESC)
	{
		k=my_getch();

		string s=format("{} ({})", get_keyname(k), k);

		msg.add(s.c_str(), CH_GREEN);
	}
}

void test_random_generator()
{
	const float f=randrealnum(-1.0f, 1.0f);
	msg.newmsg(C_WHITE, "Random number %f.", f);
}

void test_valuables()
{
	Spawner spw(world->Get_Current_Level());
	Coord c=player.Get_Location();

	spw.Create_Item(c, IS_MONEY, -1, RANDU(128), -1);
}

void test_scripts()
{
	//texts->Random_Message(Script::Eating_Sound);
	texts->Debug(Script::Sparhawk_Comments);
}

void test_studying()
{
	School sch(false);
	sch.Study(player);
}

void test_window()
{
	Window ikkuna("Window", 3, 3, 25, 10, CH_GREEN, CH_WHITE);

	ikkuna.Draw("Testing window text output routine to see if the text "
	"fits in the window and how it looks if the text is too long "
	"for the window. Just a little bit more text here.");

	wait_key();
}
