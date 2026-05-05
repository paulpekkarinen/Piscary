/**************************************************************************
 * codex.cpp --                                                           *
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

//Refactored 23.6.2025 - 7.7.2025 Paul K. Pekkarinen

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include "avatar.h"
#include "caves.h"
#include "codex.h"
#include "output.h"
#include "input.h"
#include "world.h"

using namespace std::chrono_literals;

void clear_flag_bit(int16u &var, int16u lippu)
{
	var&=~lippu;
}

void delay(int delaycount)
{
	std::chrono::seconds d(delaycount);
	std::this_thread::sleep_for(d);	
}

void set_flag_bit(int16u &var, int16u lippu)
{
	var|=lippu;
}

//Select items from ground or inventory.
int inv_selectsource(int s2_x, int s2_y, int type)
{
	/* check if s1 contains items of 'type', recursively */
	const int s1_count = player.inv.Count_Items(type, -1, -1, true);

	/* check if s2 (ground) contains items of 'type', recursively */
	level_type *gnd=world->Get_Current_Level();
	const int s2_count = gnd->inv.Count_Items(type, s2_x, s2_y, true);

	if (s1_count && s2_count)
	{
		if (confirm_yn("Do you want to browse the ground (y) or your "
			"inventory (n)", true, true))
			return inventory::Ground;
		else
			return inventory::Backpack;
	}

	if (s1_count)
		return inventory::Backpack;
	if (s2_count)
		return inventory::Ground;

	return inventory::Nothing;
}

void panic_exit(const char *error_text)
{
	showcursor();
	printf("Fatal error!\n%s\n", error_text);
	exit(EXIT_FAILURE);
}
