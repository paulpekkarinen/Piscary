//Legend of Saladir - output.cpp

//Refactored 5.4.2022 - 14.4.2026 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstring>
#include <string>
#include "geometry.h"
#include "input.h"
#include "output.h"

using std::string;

constexpr int Msg_Buffer_Size=100;
char msg_buffer[Msg_Buffer_Size];
char zmsg_buffer[Msg_Buffer_Size];

/* a buffer for variable argument functions */
constexpr int Varg_Buf_Size=1024;
char vargbuffer[Varg_Buf_Size];

void clearline(int y)
{
	move(y, 0);
	clrtoeol();
}

void clear_lines(int from_y, int to_y)
{
	for (int y=from_y; y<=to_y; y++)
		clearline(y);
}

void clear_screen()
{
	clear();
	my_setcolor(C_WHITE);
}

//Determine a char to print the line with.
char decidelinechar(int ox, int oy, int nx, int ny)
{
	char outchar='-';

	if((nx>ox && ny>oy) || (nx<ox && ny<oy))
		outchar='\\';
	if((nx<ox && ny>oy) || (nx>ox && ny<oy))
		outchar='/';

	if(ny==oy)
		outchar='-';
	if(nx==ox)
		outchar='|';

	return outchar;
}

void enable_scroll()
{
	scrollok(stdscr, TRUE);
}

void disable_scroll()
{
	scrollok(stdscr, FALSE);
}

void drawline(int y, char chr)
{
	move(y, 0);

	for(int i=0; i<SCREEN_COLS; i++)
		addch(chr);
}

void drawline_limit(int y, int x1, int x2, char chr)
{
	if(x1>=x2) return;

	gotoxy(x1, y);

	for(int i=x1; i<=x2; i++)
		addch(chr);
}

void drawborder(int x1, int y1, int x2, int y2)
{
	int j;
	for(j=y1; j<y2; j++)
	{
		gotoxy(x1, j);
		my_printf("|");
		drawline_limit(j, x1+1, x2, ' ');
	}
	gotoxy(x1, j);
	my_printf("+");
	drawline_limit(j, x1+1, x2-1, '-');
}

void draw_rectangle(int x, int y, int w, int h, char ch)
{
	for (int dy=y; dy<y+h; dy++)
	{
		gotoxy(x, dy);
		for (int dx=x; dx<x+w; dx++)
		{
			addch(ch);
		}
	}
}

//Return current cursor position.
Coord get_cursor()
{
	Coord rv;
	getyx(stdscr, rv.y, rv.x);
	return rv;
}

int get_cursor_x()
{
	Coord c=get_cursor();
	return c.x;
}

int get_cursor_y()
{
	Coord c=get_cursor();
	return c.y;
}

void gotoxy(int x, int y)
{
	move(y, x);
}

//Move cursor at the location after header, where the "content" starts.
void goto_content()
{
	move(2, 0);
}

void hidecursor()
{
	curs_set(0);
}

//Check if word fits on screen.
bool is_over_border(int word_length)
{
	Coord c=get_cursor();

	if (c.x>=SCREEN_COLS-word_length)
		return true;

	return false;
}

void makeborder(int x1, int y1, int wd, int hg)
{
	gotoxy(x1, y1);
	/* curses origo in 0,0, my origo in 1,1 */

	vline('|', hg);
	hline('=', wd);
	addch('+');

	gotoxy(x1, y1+hg-1);
	hline('=', wd);
	addch('+');

	gotoxy(x1+wd-1, y1);
	vline('|', hg);
	addch('+');

	gotoxy(x1+wd-1, y1+hg-1);
	addch('+');
}

void my_center_puts(int y, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(vargbuffer, Varg_Buf_Size, fmt, ap);
	va_end(ap);

	print_centered(y, vargbuffer);
}

void my_cputs_init(int color, bool cond, const char *str)
{
	const char keytxt[] = "[KEY]";

	int y=SCREEN_LINES-2;

	my_setcolor(color);

	if(!cond)
	{
		y++;
	}

	clearline(y);

	int x=(SCREEN_COLS/2) - (strlen(str)/2);
	print_text_to(x, y, str);

	if(!cond)
	{
		set_color(C_GREEN);
		clearline(y+1);
		gotoxy(SCREEN_COLS - strlen(keytxt)-1, y+1);
		my_printf(keytxt);
		my_getch();
	}

	refresh();
}

void my_printf(const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(msg_buffer, Msg_Buffer_Size, fmt, argptr);
	va_end(argptr);

	int index=0;
	while (msg_buffer[index]!=0)
		addch(msg_buffer[index++]);
}

/* Routine to print a buffer of text to screen and word wrap it.
** This will word wrap text to the region specified by
** top-left <-> bottom-right  variables and ask more if needed!
*/
void my_wordwraptext(const char *dptr, int top, int bottom, int left, int right)
{
	bool NOMORE, enablemore=true;
	int i, cx, cy;
	static char oneword[40]={0};

	/* no NULL strings */
	if(!dptr)
		return;

	/* set cursor to start position */
	if(top!=bottom)
		gotoxy(left, top);
	else
		enablemore=false;

	cy=get_cursor_y();
	cx=get_cursor_x();

	drawline_limit(cy, left, right, ' ');
	gotoxy(cx, cy);

	cx=left;
	cy=top;

	NOMORE=false;
	while(!NOMORE)
	{
		oneword[0]=0;
		while((*dptr)<32 && (*dptr)!=0)
		{
			if((*dptr)==13 || (*dptr)==10)
			{
				cx=left;
				if(cy<bottom)
					cy++;
				drawline_limit(cy, left, right, ' ');
				gotoxy(cx, cy);

				if(cy>=bottom-1 && enablemore)
				{
					showmore(false, false);

					for(int j=top; j<bottom; j++)
						drawline_limit(j, left, right, ' ');
					cy=top;
					cx=left;
					gotoxy(left, top);
				}
			}
			if((*dptr)=='\t')
			{
				cx+=4;
				gotoxy(cx, cy);
			}
			if(((*dptr)>=1) && ((*dptr)<=8))
				my_setcolor((*dptr));
			if(((*dptr)>=21) && ((*dptr)<=28))
				my_setcolor((*dptr)-12);
			dptr++;
		}
		/* collect a word until space comes */
  //   	while( !(isspace(*dptr)) && ((*dptr)!=0) )
		oneword[0]=0;
		i=0;

		while((*dptr)>=32)
		{
			oneword[i++]=*dptr;
			oneword[i]=0;
			if((*dptr++)==32)
				break;
		}

		if((*dptr)==0) NOMORE=true;

		if(cx+(int)strlen(oneword) >= right)
		{
			cx=left;
			if(cy<bottom)
				cy++;
			drawline_limit(cy, left, right, ' ');
			gotoxy(cx, cy);
		}

		if(cy>=bottom-1 && enablemore)
		{
			showmore(false, false);
			for(int j=top; j<bottom; j++)
				drawline_limit(j, left, right, ' ');
			cy=top;
			cx=left;
			gotoxy(left, top);
		}
		cx+=(int)strlen(oneword);
		my_printf("%s", oneword);
	}
}

void put_char(char ch, int color)
{
	set_color(color);
	addch(ch);
}

void put_char_to(char ch, const Coord &c)
{
	mvaddch(c.y, c.x, ch);
}

void showcursor()
{
	curs_set(1);
}

void print_centered(int y, const char *txt)
{
	const int x=(SCREEN_COLS/2) - (strlen(txt)/2);
	print_text_to(x, y, txt);	
}

void print_text(const char *txt)
{
	addstr(txt);
}

void print_text_to(int x, int y, const char *txt)
{
	move(y, x);
	
	while (*txt!=0)
		addch(*txt++);	
}

void print_toc(int x, int y, const char *txt, int color)
{
	move(y, x);
	set_color(color);
	addstr(txt);
}

void ww_print(const char *txt)
{
	int slen=0;

	/* wrap until end reached */
	while(txt[slen]!=0)
	{
		//bool wrap=false;
		int wlen=slen;
		while(!isspace(txt[wlen]) && txt[wlen]!=0)
			wlen++;

		/* wrap word if going over right border */
		if (is_over_border(wlen-slen))
		{
			addch('\n');
			//wrap=true;
		}

		/* print the word */
		while(slen!=wlen)
		{
			addch(txt[slen]);
			slen++;
		}

		/* skip blanks */
		while(isspace(txt[slen]) && txt[slen]!=0)
		{
			addch(txt[slen]);
			slen++;
		}
	}
}

void zprintf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnprintf(zmsg_buffer, Msg_Buffer_Size, format, ap);
	va_end(ap);

	ww_print(zmsg_buffer);
}
