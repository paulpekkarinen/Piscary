/**************************************************************************
 * game.cpp --                                                            *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 25.9.2021 - 6.4.2026 Paul K. Pekkarinen

//#include <chrono>
#include "avatar.h"
#include "birth.h"
#include "caves.h"
#include "cmd.h"
#include "display.h"
#include "file.h"
#include "filework.h"
#include "foodstat.h"
#include "game.h"
#include "gameview.h"
#include "hiscore.h"
#include "input.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "optcont.h"
#include "options.h"
#include "output.h"
#include "program.h"
#include "script.h"
#include "storage.h"
#include "window.h"
#include "world.h"

using std::string;

/* global game data, must be saved */
gamedata Game;
int32u GAME_NOTIFYFLAGS;
World *world; //gamedata keeps track of the world instance

int32u gamedata::get_new_monster_id()
{
	int32u rv=monid;
	monid++;
	return rv;
}

void gamedata::advance_time(int ticks)
{
	passedtime.tick(ticks);
}

void gamedata::clean_up()
{
	delete world;
	world=0;
}

/* note: Food notes:
 *
 * Basic food gathering without any skill could be 5% of the corpse
 * size, rations/other "real" food items of course 100% all time even
 * without the skill.
 *
 * When "food gathering" skill increases:
 *
 * Skill
 * 0  .. 19    5% of the corpse
 * 20 .. 39   10%
 * 40 .. 59   15%
 * 60 .. 79   20%
 * 80 .. 89   25%
 * 90 .. 100  30%
 *
 * Skill increasing:
 *
 */

void gamedata::Eat_Passturns(level_type *level, item_def *item)
{
	noticeevents(level);

	real suc=0.03;
	int weight=0;

	if(item->group==FOOD_CORPSE)
	{
		/* test for food gathering skill */
		int skill=player.skills.check(SKILLGRP_GENERIC, SKILL_FOODGATHER);

		suc += 0.003 * skill;

		/* now test for skill levels */
		weight = (int32u)((real)item->weight * suc);

		/* modify skill marks */
		player.skills.modify_raise(SKILLGRP_GENERIC, SKILL_FOODGATHER, 1, 1);
	}
	else
		weight = item->weight;

	int movecount=1+weight/WEIGHT_KILO;

	player.Eat_Addnutr(item, weight);

	for(int i=0; i<movecount; i++)
	{
		player.Spend_Time(TIME_EATKILO);

		msg.setdelay(100);
		texts->Random_Message(Script::Eating_Sound);
		msg.setdelay(0);
		Passturn(true, false);
	}

	msg.vnewmsg(C_WHITE, "That was worth %4.2fkg of food.",
			(real)weight/WEIGHT_KILO);
}

void gamedata::initialize(bool fast)
{
	monid=0;
	state=Running;
	GAME_NOTIFYFLAGS=0;
	world=new World;

	//this is a hack needed for skill's modify_raise when initializing
	//the player character, to prevent displaying messages, but it's
	//relatively harmless since it's only used in one routine
	global_initmode = true;

	player.Initialize();
	Birth sotka;
	int cyearadd;

	if (fast)
		cyearadd=sotka.Fast_Action();
	else
		cyearadd=sotka.Player_Born(this);

	global_initmode = false;

	/* game was loaded if < 0 */
	if (cyearadd >= 0)
	{
		msg.newmsg(CH_WHITE,
			"%d years later, you suddenly find yourself "
			"in a small town...", cyearadd);
	}
}

void gamedata::Menu()
{
	Window menuwin("Game", 30, 10, 27, 9, CH_GREEN, CH_WHITE);

	menuwin.Draw(
		"o - Options\0"
		"s - Save and exit\0"
		"v - Show version\0"
		"---\0"
		"q - Quit without saving\0$");

	const int ch=my_getch();
	display->Redraw(world->Get_Current_Level());

	switch (ch)
	{
		case 'o': optcont->Edit(); break;
		case 's': Save(); break;
		case 'v': msg.newmsg(program->Get_Version()); break;
		case 'q': quit(); break;
		default: break;
	}
}

//Returns true if gameview was completely redrawn.
bool gamedata::noticeevents(level_type *level)
{
	bool rv=false;

	if ((GAME_NOTIFYFLAGS & GAME_DO_REDRAW))
	{
		display->Redraw(level);
		rv=true;
	}
	else
		display->Stats(false);

	GAME_NOTIFYFLAGS=0;
	return rv;
}

/*
 * This routine does one turn of game...
 * When player makes a move, it takes a certain amount of time, this time
 * is then divided into a number of time slots. The length of one time slot
 * is taken from player speed attribute.
 */
void gamedata::Passturn(bool playervis, bool foodsub)
{
	if (player.timetaken<BASE_TIMENEED)
		player.movecount++;

	if (player.nutr<=FOOD_FAINTING)
	{
		if (CONFIGVARS.foodwarn)
			msg.newmsg("You're starving!", CH_RED);
	}

	level_type *level=world->Get_Current_Level();

	/* if move didn't take any time, ... */
	if (player.timetaken>=BASE_TIMENEED)
	{
		const bool b=noticeevents(level);
		msg.notice();

		if (playervis && b==false)
			gameview.Show();

		return;
	}

	player.Check_Room(level);

	int spdadd=player.stat[STAT_SPD].Get();
	int ptime=player.stat[STAT_SPD].Get(); //note: should this be something else?

	if (world->Is_Outside() && !player.huntmode)
	{
		spdadd = spdadd / TIMEMOD_OUTSIDE;
	}

	/* calculate the number of used "time slots" taken by player move */
	int difft = ((int)BASE_TIMENEED - player.timetaken) / spdadd;

	/* advance world time and calendar */
	advance_time(ptime * difft);
	world->Advance_Time(ptime * difft);

	/* regenerate player hitpoints and handle other stuff */
	player.Handlestatus(level, difft);

	//note: wilderness time calculation check was commented out
	/*
	 * if dungeon==0 then player is in wilderness and there're are no
	 * monsters at the moment.
	 *
	 */

	 /*
	   if(player.dungeon!=0) {
	   while(player.timetaken<BASE_TIMENEED) {

	   handle_monsters(level, difft);

	   player.timetaken+=spdadd;
	   }
	   }
	 */
	level->crew.handle_monsters(level, difft);

	player.timetaken+=(difft*spdadd);
	const bool b=noticeevents(level);

	level->Remove_Dead_Monsters(); //check residue!

	if (playervis && b==false)
		gameview.Show();

	msg.notice();

	player.timetaken=BASE_TIMENEED;  // clear turntime before next turn
}

void gamedata::run(bool fast)
{
	showcursor();
	initialize(fast);

	attrset(A_NORMAL);

	world->Arrival();
	level_type *level=world->Get_Current_Level();
	teleport_player(level, false, true);
	display->Redraw(level);
	msg.notice();

	while (state==Running)
	{
		/* handle keyboard commands */
		read_cmd();

		//calculate time a turn takes
		//std::chrono::steady_clock::time_point StartTime=std::chrono::steady_clock::now();

		Passturn(true, true);

		/*using namespace std::chrono;
		steady_clock::time_point EndTime = steady_clock::now();
		auto Duration = duration_cast<microseconds>(EndTime-StartTime);
		debug->Message("Time: %lu ms", Duration.count());*/
	}

	if (state==End_Of_Game)
	{
		if (player.inv.Get_Weight() > 0)
		{
			if (confirm_yn("Do you want to see your stuff", true, true)==true)
				player.Show_Inventory();
		}

		clear_screen();

		program->scoreboard->Calculate();

		//note: how about the victory condition?
		my_printf("\n%s, you were killed by %s!\n\n",
			player.Get_Name(), get_player_killer());
		//      my_printf("You survived %d moves!\n", player.movecount);

		//scoreboard->Showbest();
		showmore(false, false);

		clear_screen();
	}

	clean_up();
}

void gamedata::show_time()
{
	passedtime.show_passed();
}

void gamedata::quit()
{
	if (confirm_yn("Really quit", false, true))
		state=Quit_And_Save;
}

void gamedata::Save()
{
	if (confirm_yn("Do you want to save", true, true)==false)
		return;

	if (switch_to_savedir()==false)
	{
		msg.add("Unable to create or access 'save' directory.", CH_RED);
		return;
	}

	Tar_Ball tb;
	tb.Put(state);
	tb.Put(monid);
	passedtime.save(tb);
	world->Save(tb);
	player.Save(tb);

	change_directory("..");

	msg.newmsg("Game saved.", C_WHITE);
	showmore(false, false);

	state=Quit_And_Save;
}

int gamedata::Load(const char *plrname)
{
	//when loading assumes that save directory exists, so don't use
	//'switch_to_savedir' here
	if (change_directory("save")==false)
		return File::Directory_Error;

	string fn(plrname);
	fn.append(".sav");

	File savefile(fn);
	if (savefile.Open()==false)
		return File::Cant_Open;

	Tar_Ball tb;
	tb.Load(&savefile);

	state=tb.Get_Next_Value();
	monid=tb.Get_Next_Unsigned();
	passedtime.load(tb);
	world->Load(tb);
	level_type *curle=world->Get_Current_Level();
	player.Load(tb, curle);

	//delete savefile after loading
	if (deletefile(fn.c_str())==false)
		my_printf("\nUnable to delete savefile %s, weird!\n\n", fn.c_str());

	change_directory("..");

	GAME_NOTIFYFLAGS=0;
	state=Running;

	return File::Success;
}
