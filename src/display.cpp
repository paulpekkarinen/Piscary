/**************************************************************************
 * display.cpp --                                                         *
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

//Refactored 25.2.2023 - 12.4.2026 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <array>
#include <cmath>
#include <cstring>
#include <format>
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "codex.h"
#include "currency.h"
#include "display.h"
#include "game.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "itemdata.h"
#include "lexicon.h"
#include "message.h"
#include "material.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "program.h"
#include "roleplay.h"
#include "salamath.h"
#include "saldebug.h"
#include "scrolls.h"
#include "terrain.h"
#include "viewtile.h"
#include "world.h"

using std::array;
using std::format;
using std::string;

const char *condition_txt[]=
{
     "is very near to death.",
     "is in VERY bad condition.",
     "is in very bad condition.",
     "is in pretty bad condition.",
     "has some serious looking wounds.",
     "has has some wounds.",
     "has got some bruises and scars.",
     "is looking healthy.",
     "is in good shape.",
     "is in exellent condition",	/* full hp */
     0,
};

const char *statdesc_str=
"\007High strength makes you, well eh..., stronger. It affects your carrying "
"capasity and several other things like how hard you hit your foes. ";

const char *statdesc_dex=
"\007Dexterity affects your speed and other speed related issues. High "
"dexterity allows you to act faster in all situations. ";

const char *statdesc_tgh=
"\007Toughness affects in many ways. High toughness makes you more tolerant "
"to attacks, affecting your defence rating. Low toughness makes the opposite. "
"There're some other situations too where high toughness does aid your "
"journeys.";

const char *statdesc_con=
"\007Constitution tells how your body is built. The natural strength of your "
"body. It affects your healing rate, how fast your wounds recover.";

const char *statdesc_cha=
"\007Charisma is your \"outlook\", how well you get by with others, how "
"good looking you are, and so on.\n\n"
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_wis=
"\007Wisdom tells something about your general knowledge.\n\n"
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_int=
"\007Intelligence affects your learning ability. The higher your intelligence "
"is, the better you can learn new skills, spells and so on.\n\n "
"\001You can't edit this stat because it really doesn't affect anything "
"yet.\n";

const char *statdesc_luc=
"\007";

const char *statdesc_spd=
"\007Speed is a derived attribute, it's calculated mainly by using your "
"dexterity. High speed allows you to move faster.";

const char *txt_statdescs[]=
{
   statdesc_str,
   statdesc_tgh,
   statdesc_con,
   statdesc_cha,
   statdesc_dex,
   statdesc_wis,
   statdesc_int,
   statdesc_luc,
   statdesc_spd,
   0
};

char itempstr[100];

const char *food_condition[]=
{
     "new",
     "good",
     "dubious"
     "stinking"
     "tainted"
     "rotten",
     0
};

const char *condition[]=
{
     "new",
     "almost new",
     "used",
     "worn",
     "almost broken",
     "broken",
     0
};

const char txt_logo[]=
	"#########################     "
	"#.................@.....#     "	
	"#....$....#############.#     "
	"#.........#           #.######"
	"###########      The  #......+"
	"   Legend of Saladir  #.######"	
	"#######################.#     "
	"#.......................#     "		
	"#########################     ";

void Display::Attribute_As(const char *what, Attribute &a)
{
	my_printf("%s: %3d/%-3d", what, a.value, a.max_value);
}

void Display::Error(const char *txt)
{
	print_text(txt);
	showmore(false, false);	
}

void Display::Footer_Error(const char *txt)
{
	set_color(CH_RED);
	clearline(SCREEN_LINES-1);
	print_centered(SCREEN_LINES-1, txt);
	wait_key();
}

void Display::Footer_Failure(const char *txt)
{
	set_color(CH_RED);
	clearline(SCREEN_LINES-2);
	print_centered(SCREEN_LINES-2, txt);
		
	set_color(CHB_WHITE);
	print_centered(SCREEN_LINES-1, "[Press a key]");
	wait_key();
}

void Display::Footer(const char *txt, int color)
{
	set_color(get_darker_color(color));
	drawline(SCREEN_LINES-1, '-');
	
	set_color(color);
	
	string s("~{ ");
	s.append(txt);
	s.append(" }~");

	print_centered(SCREEN_LINES-1, s.c_str());	
}

void Display::Header(const char *txt, int color)
{
	clear();

	set_color(get_darker_color(color));
	drawline(0, '-');
		
	set_color(color);

	string s("~{ ");
	s.append(txt);
	s.append(" }~");

	print_centered(0, s.c_str());
}

void Display::Hiscore_Header()
{
	Header("All time best scores", C_GREEN);
	Footer("Arrow up = Next | arrow down = Previous | Esc = exit", C_GREEN);
}

//prints out some info about the item based on the identified status...
void Display::Item_Info(item_def *iptr, int weight, int count, const char *acttxt)
{
	string status;
	string ident;
	string item; //combined name
	string many;
	string pile;
	string price;

	/* if it's a container, modify the weight */
	if (iptr->inv != 0)
		weight += iptr->inv->Get_Weight();

	if (iptr->status & ITEM_UNPAID)
	{
		Currency rupees(iptr->price);

		price="(";

		if (rupees.gold)
			price+=format("{}g", rupees.gold);

		if (rupees.silver)
		{
			if (rupees.gold)
				price.append(",");
			price+=format("{}s", rupees.silver);
		}

		if (rupees.copper)
		{
			if (rupees.gold || rupees.silver)
				price.append(",");
			price+=format("{}c", rupees.copper);
		}
		price.append(")");
	}

	if ((iptr->status & ITEM_IDENTIFIED))
	{
		if ((iptr->status & ITEM_BLESSED))
		{
			status="blessed ";
			my_setcolor(CH_GREEN);
		}
		if ((iptr->status & ITEM_CURSED))
		{
			status="cursed ";
			my_setcolor(CH_RED);
		}
		/* show modifiers */
		if (iptr->melee_dt >0 || iptr->melee_ds || iptr->meldam_mod >0)
		{
			sprintf(itempstr, "[%dd%d,%+2d] ",
				iptr->melee_dt, iptr->melee_ds, iptr->meldam_mod);
			ident.append(itempstr);
		}

		if (iptr->missi_dt >0 || iptr->missi_ds || iptr->misdam_mod >0)
		{
			sprintf(itempstr, "{%dd%d,%+2d} ",
				iptr->missi_dt, iptr->missi_ds, iptr->misdam_mod);
			ident.append(itempstr);
		}

		if (iptr->ac>0)
		{
			sprintf(itempstr, "(AC%+2d)", iptr->ac);
			ident.append(itempstr);
		}

		if (acttxt==0)
		{
			sprintf(itempstr, "%s %4.2fkg",
				price.c_str(), (real)(count*weight)/WEIGHT_KILO);
			ident.append(itempstr);
		}
		else
		{
			ident+=price;
		}
	}
	else
	{
		if (acttxt==0)
		{
			sprintf(itempstr,
				"%s %4.2fkg", price.c_str(), (real)(count*weight)/WEIGHT_KILO);
			ident.append(itempstr);
		}
		else
			ident=price;
	}

	if (count>1)
	{
		pile=format("a pile of {} ", count);
		many="s";
	}
	else
		pile="a ";

	const char *itemname=iptr->name.c_str();

	if (iptr->type==IS_FOOD)
	{
		item=format("{}{}{}{}", pile, status, itemname, many);

		if ((iptr->status & ITEM_IDENTIFIED))
			item+=format(" ({})", food_condition[iptr->icond]);
	}
	else if (iptr->type==IS_SCROLL)
	{
		if (list_scroll[iptr->group].flags & SCFLAG_IDENTIFIED)
			iptr->status|= ITEM_IDENTIFIED;

		if (iptr->status & ITEM_IDENTIFIED)
		{
			item=format("{}{}{}{} of {} (\"{}\")",
				pile, status, itemname, many,
				iptr->rname, iptr->sname);
		}
		else
		{
			item=format("{}{} {}{} labeled \"{}\"",
				pile, materials[iptr->material].name,
				itemname, many, iptr->sname);

			if (list_scroll[iptr->group].flags & SCFLAG_NAMED)
			{
				item+=format(" (\"{}\")",
					list_scroll[iptr->group].cname);
			}
			else if (list_scroll[iptr->group].flags & SCFLAG_TRIED)
				item.append(" {tried}");
		}
	}
	else if (iptr->type==IS_MONEY)
	{
		item=format("{}{}{}{}", pile, status, itemname, many);
	}
	else if (iptr->type==IS_SPECIAL)
	{
		item=format("{} {}{}{}", pile, status, itemname, many);
	}
	else
	{
		if (iptr->material>=0)
		{
			item=format("{}{} {}{} {}{}",
				pile, condition[iptr->icond], status,
				materials[iptr->material].name, itemname, many);
		}
		else
		{
			item=format("{}{} {}{}{}",
				pile, condition[iptr->icond], status,
				itemname, many);
		}
	}

	if (acttxt==0)
	{
		my_printf(item.c_str());
		const int idenlen=(int)ident.size();
		gotoxy(SCREEN_COLS-idenlen, get_cursor_y());
		my_printf(ident.c_str());
	}
	else
	{
		string s=format("{} {}{}!", acttxt, item, ident);
		msg.newmsg(s.c_str(), C_WHITE);
	}
}

void Display::Monster_Description(being *mptr)
{
	if (!mptr)
		return;

	string moname=monster_sprintf(mptr, false, false);
	Underline_Header(moname.c_str(), CH_WHITE, C_YELLOW);
	
	my_setcolor(C_WHITE);
	if (mptr->m.longdesc)
	{
		my_wordwraptext(desclist[mptr->m.longdesc], 4, SCREEN_LINES-1, 1, SCREEN_COLS-1);
		showmore(false, false);
	}

	my_setcolor(C_WHITE);
	const char *dptr=npc_races[mptr->m.race].desc;

	if (dptr)
		my_wordwraptext(dptr, 4, SCREEN_LINES-1, 1, SCREEN_COLS-1);
	else
		my_printf("\nNo race description.\n");
	my_printf("\n");

	if (npc_races[mptr->m.race].behave & BEHV_ANIMAL)
	{
		my_printf("It's an animal");
		if (npc_races[mptr->m.race].behave & BEHV_CANUSEITEM)
			my_printf(" but it's known to use some items. ");
		else my_printf(". ");
	}
	if (npc_races[mptr->m.race].behave & BEHV_FLYING)
		my_printf("It can fly.");
	my_printf("\n");
	if (npc_races[mptr->m.race].attacktypes & ATTACK_BITE)
		my_printf("It can bite. ");
	if (npc_races[mptr->m.race].attacktypes & ATTACK_KICK)
		my_printf("It can kick. ");
	if ((npc_races[mptr->m.race].attacktypes & ATTACK_HIT) || npc_races[mptr->m.race].attacktypes==0)
		my_printf("It can hit. ");
	my_printf("\n");

	bool some1=false;
	zprintf("As you examine %s closer, you notice that ",
		gender_art2[mptr->m.gender]);

	invnode *eqitem=mptr->equips.get_inventory_item(EQUIP_RHAND);
	if (eqitem)
	{
		some1=true;
		zprintf("%s has readied a %s in %s right hand",
			gender_art1[mptr->m.gender],
			mptr->equips.get_equipment_name(EQUIP_RHAND),
			gender_art3[mptr->m.gender]);
	}

	eqitem=mptr->equips.get_inventory_item(EQUIP_LHAND);
	if (eqitem)
	{
		if (some1)
			zprintf(" and ");
		zprintf("in %s left hand %s has a %s",
			gender_art3[mptr->m.gender],
			gender_art1[mptr->m.gender],
			mptr->equips.get_equipment_name(EQUIP_LHAND));
	}

	eqitem=mptr->equips.get_inventory_item(EQUIP_MISSILE);
	if (eqitem)
	{
		if (some1)
			zprintf(" and ");
		zprintf("%s seems to have some missiles readied (%s)",
			gender_art1[mptr->m.gender],
			mptr->equips.get_equipment_name(EQUIP_MISSILE));
	}

	if (!some1)
		zprintf("%s is not using anything particular.\n",
			gender_art1[mptr->m.gender]);
	else
		zprintf(".\n");
	//   my_printf("\n");

	const int max_hit_points=mptr->health.max_value;

	real tmp=(real)max_hit_points;
	tmp=tmp/CONDITION_MAXNUM;

	real rg=(real)mptr->Get_Hit_Points();
	rg=rg/tmp;
	int i=(int)rg;

	if (i<0)
		i=0;
	if (i>=CONDITION_MAXNUM)
		i=CONDITION_MAXNUM-1;

	my_printf("%s %s\n", moname.c_str(), condition_txt[i]);

	if (debug->Cheat())
	{
		my_setcolor(CH_BLUE);
		my_printf("\nCheat values here: (monster id=%ld)\n", mptr->id);
		my_setcolor(C_WHITE);
		my_printf("It can target:");
		if (npc_races[mptr->m.race].targetflags & TARGET_HEAD)
			my_printf(" %s", bodyparts[HPSLOT_HEAD]);
		if (npc_races[mptr->m.race].targetflags & TARGET_BODY)
			my_printf(" %s", bodyparts[HPSLOT_BODY]);
		if (npc_races[mptr->m.race].targetflags & TARGET_LHAND)
			my_printf(" %s", bodyparts[HPSLOT_LEFTHAND]);
		if (npc_races[mptr->m.race].targetflags & TARGET_RHAND)
			my_printf(" %s", bodyparts[HPSLOT_RIGHTHAND]);
		if (npc_races[mptr->m.race].targetflags & TARGET_LEGS)
			my_printf(" %s", bodyparts[HPSLOT_LEGS]);

		my_printf("\nAlignment=%d\tLevel=%d (exp=%d)\n",
			mptr->m.align, mptr->m.level, mptr->exp);
		my_printf("It's hitpoints are %d/%d\n",
			mptr->health.value, max_hit_points);
		for (i=0; i<HPSLOT_MAX; i++)
		{
			if (npc_races[mptr->m.race].bodyparts[i]>=0)
			{
				if (npc_races[mptr->m.race].behave & BEHV_FLYING)
					my_printf("%s [%d/%d] ", bodyparts_flying[i],
						mptr->hpp[i].cur, mptr->hpp[i].max);
				else
					my_printf("%s [%d/%d] ", bodyparts[i],
						mptr->hpp[i].cur, mptr->hpp[i].max);
			}
		}
		my_printf("\n");
		if (mptr->m.status & MST_FLEEMODE)
			my_printf(", It's fleeing");
		if (mptr->m.status & MST_ATTACKMODE)
		{
			my_printf(", It's attacking");
			if (mptr->target==NULL)
				my_printf(" YOU");
			else
				my_printf(" %s", mptr->target->m.desc.c_str());
		}
		my_printf("\n");
		my_printf("STR=%d DEX=%d CON=%d WIS=%d INT=%d CHA=%d TGH=%d SPD=%d LCK=%d\n",
			mptr->stat[STAT_STR].Get(),
			mptr->stat[STAT_DEX].Get(),
			mptr->stat[STAT_CON].Get(),
			mptr->stat[STAT_WIS].Get(),
			mptr->stat[STAT_INT].Get(),
			mptr->stat[STAT_CHA].Get(),
			mptr->stat[STAT_TGH].Get(),
			mptr->stat[STAT_SPD].Get(),
			mptr->stat[STAT_LUC].Get());

		const int con=mptr->stat[STAT_CON].Get();
		const int con_hitp=roleplay.Get_Con_HP(con);

		rg=(real)con_hitp;
		rg=rg/TIME_MOVEAROUND;
		my_printf("Regen at %d, current %d. It take appr. %4.2f turns per Hp.\n",
			con_hitp, mptr->regentime, rg);
		rg=((real)max_hit_points) * 0.20;
		my_printf("It flees when hitpoints fall below %4.2f\n", rg);
	}
	showmore(false, false);
}

void Display::Draw_Terrain(int ctype, int fov)
{
	if (fov==Viewtile::Visible)
	{
		set_color(terrains[ctype].color);
		addch(terrains[ctype].out);
	}
	else
	{
		//dark but explored
		if (fov==Viewtile::Dark)
			put_char(terrains[ctype].out, CH_DGRAY);
	}
}

void Display::Player_Status(Actor &tonttu)
{
	Header("Your status", CH_GREEN);

	goto_content();
	
	my_printf("Limb status:\n");
	set_color(C_WHITE);
	for (int i=0; i<HPSLOT_MAX; i++)
	{
		set_color(C_YELLOW);
		my_printf("%-15s", bodyparts[i]);
		set_color(C_WHITE);
		my_printf(" Hp [%3d/%3d] Ac [%3d] ",
			tonttu.hpp[i].cur,
			tonttu.hpp[i].max,
			tonttu.hpp[i].ac);

		my_printf(" Fire%03d Posn%03d Cold%03d Elec%03d Watr%03d Acid%03d\n%15s ",
			tonttu.hpp[i].res.fire,
			tonttu.hpp[i].res.poison,
			tonttu.hpp[i].res.cold,
			tonttu.hpp[i].res.elec,
			tonttu.hpp[i].res.water,
			tonttu.hpp[i].res.acid, "");

		if (tonttu.equips.is_usable(eqslot_from_hpslot[i]))
		{
			set_color(C_GREEN);
			my_printf("Condition OK!\n");
		}
		else
		{
			set_color(C_RED);
			my_printf("Not usable!\n");
		}
		set_color(C_WHITE);

		/*
		  my_printf("%15s Fire=%3d Poison=%3d Cold=%3d "
		  "Elec=%3d Water=%3d Acid=%3d\n", "",
		  player.hpp[i].res.fire,
		  player.hpp[i].res.poison,
		  player.hpp[i].res.cold,
		  player.hpp[i].res.elec,
		  player.hpp[i].res.water,
		  player.hpp[i].res.acid);
		*/
	}

	set_color(C_GREEN);
	my_printf("Experience:\n");
	roleplay.Check_Expneeded(player);

	wait_more_lines(SCREEN_LINES-1, "Press a key");
}

void Display::Underline_Header(const char *txt, int color, int ucolor)
{
	clear();
	set_color(color);
	print_centered(0, txt);

	set_color(ucolor);
	drawline(1, '-');		
}

void Display::Redraw(level_type *level)
{
	GAME_NOTIFYFLAGS|=GAME_SHOWALLSTATS;

	clear_screen();
	msg.update();

	gameview.Show();

	this->Stats(false);

	GAME_NOTIFYFLAGS = 0;
}

void Display::Journey_Log()
{
	Header("Journey log", CH_GREEN);

	goto_content();
	my_printf("Time:\n");
	set_color(C_WHITE);
	world->Show_Time();
	Game.show_time();

	wait_more_lines(SCREEN_LINES, "Press a key");
}

void Display::More_Inventory(const int y)
{
	print_toc(0, y, "[...more]", CH_GREEN);
}

void Display::Selection_Prompt(int weight)
{
	int i=0;

	string cattypes("Select item group with [ ");

	for (i=0; i<NUMOFITEMGROUPS; i++)
		cattypes.push_back(gategories[i].out);

	cattypes.append(" or . for all ]");

	my_setcolor(CH_DGRAY);
	drawline(SCREEN_LINES-3, '=');
	gotoxy(55, SCREEN_LINES-3);
	set_color(C_YELLOW);
	if (weight>0)
		my_printf("[ Total of %4.2fkg ]", (real)(weight)/1000);

	set_color(C_WHITE);
	clear_lines(SCREEN_LINES-3, SCREEN_LINES-1);

	print_centered(SCREEN_LINES-3, cattypes.c_str());
	set_color(C_GREEN);
	print_centered(SCREEN_LINES-1, "To get help with this screen, press CTRL+I.");
	/*
	   print_centered(SCREEN_LINES-2,
			"[a-z Select] [A-Z Toggle] [1 open container] [2 move selected]");
	   print_centered(SCREEN_LINES-1,
			"[2,3,PGDN,DOWN move down] [8,9,PGUP,UP move up] [SPC/ESC close]");
	*/
}

void Display::Stat_Value(int wy, const char *txt, statpack *pstat)
{
	const int result=pstat->Get();

	gotoxy(0, wy);
	set_color(C_WHITE);

	my_printf("%s ", txt);
	my_printf("%2d", result);

	if (pstat->temp > 0)
	{
		set_color(C_GREEN);
		my_printf(" (%+2d)",pstat->temp);
	}
	else if (pstat->temp < 0)
	{
		set_color(C_RED);
		my_printf(" (%2d)",pstat->temp);
	}

	if (pstat->perm > 0)
	{
		set_color(CH_GREEN);
		my_printf(" (%+2d)",pstat->perm);
	}
	else if (pstat->perm < 0)
	{
		set_color(CH_RED);
		my_printf(" (%2d)",pstat->perm);
	}
}

void Display::Stat_Description(int st, int top)
{
	my_wordwraptext(txt_statdescs[st], top, SCREEN_LINES, 1, SCREEN_COLS);
	clrtobot();
}

void Display::Stats(bool showmove)
{
	my_setcolor(C_WHITE);

	/* movement count on every turn */
	gotoxy(0, 4);
	my_printf("Moves: %-6d", player.movecount);

	/* does money amount need a redraw ? */
	if (GAME_NOTIFYFLAGS & GAME_MONEYCHG)
	{
		print_text_to(0, 6, "Wealth:");

		gotoxy(1, 7);
		my_printf("%6.1f gp",
			((real)player.inv.copper) / valuables[MONEY_GOLD].value);		
	}

	/* does experience need a redraw ? */
	if (GAME_NOTIFYFLAGS & GAME_EXPERCHG)
	{
		gotoxy(0, 8);
		my_printf("Exp: %d", player.exp);
	}

	if (GAME_NOTIFYFLAGS & GAME_ATTRIBCHG)
	{
		this->Stat_Value(11, "Str:", &player.stat[STAT_STR]);
		this->Stat_Value(12, "Tgh:", &player.stat[STAT_TGH]);
		this->Stat_Value(13, "Dex:", &player.stat[STAT_DEX]);
		this->Stat_Value(14, "Con:", &player.stat[STAT_CON]);
		this->Stat_Value(15, "Cha:", &player.stat[STAT_CHA]);
		this->Stat_Value(16, "Wis:", &player.stat[STAT_WIS]);
		this->Stat_Value(17, "Int:", &player.stat[STAT_INT]);

		this->Stat_Value(18, "Spd:", &player.stat[STAT_SPD]);
		this->Stat_Value(19, "Lck:", &player.stat[STAT_LUC]);
	}

	if (GAME_NOTIFYFLAGS & GAME_HPSPCHG)
	{
		gotoxy(0, 19);
		Attribute_As("Hp", player.health);
		gotoxy(0, 20);
		Attribute_As("Sp", player.mana);
	}

	set_color(C_WHITE);
	gotoxy(0, STATUSROW);

	if (GAME_NOTIFYFLAGS & GAME_LEVELCHG)
	{
		my_printf("%s %s (lvl %d)",
			player.Get_Name(), player.Get_Title(), player.m.level);

		this->World_Location(world);
	}

	if (GAME_NOTIFYFLAGS & GAME_ALIGNCHG)
	{
		gotoxy(SCREEN_COLS-2, STATUSROW+1);
		const int a=player.m.align;
		if (a == CHAOTIC)
			my_printf("C-");
		else if (a < CHAOTIC_E)
			my_printf("C ");
		else if (a == NEUTRAL)
			my_printf("N=");
		else if (a < NEUTRAL_E)
			my_printf("N ");
		else if (a < LAWFUL_E)
			my_printf("L ");
		else if (a == LAWFUL)
			my_printf("L+");
	}

#ifdef _NOT_USED_CODE_
	clearline(STATUSROW+2);

	gotoxy(0, STATUSROW+2);
	if (player.nutr == FOOD_FAINTED)
	{
		set_color(CHB_RED);
		my_printf("Fainted! ");
	}
	else if (player.nutr <= FOOD_FAINTING)
	{
		set_color(CH_RED);
		my_printf("Fainting ");
	}
	else if (player.nutr <= FOOD_STARVING)
	{
		set_color(C_RED);
		my_printf("Starving ");
	}
	else if (player.nutr <= FOOD_HUNGRY)
	{
		set_color(C_YELLOW);
		my_printf("Hungry ");
	}
	else if (player.nutr <= FOOD_FULL)
	{
	}
	else if (player.nutr <= FOOD_SATIATED)
	{
		set_color(C_GREEN);
		my_printf("Satiated ");
	}
	else
	{
		set_color(CH_GREEN);
		my_printf("Bloated ");
	}

	/* confuzion */
	if (player.conf > 0)
	{
		set_color(CH_BLUE);
		my_printf("Confuzed ");
	}

	/* check weights */
	if (player.burden == INV_OVERLOAD)
	{
		set_color(CH_RED);
		my_printf("Overloaded ");
	}
	else if (player.burden == INV_STRAIN)
	{
		set_color(C_RED);
		my_printf("Strained ");
	}
	else if (player.burden == INV_BURDEN)
	{
		set_color(C_RED);
		my_printf("Burdened ");
	}
#endif

	if (GAME_NOTIFYFLAGS & GAME_CONDCHG)
		player.conditions.statshow(STATUSROW+2);
}

void Display::Story()
{
	Header("Story", CH_GREEN);

	my_wordwraptext(
		"Returning from your last fishing trip you make a wrong turn in a particularly "
		"dense forest. When you finally emerge out of the forest you stumble upon "
		"a place called Salmorria. It sounds promising for fishing business and can't "
		"be worse than the last place, where you missed an opportunity to find a "
		"large diamond inside a fish. You were beaten by an oldtimer who hopefully "
		"gets so excited about the diamond that he kicks the bucket.\r\r"
		"Armed with your bendy fishing rod you cluelessly run into a new adventure.",
		3, SCREEN_LINES-3, 3, SCREEN_COLS-3);

	Footer("x = Return to main menu", CH_GREEN);
	my_getch();
}

void Display::Title()
{
	clear_screen();

	const int data_width=30;
	const int logox=(SCREEN_COLS-data_width)/2;
	int i=0;

	for (int dy=0; dy<9; dy++)
	{
		gotoxy(logox, dy+3);
		for (int dx=0; dx<data_width; dx++)
		{
			int color;
			const char ch=txt_logo[i];

			switch (ch)
			{
				case '#': color=CH_DGRAY; break;
				case '@': color=CH_RED; break;
				case '$': color=CH_YELLOW; break;
				default: color=CH_WHITE; break;
			}
		
			put_char(txt_logo[i], color);
			i++;
		}
	}

	int cy=SCREEN_LINES/2-2;

	set_color(CH_BLUE);
	print_centered(cy, "Fishing Route");

	const int cx=(SCREEN_COLS/2 - 5);
	cy=SCREEN_LINES/2+2;
	set_color(CH_GREEN);
	print_text_to(cx, cy, "p) play");
	print_text_to(cx, cy+1, "n) new game");
	print_text_to(cx, cy+2, "h) highscores");
	print_text_to(cx, cy+3, "s) story");
	print_text_to(cx, cy+4, "q) exit");

	cy=SCREEN_LINES-1;
	set_color(CH_YELLOW);
	print_centered(cy-2, PROC_VERSION PROC_PLATFORM " " PROC_DATE);
	print_centered(cy-1, "(C)1997-1999 by Erno Tuomainen");
	set_color(CH_BLUE);
	print_centered(cy, "Piscary 2021-2026 by Paul Pekkarinen");
}

void Display::World_Location(World *myworld)
{
	string s;
	if (player.huntmode)
		s="Wilderness";
	else
	{
		s.append(myworld->Get_Dungeon_Name());
		s.append(" Lvl:");
		s+=to_string(myworld->Get_Level_Index());
		s.append(" ");
		s.append(myworld->Get_Level_Name());
	}

	const int sz=(int)s.size();

	print_text_to(SCREEN_COLS-sz, STATUSROW, s.c_str());
}
