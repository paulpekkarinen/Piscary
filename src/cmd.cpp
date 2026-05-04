/**************************************************************************
 * cmd.cpp --                                                             *
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

//Refactored 25.9.2021 - 6.9.2025 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstring>
#include "aim.h"
#include "avatar.h"
#include "caves.h"
#include "cmd.h"
#include "consume.h"
#include "display.h"
#include "drill.h"
#include "game.h"
#include "input.h"
#include "inquire.h"
#include "log.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "pack.h"
#include "ranged.h"
#include "saldebug.h"
#include "skills.h"
#include "talk.h"
#include "test.h"
#include "textdata.h"
#include "textview.h"
#include "trade.h"
#include "use.h"
#include "way.h"
#include "window.h"
#include "world.h"

using std::string;
using namespace cmd;

/* local prototypes */
int check_cmdtable(const int *cmdline, int keynum);
void cmd_help();
void cmd_showkeys();
int compare_cmd(const void* cmd1, const void* cmd2);
void run_command(playerinfo *plr, const int command);

#define CMD_NOCMD -1
#define CMD_MULTI -2

const char *command_categories[]=
{
	"Navigation",
	"Items",
	"Adventure",
	"Traits",

	"Actions",
	"Interact",
	"View",
	"Debug",

	"Editor"
};

/*
 * This is a complete list of player usable commands.
 *
 * Name is a plain string describing the command
 * Key sequence can contain a sequence of 1-4 keys needed to execute
 * the command
 */
Command_Data cmdtable[Amt_Of_Commands]=
{
	//navigation
	{"Move north", {KEY_UP, 0, 0, 0, 0}, Navigation, false},
	{"Move northeast", {'9', 0, 0, 0, 0}, Navigation, false},
	{"Move east", {KEY_RIGHT, 0, 0, 0, 0}, Navigation, false},
	{"Move southeast", {'3', 0, 0, 0, 0}, Navigation, false},
	{"Move south", {KEY_DOWN, 0, 0, 0, 0}, Navigation, false},
	{"Move southwest", {'1', 0, 0, 0, 0}, Navigation, false},
	{"Move west", {KEY_LEFT, 0, 0, 0, 0}, Navigation, false},
	{"Move northwest", {'7', 0, 0, 0, 0}, Navigation, false},

	{"Walk north", {'w', '8', 0, 0, 0}, Navigation, false},
	{"Walk northeast", {'w', '9', 0, 0, 0}, Navigation, false},
	{"Walk east", {'w', '6', 0, 0, 0}, Navigation, false},
	{"Walk southeast", {'w', '3', 0, 0, 0}, Navigation, false},
	{"Walk south", {'w', '2', 0, 0, 0}, Navigation, false},
	{"Walk southwest", {'w', '1', 0, 0, 0}, Navigation, false},
	{"Walk west", {'w', '4', 0, 0, 0}, Navigation, false},
	{"Walk northwest", {'w', '7', 0, 0, 0}, Navigation, false},

	{"Enter downstairs", {'>', 0, 0, 0, 0}, Navigation, true},
	{"Enter upstairs", {'<', 0, 0, 0, 0}, Navigation, true},

	//items
	{"Equip items/View equipment", {'q', 0, 0, 0, 0}, Items, true},
	{"Display inventory", {'i', 0, 0, 0, 0}, Items, true},
	{"Get item", {',', 0, 0, 0, 0}, Items, false},
	{"Drop item", {'d', 0, 0, 0, 0}, Items, true},

	//adventure
	{"Game menu", {'G', 0, 0, 0, 0}, Adventure, true},
	{"Quests", {':', 'q', 0, 0, 0}, Adventure, true},
	{"Character status", {'@', 0, 0, 0, 0}, Adventure, true},
	{"Journey log", {'J', 0, 0, 0, 0}, Adventure, true},
	{"Weather report", {':', 'w', 0, 0, 0}, Adventure, false},

	//traits
	{"Change your tactics", {'T', 0, 0, 0, 0}, Traits, false},
	{"Apply skills", {'S', 0, 0, 0, 0}, Traits, false},
	{"Apply quickskill number 0", {':', '0', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 1", {':', '1', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 2", {':', '2', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 3", {':', '3', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 4", {':', '4', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 5", {':', '5', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 6", {':', '6', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 7", {':', '7', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 8", {':', '8', 0, 0, 0}, Traits, false},
	{"Apply quickskill number 9", {':', '9', 0, 0, 0}, Traits, false},

	//actions
	{"Push", {'p', 0, 0, 0, 0}, Actions, false},
	{"Search", {'s', 0, 0, 0, 0}, Actions, false},
	{"Toggle search mode", {':', 's', 0, 0, 0}, Actions, false},
	{"Look at direction", {'l', 0, 0, 0, 0}, Actions, false},
	{"Look around", {'L', 0, 0, 0, 0}, Actions, true},
	{"Read", {'r', 0, 0, 0, 0}, Actions, true},
	{"Open door", {'o', 0, 0, 0, 0}, Actions, false},
	{"Close door", {'c', 0, 0, 0, 0}, Actions, false},
	{"Cast a spell", {'Z', 0, 0, 0, 0}, Actions, false},
	{"Eat food", {'e', 0, 0, 0, 0}, Actions, true},
	{"Throw/Shoot", {'t', 0, 0, 0, 0}, Actions, true},
	{"Rest (pass one turn)", {'.', 0, 0, 0, 0}, Actions, false},

	//interact
	{"Talk with someone", {'C', 0, 0, 0, 0}, Interact, false},
	{"Pay your bill", {'P', 0, 0, 0, 0}, Interact, false},
	{"Check your bill", {':', 'b', 0, 0, 0}, Interact, false},

	//view
	{"Redraw screen", {MYKEY_CTRLR, 0, 0, 0, 0}, View, true},
	{"Display previous message", {MYKEY_CTRLP, 0, 0, 0, 0}, View, false},
	{"Display message buffer", {'m', 0, 0, 0, 0}, View, true},
	{"Display help menu", {'?', 0, 0, 0, 0}, View, true},
	{"Display keybindings", {'K', 0, 0, 0, 0}, View, true},
	{"Display error log", {':', 'e', 0, 0, 0}, View, true},

#ifdef saladir_debug
	//debug commands
	{"Debug: Light on", {MYKEY_CTRLH, 0, 0, 0, 0}, Debugging, true},
	{"Debug: Identify item", {MYKEY_CTRLI, 0, 0, 0, 0}, Debugging, true},
	{"Debug: Quit", {'Q', 0, 0, 0, 0}, Debugging, false},
	{"Debug: Test something", {MYKEY_CTRLT, 0, 0, 0, 0}, Debugging, true},
	{"Debug: Messages", {'M', 0, 0, 0, 0}, Debugging, true},
	{"Debug: Command menu", {MYKEY_CTRLD, 0, 0, 0, 0}, Debugging, true},
#endif

	//editor commands (editor was removed)
/*	{"Editor: ON/OFF", {MYKEY_F10, 0, 0, 0, 0}, editor_onoff, Edit},
	{"Editor: Next terrain", {MYKEY_F1, 0, 0, 0, 0}, editor_nextterr, Edit},
	{"Editor: Prev terrain", {MYKEY_F2, 0, 0, 0, 0}, editor_prevterr, Edit},
	{"Editor: Dramode", {MYKEY_F3, 0, 0, 0, 0}, editor_drawmode, Edit},
	{"Editor: Set terrain", {' ', 0, 0, 0, 0}, editor_set, Edit},
	{"Editor: Set map visible", {MYKEY_F7, 0, 0, 0, 0}, editor_visible, Edit},
	{"Editor: Hide map", {MYKEY_F8, 0, 0, 0, 0}, editor_visibleoff, Edit},
	{"Editor: Clear map with current terrain", {MYKEY_F9, 0, 0, 0, 0}, editor_clear, Edit},
	{"Editor: Save map", {MYKEY_F5, 0, 0, 0, 0}, editor_save, Edit},
	{"Editor: Load map", {MYKEY_F6, 0, 0, 0, 0}, editor_load, Edit}*/
};

int compare_cmd(const void* cmd1, const void* cmd2)
{
	//sort first by category, then alphabetic order
	const int c1=((Command_Data*)cmd1)->category;
	const int c2=((Command_Data*)cmd2)->category;

	if (c1<c2)
		return -1;
	else
	{
		if (c1>c2) return 1;
	}

	return strcmp(((Command_Data*)cmd1)->name, ((Command_Data*)cmd2)->name);
}

int check_cmdtable(const int *cmdline, int keynum)
{
	for (int indx=0; indx<Amt_Of_Commands; indx++)
	{
		bool cmdmatch=true;
		for (int i=0; i<keynum+1; i++)
		{
			if (cmdline[i]!=cmdtable[indx].keys[i])
				cmdmatch=false;
		}

		if (cmdmatch)
		{
			if (cmdtable[indx].keys[keynum+1]==0)
				return indx;
			else
				return CMD_MULTI;
		}
	}

	return CMD_NOCMD;
}

/* read one full command, even multikey command */
void read_cmd()
{
	int mk=0;
	bool cmdnotfull=true;

	//static char txt_cmdline[128]={0};
	static int cmdline[CMD_MAXLEN+1]={0};

	//txt_cmdline[0]=0;
	string cmdstr;

	while (cmdnotfull)
	{
		const int ch=my_getch();
		msg.update();
		cmdstr.append(get_keyname(ch));
		cmdline[mk]=ch;

		const int ci=check_cmdtable(cmdline, mk);
		clearline(0); //note: is this needed when msg.update is called?
		gotoxy(0, 0);
		set_color(C_RED);

		if (ci==CMD_NOCMD)
		{
			cmdnotfull=false;
			mk=0;
			my_printf("\"%s\" Unknown Cmd.", cmdstr.c_str());
		}
		else if (ci==CMD_MULTI)
		{
			mk++;
			my_printf("Cmd \"%s\"", cmdstr.c_str());
		}
		else
		{
			cmdnotfull=false;
			mk=0;
			run_command(&player, ci);
		}

		if (mk>CMD_MAXLEN)
		{
			cmdnotfull=false;
			mk=0;
		}
	}
}

void run_command(playerinfo *plr, const int command)
{
	level_type *level=c_level;

	//check movement commands first
	if (command>=Move_North && command<=Move_Northwest)
	{
		moveplayer(plr, Way::Get_Command_Direction(command), level, false);
		return;
	}

	//then check walking
	if (command>=Walk_North && command<=Walk_Northwest)
	{
		repeatmove(plr, Way::Get_Command_Direction(command), level);
		return;
	}

	//check rest of the commands
	switch (command)
	{
		case Enter_Downstairs:
			if (world->Player_Go_Down(level)==false)
				msg.newmsg("You start foolishly digging your way down...");
		break;
		case Enter_Upstairs:
			if (world->Player_Go_Up(level)==false)
				msg.newmsg("You jump very hard...");
		break;

		case Equip_Item: player.equips.player_equip(); break;
		case Show_Inventory: player.Show_Inventory(); break;
		case Pick_Item: pick_up_item(plr, level); break;
		case Drop_Item: drop_selected(plr); break;

		case Game_Menu: Game.Menu(); break;
		case Show_Quests: player.quests.showall(); break;
		case Show_Character_Status: display->Player_Status(player); break;
		case Show_Journey_Log: display->Journey_Log(); break;
		case Show_Weather: world->Weather_Report(); break;

		case Change_Tactics: player.Changetactics(); break;
		case Apply_Skill: use_skill(plr, level, SKILLGRP_ALL, -1); break;
		case Apply_Quickskill_0:
		case Apply_Quickskill_1:
		case Apply_Quickskill_2:
		case Apply_Quickskill_3:
		case Apply_Quickskill_4:
		case Apply_Quickskill_5:
		case Apply_Quickskill_6:
		case Apply_Quickskill_7:
		case Apply_Quickskill_8:
		case Apply_Quickskill_9:
		{
			//get quick skill ids 0-9
			use_quickskill(plr, level, command-Apply_Quickskill_0);
		}
		break;

		case Push_Something: player_push(plr, level, 0); break;
		case Search: search_surroundings(plr, level, false); break;
		case Toggle_Search: player.Switch_Searchmode(); break;
		case Look: lookdir(plr, level); break;
		case Look_Around:
		{
			Aim a(level);
			a.Select();			
		}
		break;
		case Read: read_scroll(plr, level); break;
		case Open_Door: open_door(plr, level); break;
		case Close_Door: close_door(plr, level); break;
		case Cast_Spell: use_skill(plr, level, SKILLGRP_MAGIC, -1); break;
		case Eat_Food: eat_food(plr, level); break;
		case Throw_Or_Shoot: ranged_attack(plr, level); break;
		case Rest: moveplayer(plr, DIR_SELF, level, false); break;

		case Talk: chat(plr); break;
		case Pay_Bill: paybill(plr, level); break;
		case Check_Bill: player.Check_Bill(); break;

		case Redraw_Screen:
			//command data sets redraw for this, no need for manual call
		break;
		case Previous_Message: msg.showlast(); break;
		case Show_Message_Buffer: msg.showall(); break;
		case Show_Help_Menu: cmd_help(); break;
		case Show_Keys: cmd_showkeys(); break;
		case Show_Error_Log: diary.Read(); break;

		case Cheat_Light: debug->Toggle_Light(); break;
		case Cheat_Identify_Item: debug->Identify(); break;
		case Debug_Quit: Game.Set_State(gamedata::Debug_Quit); break;
		case Debug_Test: run_current_test(); break;
		case Debug_Messages: debug->View_Messages(); break;
		case Debug_Menu: debug->Menu(); break;

		default:
			msg.newmsg(C_RED, "Unknown command id %d.", command);
		break;
	}

	if (cmdtable[command].redraw_view)
		GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}

void cmd_help()
{
	Window menuwin("Help", 30, 10, 27, 7, CH_GREEN, CH_WHITE);

	menuwin.Draw(
		"k - Keybindings\0"
		"m - Manual\0$");

	const int ch=my_getch();
	display->Redraw(world->Get_Current_Level());

	switch (ch)
	{
		case 'k': cmd_showkeys(); break;
		case 'm': text_data->View(Text_Data::Manual); break;
		default: break;
	}
}

void cmd_showkeys()
{
	string s;

	/* create a text file containing keyinfo */
	s.append("\001Legend of Saladir keyboard commands\n");

	s.append("\n\007Commands like '\001C-a\007' means that you need "
		"to press keys \002CTRL \007and \002a\007"
		" together. Some commands may require multiple keypresses, ie. "
		"command \"\001:?k\007\" means that you'll have to press first "
		"\002:\007, then \002? \007and finally \002k \007to "
		"execute the command.\n\n");
	s.append("Commands starting with '\001w\007' will repeat several times, "
		"the repeat count can be defined in options with variable "
		"\003REPEATCOUNT\007.\n\n");

	static char keystr[128]={0};
	static char outstr[256]={0};
	int cat=-1;

	for (int indx=0; indx<Amt_Of_Commands; indx++)
	{
		//display the category
		if (cat!=cmdtable[indx].category)
		{
			cat=cmdtable[indx].category;
			if (cat>0) s.append("\n");
			s.append("~ ");
			s.append(command_categories[cat]);
			s.append(" ~\n");
		}

		keystr[0]=0;
		for (int i=0; i<CMD_MAXLEN; i++)
		{
			if (cmdtable[indx].keys[i]!=0)
				strcat(keystr, get_keyname(cmdtable[indx].keys[i]));
		}
		sprintf(outstr, "\002%-9s \007%s\n", keystr, cmdtable[indx].name);

		s.append(outstr);
	}

	s.append("\nCommands are case sensitive.\n\n");

	//finally view the text
	viewtext(s.c_str());
}
