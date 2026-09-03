/**************************************************************************
 * program.cpp --                                                         *
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

//Refactored 14.7.2022 - 27.3.2026 by Paul K. Pekkarinen

#include "amount.h"
#include "dice.h"
#include "display.h"
#include "filework.h"
#include "hiscore.h"
#include "optcont.h"
#include "options.h"
#include "program.h"
#include "saldebug.h"
#include "skills.h"

bool global_initmode;
Amount mucho; //how many of something there are

//global instances handled by the program class
Display *display;
Optcont *optcont;

char Program::SALADIR_VERSTRING[] =
"Legend of Saladir " PROC_VERSION PROC_PLATFORM " (C)1997/98 Erno Tuomainen";

Program::Program()
{
	display=new Display;

	initdice();

	scoreboard=new Scoreboard;
	optcont=new Optcont;

	//load options and highscores from 'save' folder (if it exists)
	if (change_directory("save"))
	{
		optcont->Load();

		debug->Message("Loading highscores...");
		scoreboard->Load();

		change_directory("..");
	}
	else
		debug->Message("Save directory not yet created. Loaded nothing from there.");

	debug->Message("Initialize skills...");
	init_skills();

	debug->Message("Initializing amounts...");
	mucho.Initialize();
}

Program::~Program()
{
	//save options and scoreboard on exit, if they are changed
	if (optcont->Is_Changed() || scoreboard->Is_Changed())
	{
		bool err=false;

		if (switch_to_savedir()==false)
			err=true;
		else
		{
			if (optcont->Save()==false)
				err=true;

			if (scoreboard->Save()==false)
				err=true;

			change_directory("..");
		}

		if (err)
			display->Error("Can't save the configuration or highscores.\n");
	}

	delete scoreboard;
	destroydice();
	delete display;
	delete optcont;
}
