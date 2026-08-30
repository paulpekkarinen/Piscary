/**************************************************************************
 * birth.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 4.9.2021 - 9.9.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "birth.h"
#include "body.h"
#include "cornucop.h"
#include "creature.h"
#include "dice.h"
#include "display.h"
#include "file.h"
#include "game.h"
#include "input.h"
#include "lexicon.h"
#include "magic.h"
#include "names.h"
#include "output.h"
#include "program.h"
#include "roleplay.h"
#include "school.h"
#include "score.h"
#include "skills.h"
#include "world.h"

using std::string;

#define NUM_AGEPREFIX 4
const char *txt_ageprefix[]=
{
   "At the age of %d",
   "When you were %d years old",
   "During your studies at the age of %d",
   "You were %d years old when",
   NULL
};

const char *txt_events[]=
{
   "your distant uncle died and as his only relative\nyou received his "
     "fortune of %d gold!",
   "a black plague stroke the town you were studying\nat, evetually you "
     "got sick and were sick for %d months and lost %d %s.",
   "a group of bandits raided your town. While\nhelping your fellow "
     "citizens to defend against the bandits\nyou were wounded but "
     "gained %d %s!",
   NULL
};

const char welcometext[]=
"\007Welcome to the land of \002Salmorria\007, stranger. You're just "
"a few steps away from the adventure, gold and fortune. You're ready "
"to go after the Legend of Saladir...\n"
"But before anything, please select a name for yourself or press enter "
"and just let the fate decide...\n\n"
"\0";

const char txt_prebirth[]=
"\7Before you can go adventuring, you must first create a character. "
"In this version you have two choices for creating your brave adventurer."
"\n\n\t\1a\7) Create your own character\n"
"\t\1b\7) Create a random character\n\n";

const char txt_titlesel[]=
"\007Now you can think a title for you. Think of something which describes "
"yourself. An example, if your name is \"\002Melthor\007\", then you could be "
"\"\002Melthor \003the great.\007\" or \"\002Melthor \003the magician\007\". "
"You can leave this blank and use default \"\003the adventurer\007\" "
"instead.\n\n";

const char txt_primaryweapon[]=
"\007Now it's time to pick your primary weapon skill. You will learn this "
"skill all the way up to\001 25% \007(out of 100%) and you will also get "
"a starting weapon which needs this skill.";

const char txt_secondaryweapon[]=
"\007Now pick the secondary weapon skill. You will have a\001 10% "
"\007chance on the "
"skill you now select. If you pick the same skill as the primary skill, "
"you will increase it by\001 10%\007.";

const char txt_statedit[]=
"\007Now edit your attributes, these attributes tell a lot about you. "
"How strong you are, how intelligent you are, and so on. "
"An average value is \00150 \007which means that you gain no bonuses but "
"don't get any negative effects either.\n\n";

const char txt_statedit_auto[]=
"\007In automatic mode, you can not decrease your already calculated "
"attributes, but you have some points left to distribute as you wish.\n\n";

const char txt_statedit2[]=
"\007Use \002UP/DOWN \007or \002A/Z \007keys to select attribute for "
"editing.\n"
"\002LEFT/RIGHT \007and \002-/+ \007keys will modify the value of the "
"highlighted attribute.\n"
"When all points used, press \002ENTER \007to continue, or press \002Q \007to "
"force quit (and possibly) leave unused points to the pool.\n\n";

Birth::Birth() noexcept
	: used_effects{0}
{

}

void Birth::Ask_Skills() //note: commented out
{
   /* first two weapon skills */
/*
   skill_modify(&player.skills, SKILLGRP_WEAPON,
		skill_listselect(SKILLGRP_WEAPON, txt_primaryweapon),
		25, true);

   skill_modify(&player.skills, SKILLGRP_WEAPON,
		skill_listselect(SKILLGRP_WEAPON, txt_secondaryweapon),
		10, true);

   skill_modify(&player.skills, SKILLGRP_MAGIC,
		skill_listselect(SKILLGRP_MAGIC, "SPELL"),
		10, true);
*/

//   skill_modify(&player.skills, SKILLGRP_MAGIC,
//		SPELL_TELEPORT, 50, true);
//   skill_modify(&player.skills, SKILLGRP_MAGIC,
//		SPELL_IDENTIFY, 50, true);
}

void Birth::Random_Stats(statpack *stats)
{
	int i;

	/* fill pool with enough points to distribute between stats */
	int statpool = (STAT_BASICARRAY * 40)+RANDU(STAT_BASICARRAY * 10) ;

	/* clear stats first */
	for (i=0; i<STAT_BASICARRAY; i++)
		stats[i].Set_Initial(0, STATMAX_GEN, 1);

	while (statpool > 0)
	{
		i=RANDU(STAT_BASICARRAY);
		stats[i].Change_Initial(1, true);
		statpool--;
	}

	/* set luck */
	stats[STAT_LUC].Set_Initial(
		(throwdice(1, STATMAX_LUCK/2, 0) +
			throwdice(1, STATMAX_LUCK/2, 0)), STATMAX_LUCK, 0);
}

void Birth::Randomeffect(int age)
{
	if (throwdice(1, 100, 0) > 20)
		return;

	int prefix=RANDU(NUM_AGEPREFIX);
	int effect=RANDU(NUM_RANDEFFECT);

	/* assure that the effect is not yet used */
	int amount=0;
	while (used_effects[effect]!=0 && (amount++ < 100))
		effect=RANDU(NUM_RANDEFFECT);

	/* if it's still used, return */
	if (used_effects[effect]>0)
		return;

	used_effects[effect]=1;

	string s("\n");
	s.append(txt_ageprefix[prefix]);
	s.append(" ");
	s.append(txt_events[effect]);
	s.append("\n");

	if (effect==RANDEFF_INHERIT)
	{
		amount=50+RANDU(200);
		player.inv.Add_Gold(amount);
		my_printf(s.c_str(), age, amount);
	}
	else if (effect==RANDEFF_PLAGUE)
	{
		amount=(5+RANDU(10));
		const int st=RANDU(STAT_BASICARRAY);
		player.stat[st].Change_Initial(amount, true);
		amount=-amount;

		Stat stype(st);
		my_printf(s.c_str(), age, 1+RANDU(5), amount, stype.Get_Short_Name());
	}
}

void Birth::Ask_Stats(bool automatic)
{
	int statpool=10, cstat;
	int stat_tx=0, cy=0;

	clear_screen();

	if (!automatic)
		my_wordwraptext(txt_statedit, 1, SCREEN_LINES, 1, SCREEN_COLS);
	else
		my_wordwraptext(txt_statedit_auto, 1, SCREEN_LINES, 1, SCREEN_COLS);

	/* init randomly */
	Random_Stats(player.stat);

	const int stat_ty=get_cursor_y();

	for (int i=0; i<7; i++)
	{
		Stat stype(i);

		set_color(C_WHITE);
		my_printf("%-20s: ", stype.Get_Name());
		stat_tx=get_cursor_x();
		set_color(C_YELLOW);
		cstat=player.stat[i].Get();
		my_printf("%2d (%10s)\n", cstat, txt_statrating[cstat/10]);
	}

	my_wordwraptext(txt_statedit2, get_cursor_y()+1, SCREEN_LINES, 1, SCREEN_COLS);

	int desc_y=get_cursor_y();

	my_setcolor(C_WHITE);
	drawline(desc_y, '=');
	gotoxy(2, desc_y++);
	set_color(CH_WHITE);
	my_printf("[ Description ]");

	gotoxy(stat_tx+15, stat_ty);
	set_color(C_WHITE);
	my_printf("Left: %3d", statpool);
	gotoxy(stat_tx, stat_ty+cy);
	set_color(CH_RED);

	const int cy_stat=player.stat[cy].Get();
	my_printf("%2d (%10s)", cy_stat, txt_statrating[cy_stat/10]);

	while (1)
	{
		int ch=my_getch();

		cstat=player.stat[cy].Get();

		gotoxy(stat_tx, stat_ty+cy);
		set_color(C_YELLOW);
		my_printf("%2d (%10s)", cstat,
			txt_statrating[cstat/10]);

		if ((ch==KEY_DOWN || ch=='z' || ch=='2') && cy<6)
		{
			cy++;
			Stat stype(cy);
			stype.Show_Description(desc_y);
		}
		else if ((ch==KEY_UP || ch=='a' || ch=='8') && cy>0)
		{
			cy--;
			Stat stype(cy);
			stype.Show_Description(desc_y);
		}
		else if ((ch=='+' || ch==KEY_RIGHT || ch=='6') && statpool>0)
		{
			if (cy==STAT_CHA || cy==STAT_INT || cy==STAT_WIS)
				continue;

			if (cstat < STATMAX_GEN)
			{
				statpool--;
				player.stat[cy].Change_Initial(1, true);
			}
		}
		else if (!automatic && (ch=='-' || ch==KEY_LEFT || ch=='4') && cstat>30)
		{
			if (cy==STAT_CHA || cy==STAT_INT || cy==STAT_WIS)
				continue;

			statpool++;
			player.stat[cy].Change_Initial(-1, true);
		}
		else if ((ch==' ' || ch==KEY_ENTER))
		{
			if (statpool==0)
				break;
			else
			{
				gotoxy(stat_tx+16, stat_ty+STAT_BASICARRAY);
				set_color(C_RED);
				my_printf("You have undistributed points!",
					statpool);
			}
		}
		else if (ch=='q' || ch=='Q')
			break;

		gotoxy(stat_tx+16, stat_ty);
		set_color(C_WHITE);
		my_printf("Left: %3d", statpool);

		gotoxy(stat_tx+16, stat_ty+1);
		my_printf("Carrying capasity = %4.2f", (real)player.Calc_Carryweight()/1000);

		gotoxy(stat_tx, stat_ty+cy);
		set_color(CH_RED);
		cstat=player.stat[cy].Get();
		my_printf("%2d (%10s)", cstat, txt_statrating[cstat/10]);
	}
}

string Birth::Give_Random_Name()
{
	string s;
	random_name(s, NAMEMAX-1);
	uppercase_first_letter(s);
	return s;
}

//Generate player without asking anything but maybe name
//in the release version.
int Birth::Fast_Action()
{
	player.m.name=Give_Random_Name();
	player.m.desc="the wizard";
	player.m.gender=random_number(SEX_MALE, SEX_FEMALE);
	Random_Stats(player.stat);

	School sch(true);
	const int yearstook=sch.Study(player);

	Randomeffect(yearstook);

	//note: skills are not set here in any way, but they should be set
	//possibly in this place, before sending skills to 'Initpack'

	//init inventory
	Cornucopia cc(player.inv);
	cc.Initpack(player.skills);

	//note: this is also in playerinfo::Initialize, figure this out later
	const int pr=player.Get_Race();
	roleplay.Calculate_HP(player.hpp, npc_races[pr].hp_base, pr);
	player.Calculate_Totalhp();

	player.stat[STAT_SPD].initial= npc_races[pr].stats.SPD +
		roleplay.Get_Dex_Speed(player.stat[STAT_DEX].Get());

	return yearstook;
}

/************************************/
/* Here we give birth to the player */
/************************************/
int Birth::Player_Born(gamedata *mygame)
{
	int yearstook=0;

	clear_screen();
	my_printf(program->Get_Version());
	my_wordwraptext(welcometext, 3, SCREEN_LINES, 1, SCREEN_COLS);

	set_color(C_YELLOW);
	my_printf("What is your name? ");

	set_color(C_GREEN);
	string plrname=get_string(true, NAMEMAX-1);

	if (plrname.empty())
	{
		plrname=Give_Random_Name();
		set_color(C_WHITE);
		my_printf("\nSo be it, your name is \"%s\" today.\n", plrname.c_str());
	}
	else
	{
		const int g=mygame->Load(plrname.c_str());

		if (g==File::Success)
		{
			world->Display_Return_Message(player.Get_Name());
			showmore(false, false);
			return -1; //-1 = game found and loaded
		}
		else
		{
			//if can't open, the file simply doesn't exist, but in other errors
			//use failsafe code
			if (g!=File::Cant_Open)
			{
				my_printf("ERROR: Failed to restore the save file of %s!\n", plrname.c_str());
				plrname=Give_Random_Name(); //use random name
				my_printf("From now on you are known as %s.\n", plrname.c_str());
			}
		}
	}

	my_printf("\n");
	my_setcolor(C_WHITE);

	player.m.name=plrname; //set the name of player

	/* no previous save files exist */
	set_color(C_YELLOW);

#ifdef debug_birth
	int sel=0;
	player.m.gender=random_number(SEX_MALE, SEX_FEMALE);
#else
	string resp(player.Get_Name());
	resp.append(", are you (m)ale or (f)emale");

	int sel=get_response(resp.c_str(), "mMfF");
	if (sel==0 || sel==1)
		player.m.gender=SEX_MALE;
	else
		player.m.gender=SEX_FEMALE;
#endif

	clear_screen();
	my_wordwraptext(txt_prebirth, 1, SCREEN_LINES, 1, SCREEN_COLS);
	sel=get_response("What will it be?", "aAbB");

	bool automode;
	if (sel==0 || sel==1)
		automode=false;
	else
		automode=true;

	Ask_Stats(automode);

	clear_screen();
	world->Show_Birth_Time();

	/* clear randeffect table */
	for (int num=0; num<NUM_RANDEFFECT; num++)
		used_effects[num]=0;

	School sch(automode);
	yearstook=sch.Study(player);

	Randomeffect(yearstook);
	Ask_Skills();

	clear_screen();
	my_wordwraptext(txt_titlesel, get_cursor_y(), SCREEN_LINES, 0, SCREEN_COLS);

#ifdef debug_birth
	player.m.desc="the bard";
#else
	retitle_player(player.m); //ask for the title
#endif

	/* init inventory */
	Cornucopia cc(player.inv);
	cc.Initpack(player.skills);

	//note: these skills could be for debug purposes?
	player.skills.modify_raise(
		SKILLGRP_MAGIC, SPELL_TELEPORT,
		1200, true);
	player.skills.modify_raise(
		SKILLGRP_GENERIC, SKILL_DISARMTRAP,
		200, true);

	//note: this is also in playerinfo::Initialize
	const int pr=player.Get_Race();
	roleplay.Calculate_HP(player.hpp, npc_races[pr].hp_base, pr);
	player.Calculate_Totalhp();

	player.stat[STAT_SPD].initial= npc_races[pr].stats.SPD +
		roleplay.Get_Dex_Speed(player.stat[STAT_DEX].Get());

	//note: class init function was commented out
	/*
		const int pc=player.m.mclass;
		 if(classes[pc].initfunc == NULL) {
		 my_printf("Class init function missing! (Not done yet!)\n");
		 my_getch();
		 }
		 else {
		 classes[pc].initfunc(&player.skills, &player.inv, player.stat, player.hpp);
		 }
	*/

	return yearstook;
}
