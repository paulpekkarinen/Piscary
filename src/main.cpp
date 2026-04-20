/**************************************************************************
 * main.cpp --                                                            *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 18.04.1999                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 25.9.2021 - 25.3.2026 Paul K. Pekkarinen

#include <cstdio>
#include "display.h"
#include "filework.h"
#include "caves.h"
#include "game.h"
#include "hiscore.h"
#include "input.h"
#include "lexicon.h"
#include "options.h"
#include "output.h"
#include "program.h"
#include "saldebug.h"
#include "script.h"
#include "textdata.h"

//global instances handled in the main() function
Debug *debug;
Program *program;
Text_Data *text_data; //long text documents
Script *texts; //text piece to show in messages etc.

void exit_curses();

int main(int argc, char *argv[])
{
	//if fails with directories
	if (check_directories()==false)
		return EXIT_FAILURE;
		
	CONFIGVARS.Reset();

	bool debug_mode=false;

	//HANDLE COMMAND LINE PARAMETERS
	if (argc>1)
	{
		for (int i=1; i<argc; i++)
		{
			if (!my_stricmp(argv[i], "-debug"))
				debug_mode=true;
			else if (!my_stricmp(argv[i], "-?"))
			{
				printf("The following command line parameters are available:\n"
					"-?      this help\n"
					"-debug  start the game in debug mode\n\n");
				return 0;
			}
			else
			{
				printf("Unknown command \"%s\" in parameter list!\n",
					argv[i]);
				return 0;
			}
		}
	}
		
	//initialize curses
	if (initscr()==0)
	{
		printf("Error: Can't initialize curses screen.");
		return EXIT_FAILURE;
	}

	if (has_colors())
		initcolorpairs();

	noecho();
	notimeout(stdscr, TRUE);
	keypad(stdscr, TRUE);

	//check current terminal size
	const int width=getmaxx(stdscr);
	const int height=getmaxy(stdscr);

	if (width<SCREEN_COLS || height<SCREEN_LINES)
	{
		clear_screen();
		
		my_printf(
			"This terminal is too small (%d x %d). Has to be 80 x 24 or larger.",
				width, height);
		my_getch();
		exit_curses();
		return EXIT_FAILURE;
	}

	//global instances
	debug=new Debug(debug_mode);
	program=new Program;

	//change to 'data' and load it
	if (change_directory("data")==false)
		return EXIT_FAILURE;

	//load text files
	debug->Message("Loading text data...");
	text_data=new Text_Data;
	texts=new Script;

	//return to home directory after loading data
	change_directory("..");

	debug->Message("Initialization ok!");

	hidecursor();
	display->Title();
	bool loop=true;

	while (loop)
	{
		//note: add loading game later
		switch (my_getch())
		{
			case 'p':
				Game.run(true); //start fast random game
				loop=false;
			break;
			case 'n':
				Game.run(false); //create new character
				loop=false;
			break;			
			case 'h':
				program->scoreboard->Showbest(0);
				display->Title();
			break;
			case 's':
				display->Story();
				display->Title();
			break;
			case 'q':
				loop=false;
			break;
			default: break;
		}
	}

	delete text_data;
	delete texts;
	delete program;
	delete debug;

	delete_lock_file();
	
	exit_curses();
		
	return EXIT_SUCCESS;
}

void exit_curses()
{
	echo();
	showcursor();
	endwin();	
}
