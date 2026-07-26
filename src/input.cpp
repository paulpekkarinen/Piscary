/**************************************************************************
 * input.cpp --                                                           *
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

//Refactored 21.10.2022 - 7.4.2026 Paul K. Pekkarinen

#include <cstring>
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "dice.h"
#include "display.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "lexicon.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "score.h"
#include "terrain.h"
#include "textdata.h"
#include "way.h"

using std::string;

bool is_safe_char(int ch);

//const char text_targetting[] =
//"Target with movement keys [n,t,SPACE,q/ESC or?].";

bool confirm_yn(const char *prompt, bool defaultresult, bool showprompt)
{
	int resp;
	bool result;

	if(showprompt)
	{
		string s(prompt);

		if(defaultresult)
			s.append(" (Y/n)?");
		else
			s.append(" (y/N)?");

		msg.add(s.c_str(), C_WHITE);
	}

	while(1)
	{
		resp=my_getch();
		if (is_confirm_key(resp))
		{
			result=defaultresult;
			break;
		}
		if(resp=='n' || resp=='N')
		{
			result=false;
			break;
		}
		if(resp=='Y' || resp=='y')
		{
			result=true;
			break;
		}
	}

	msg.update();

	return result;
}

/*
** Calculates new position based on direction
** Used by many routines, like chat
*/
int dir_askdir(const char *prompt, bool self)
{
	int dir=Way::Nowhere;

	if (self)
		msg.vadd(C_WHITE, "%s which direction [84261397.]?", prompt);
	else
		msg.vadd(C_WHITE, "%s which direction [84261397]?", prompt);

	const int key=my_getch();

	switch (key)
	{
		case '1':
		case '2':
		case '3':
		case '4':
		case '6':
		case '7':
		case '8':
		case '9':
			dir=key-'0';
		break;
		case '5': dir=Way::Yourself; break;
		case '.':
			if (self)
				dir=Way::Yourself;
		break;
		case KEY_UP: dir=Way::North; break;
		case KEY_DOWN: dir=Way::South; break;
		case KEY_LEFT: dir=Way::West; break;
		case KEY_RIGHT: dir=Way::East; break;
		default:
			msg.newmsg("Not a valid direction!", C_WHITE);
			break;
	}

	msg.update();
	return dir;
}

int get_amount_of_items(int max_count)
{
	string s=get_string(false, 6);

	int n;
	if (s.empty()) n=max_count; //enter to select all
	else n=from_string(s);

	return n;
}

int get_index_from_key(int k, int max_items)
{
	if (k>='a' && k < 'a'+max_items)
		return k-'a';

	if (k>='A' && k < 'A'+max_items)
		return k-'A';

	return -1;
}

int get_response(const char *prompt, const char *str)
{
	if(strlen(str)<1)
	{
		my_printf("Error, no choices in string!\n");
		return -1;
	}

	if(prompt)
		my_printf(prompt);

	my_printf(" [%s] ?", str);

	int key=KEY_ESC+1; //set to not ESC

	while (key!=KEY_ESC) //break if ESC pressed
	{
		key=my_getch();
		for (int i=0; i<(int)strlen(str); i++)
		{
			if(str[i]==key)
				return i;
		}
	}
	return -1;
}

string get_string(bool safe_chars, const int max_len)
{
	const Coord c=get_cursor();
	string s;

	for (;;)
	{
		print_text_to(c.x, c.y, s.c_str());

		const int ch=my_getch();

		if (safe_chars)
		{
			//don't add if it's an illegal character
			if (is_safe_char(ch)==false) continue;
		}

		//enter can be pressed only if there are 1 or more letters
		if (ch==KEY_ENTER)
		{
			if (s.size()>0) break;
			continue;
		}

		if (ch==KEY_BACKSPACE) //backspace = delete char
		{
			if (s.size()>0)
			{
				s.pop_back();

				//delete the letter from screen also (note: test if needed)
				//int sx=my_getx();
				//int sy=my_gety();
				//my_puts(sx-1, sy, " ");
			}
		}
		else if (ch==KEY_ESC)
		{
			//pressing ESC returns an empty string
			s.clear();
			break;
		}
		else
		{
			if ((int)s.size()<=max_len)
				s.push_back(ch);
		}
	}

	return s;
}

bool is_confirm_key(int k)
{
	if (k==' ' || k==PADENTER || k==KEY_ENTER)
		return true;

	return false;
}

bool is_safe_char(int ch)
{
	//these are allowed, but not added to a string
	if (ch==KEY_BACKSPACE || ch==KEY_ESC || ch==KEY_ENTER)
		return true;
	if (ch>255) return false; //isalpha can't handle bigger numbers
	if (is_alpha(ch)) return true;
	return false;
}

bool is_quit_key(int k)
{
	if (k=='x' || k==PADENTER || k==KEY_ENTER)
		return true;

	return false;
}

bool list_more(int &y)
{
	bool rv=true;

	y++;
	if (y>SCREEN_LINES-1)
	{
		rv=wait_more(); //ESC to cancel listing
		clear_screen();
		y=0;
	}

	return rv;
}

int my_getch()
{
	flushinp();

	/* refresh screen before any input */
	refresh();

	return getch();
}

int my_gets(char *str, int mlen)
{
	int ch;
	echo();
	ch=wgetnstr(stdscr, str, mlen);
	noecho();

	return ch;
}

void rename_monster(monsterdef &mondef)
{
	string s=get_string(false, NAMEMAX);

	//monster's name can be left unchanged if the player just pressed return
	//without giving a name
	if (s.empty())
		return;

	mondef.name=s; //rename
}

void retitle_player(monsterdef &mondef)
{
	my_setcolor(C_YELLOW);
	my_printf("What is your title? ");
	my_setcolor(C_GREEN);
	mondef.desc=get_string(false, TITLEMAX);

	if (mondef.desc.empty())
		mondef.desc="the adventurer";
}

int select_textlist(const char *prompt, std::vector<const char*> &items, bool quit)
{
	int num=0;

	for (const char* item : items)
	{
		my_setcolor(C_GREEN);
		my_printf("%d", num+1);
		my_setcolor(C_WHITE);
		my_printf("] %s\n", item);
		num++;
	}

	const int cury=get_cursor_y()+1;

	while (1)
	{
		gotoxy(0, cury);
		set_color(C_GREEN);
		my_printf("%s? ", prompt);

		int ch=my_getch();

		if (ch>='1' && ch <='9')
		{
			ch-='1';
			if (ch<num)
				return ch;
		}

		if (ch=='q' && quit)
			return -1;
	}
	my_printf("\n");
	return 0;
}

void showmore(bool msgmode, bool shortmsg)
{
	int key=0, i=0;

	int col=C_WHITE;
	const Coord c=get_cursor();

	if(msgmode)
		msg.add("(more)", C_WHITE);

	/* wait for morekey */
	while(1)
	{
		if(!msgmode)
		{
			my_setcolor(col);
			col=1+RANDU(15);
			gotoxy(c.x, c.y);
			if(i==0 || shortmsg)
				my_printf("(more)");
			else
				my_printf("(more, with SPC/ENTER/ESC)");
		}
		key=my_getch();
		if(CONFIGVARS.anykeymore)
		{
			break;
		}
		else
		{
			if (key==KEY_ESC || key==KEY_ENTER || key==PADENTER || key==' ')
				break;
		}
		i++;
	}
}

void wait_key()
{
	refresh();
	getch();
}

bool wait_more()
{
	my_printf("--more--");
	if (my_getch()==KEY_ESC) return false;
	return true;
}

bool wait_more_lines(int cy, const char *teksti)
{
	my_setcolor(CH_GREEN);
	drawline(cy, '-');

	string s("+[ ");
	s.append(teksti);
	s.append(" ]+");

	print_centered(cy, s.c_str());

	if (my_getch()==KEY_ESC) return false;
	return true;
}
