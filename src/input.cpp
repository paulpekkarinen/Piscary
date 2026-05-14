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
#include "magic.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "options.h"
#include "output.h"
#include "ranged.h"
#include "score.h"
#include "target.h"
#include "terrain.h"
#include "textdata.h"

using std::string;

bool is_safe_char(int ch);

const char text_targetting[] =
"Target with movement keys [n,t,SPACE,q/ESC or?].";

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
	int dir=0;
	int key;

	if (self)
		msg.vadd(C_WHITE, "%s which direction [84261397.]?", prompt);
	else
		msg.vadd(C_WHITE, "%s which direction [84261397]?", prompt);
	key=my_getch();

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
		case '5':
		case '.':
			dir=5;
			break;
		case KEY_UP:
			dir=8;
			break;
		case KEY_DOWN:
			dir=2;
			break;
		case KEY_LEFT:
			dir=4;
			break;
		case KEY_RIGHT:
			dir=6;
			break;
		default:
			dir=10;
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

/*
 * "target" is a struct of type Target, one field must be set to determine
 * the target of a spell.
 *
 * Every spell has the similar format of targetting
 *
 */
void player_gettarget(level_type *level, Target *target, Spell *spell)
{
	int keycode, dir;

	being *mptr=0;
	invnode *iptr=0;

	bool monsterthere=false;

	int tx=0; //note: get player's screen position later
	int ty=0;

	target->clear();

	msg.add("Use movement keys for target, quit with ESC or ENTER.",
		C_WHITE);

	while(1)
	{
		keycode=my_getch();

		dir=0;

		if (is_quit_key(keycode))
			break;

		if(keycode==KEY_UP) dir=8;
		if(keycode==KEY_DOWN) dir=2;
		if(keycode==KEY_LEFT) dir=4;
		if(keycode==KEY_RIGHT) dir=6;
		if(keycode>='1' && keycode <='9')
		{
			dir=keycode-'0';
		}
		if(keycode==' ')
		{
			return;
		}
		else if(dir)
		{
			msg.update();

			/* last coordinates */
			int ltx=tx;
			int lty=ty;

			/* make new coordinates */
			tx+=move_dx[dir];
			ty+=move_dy[dir];

			/* do not allow beyond map window */
			if(tx<1) tx=1;
			if(ty<1) ty=1;
			if(tx>MAPWIN_SIZEX) tx=MAPWIN_SIZEX;
			if(ty>MAPWIN_SIZEY) ty=MAPWIN_SIZEY;

			/* check if that location is visible */
			const int rx=0; //note: was lreg.x, refactor later
			const int ry=0;

			if (gameview.Is_Visible(rx+tx-1, ry+ty-1)==false)
			{
				tx=ltx;
				ty=lty;
			}

			gameview.Show();

			Coord tc(rx+tx-1, ry+ty-1);

			gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
			put_char('*', CH_RED);
			gotoxy(0, MSGLINE+1);
			mptr=gameview.Get_Monster(tc);

			const int items=gameview.Count_Items(tc);
			monsterthere=false;

			if(mptr)
			{
				/* set target to monster */
				target->clear();

				if(spell->other >= 0)
				{
					target->pos=mptr->Get_Location();

					string s="Target = ";
					string monsname=monster_sprintf(mptr, true, false);
					append_string_with(s, monsname, '.');
					msg.newmsg(s, C_RED);
					monsterthere=true;
				}
				else
					msg.newmsg("Monster targetting not allowed.", C_RED);
			}
			else if (items)
			{
				/* set target to item coords */
				target->clear();

				if(spell->levitem >= 0)
				{
					target->pos.Set_Location(rx+tx-1, ry+ty-1);

					if(items==1)
					{
						iptr=gameview.Get_Item(tc);
						display->Item_Info(&iptr->i, iptr->i.weight, iptr->count,
							   "Target =");
					}
					else
					{
						msg.newmsg("Target = a pile of items.", C_WHITE);
					}
				}
				else
					msg.newmsg("Item targetting not allowed.", C_RED);
			}
			else if (player.Is_At(rx+tx-1, ry+ty-1))
			{
				target->clear();
				if(spell->self >= 0)
				{
					target->pos.Set_Location(rx+tx-1, ry+ty-1);
					msg.newmsg("Target = yourself.", C_WHITE);
				}
				else
					msg.newmsg("Self targetting not allowed.", C_RED);
			}
			else
			{
				const int tt=level->Get_Terrain(tc);
				string s=terrains[tt].desc;
				s.append("(no target).");
				msg.newmsg(s, C_WHITE);
			}
		}

		msg.notice();
		gotoxy(MAPWIN_RELX+tx, MAPWIN_RELY+ty);
	}
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

bool ranged_gettarget(level_type *level, int *scr_x, int *scr_y, int *lev_x, int *lev_y)
{
	being *mptr=0;

	/* get first target in sight */
	int tx, ty;
	int lasttidx=level->crew.Target_Nearest(level, &tx, &ty, 0);

	Coord pc=player.Get_Location();

	if (lasttidx==0)
	{
		tx=pc.x;
		ty=pc.y;
	}

	Coord sc(0, 0); //=player.Screen_Location(tx, ty); //note: fix this later

	*scr_x=sc.x;
	*scr_y=sc.y;
	*lev_x=tx;
	*lev_y=ty;

	msg.add(text_targetting, C_WHITE);

	int keycode='5';

	while (1)
	{
		int dir=5;
		if (keycode==KEY_ESC || keycode=='q' || keycode=='Q')
			return false;
		else if (keycode=='n' || keycode=='N')
		{
			lasttidx=level->crew.Target_Nearest(level, &tx, &ty, lasttidx);
			if (lasttidx<0)
				lasttidx=level->crew.Target_Nearest(level, &tx, &ty, lasttidx);
		}
		else if (keycode==KEY_UP)
			dir=8;
		else if (keycode==KEY_DOWN)
			dir=2;
		else if (keycode==KEY_LEFT)
			dir=4;
		else if (keycode==KEY_RIGHT)
			dir=6;
		else if (keycode>='1' && keycode <='9')
		{
			dir=keycode-'0';
		}
		else if (keycode=='t' || keycode=='T' || keycode==' ' ||
			keycode==PADENTER || keycode==KEY_ENTER)
		{
			return true;
		}
		else if (keycode=='?' || keycode=='h' || keycode=='H')
		{
			text_data->View(Text_Data::Missile_Help);

			msg.add(text_targetting, C_WHITE);
		}

		if (dir)
		{
			/* last coordinates */
			int ltx=tx;
			int lty=ty;

			/* make new coordinates */
			tx+=move_dx[dir];
			ty+=move_dy[dir];

			/* do not allow beyond map window */
			//note: refactor code here for limiting to screen location

			/* allow targetting only on visible areas */
			if (gameview.Is_Visible(tx, ty)==false)
			{
				tx=ltx;
				ty=lty;
			}

			//sc=player.Screen_Location(tx, ty); //note: fix later

			*scr_x=sc.x;
			*scr_y=sc.y;
			*lev_x=tx;
			*lev_y=ty;

			gameview.Show();

			pc=player.Get_Location();

			if (CONFIGVARS.targetline)
			{
				ranged_line(level, 0, 0, false,
					pc.x,
					pc.y,
					tx, ty, NULL, NULL, 0); //note: wonder how these nulls work?
			}

			gotoxy(sc.x, sc.y);
			put_char('X', CH_RED);
			gotoxy(0, MSGLINE+1);

			Coord tc(tx, ty);
			mptr=gameview.Get_Monster(tc);

			const int items=gameview.Count_Items(tc);
			if (mptr)
			{
				string moname=monster_sprintf(mptr, true, false);
				msg.vnewmsg(C_RED, "Target = %s.", moname.c_str());
			}
			else if (items)
			{
				/* set target to item coords */
				if (items==1)
				{
					invnode *iptr=gameview.Get_Item(tc);
					display->Item_Info(&iptr->i, iptr->i.weight, iptr->count,
						"Target =");
				}
				else
					msg.newmsg("Target = a pile of items.", C_WHITE);
			}
			else if (player.Is_At(tc))
			{
				msg.newmsg("Target = yourself.", C_WHITE);
			}
			else
			{
				const int tt=level->Get_Terrain(tc);
				msg.vnewmsg(C_WHITE, "%s (no target).", terrains[tt].desc);
			}
		}

		msg.notice();
		keycode=my_getch();
		msg.update();

		//note: fix later
		//sc=player.Screen_Location(tx, ty);
		//gotoxy(sc.x, sc.y);
	}
	msg.update();
}

int select_textlist(const char *prompt, const char **list, bool quit)
{
	int num=0;

	while (list[num] != NULL)
	{
		my_setcolor(C_RED);
		my_printf("%d", num+1);
		my_setcolor(C_WHITE);
		my_printf("] %s\n", list[num]);
		num++;
	}
	
	int cury=get_cursor_y()+1;

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
