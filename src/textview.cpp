/*
 * File text viewer
 */

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cctype>
#include <cstring>
#include "display.h"
#include "game.h"
#include "input.h"
#include "options.h"
#include "output.h"
#include "textview.h"

#define BUFSIZE 512

char *displaylines(char *txt, char *begintxt, int8u);
char *findbottom(char *txt, int32u *lines);
char *backline(char *start, char *curtxt);
char *nextline(char *, char *);
int32u processtext(char *txt);

char *findbottom(char *txt, int32u *lines)
{
	*lines=0;
	while (*txt!=0)
	{
		if (*txt=='\n')
			(*lines)++;

		txt++;
	}

	return txt;
}

void viewtext(const char *input_txt)
{
	const int textsize=strlen(input_txt)+1;
	//convert to non-const text for the old routine...
	//note: this could be changed maybe later
	char *txt=new char[textsize];
	strcpy(txt, input_txt);

	char *curtxt, *topline, *bottom, *back, *bottomline;
	int16u key, i, perc;
	int32u linecount=0, cline=0;
	bool nocmd=false;

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	clear_screen();
	enable_scroll();

	processtext(txt);

	bottom=findbottom(txt, &linecount);

	curtxt=txt;
	topline=curtxt;
	curtxt=displaylines(curtxt, txt, SCREEN_LINES-2);
	bottomline=curtxt;

	while (1)
	{
		nocmd=false;

		display->Footer(
			"[HOME/t Top]-[END/b End]-[PGDN/+ NextPg]-[PGUP/- PrevPg]-[q Quit]",
			CH_YELLOW);

		gotoxy(0, SCREEN_LINES);
		perc=(int16u(((real)(SCREEN_LINES-2+cline)/(real)linecount)*100));
		if (perc>100)
			perc=100;
		my_printf("Line %4ld/%4ld (%3d%%)", cline, linecount, perc);

		key=my_getch();

		if (key=='8' || key==KEY_UP)
		{
			nocmd=true;
			if (topline>txt)
			{
				clearline(SCREEN_LINES-1);
				clearline(SCREEN_LINES);
				topline=backline(txt, topline);
				bottomline=backline(txt, bottomline);
				scrl(-1);
				gotoxy(0, 0);
				displaylines(topline, txt, 1);
				cline--;
			}
		}
		else if (key=='2' || key==KEY_DOWN)
		{
			nocmd=true;
			if (bottomline<bottom)
			{
				clearline(SCREEN_LINES-2);
				clearline(SCREEN_LINES-1);
				gotoxy(0, SCREEN_LINES-2);
				topline=nextline(topline, bottom);
				bottomline=displaylines(bottomline, txt, 1);
				scrl(1);
				cline++;
			}

		}
		else if (key=='t' || (key==KEY_HOME && topline>txt))
		{
			curtxt=txt;
			cline=0;
		}
		else if (key=='b' || (key==KEY_END && bottomline<bottom))
		{
			curtxt=bottom;
			cline=linecount;
			for (i=0; i<SCREEN_LINES-2; i++)
			{
				back=curtxt;
				curtxt=backline(txt, curtxt);
				if (curtxt<back)
					cline--;
				else
					break;
			}
		}
		else if (key=='+' || key==KEY_NPAGE || key==' ' || key=='3')
		{
			if (bottomline>=bottom)
			{
				//	    curtxt=topline;
				nocmd=true;
			}
			else
			{
				curtxt=topline;
				for (i=0; i<SCREEN_LINES-3; i++)
				{
					curtxt=nextline(curtxt, bottom);
					back=bottomline;
					bottomline=nextline(bottomline, bottom);
					cline++;
					if (bottomline==back)
						break;
				}
			}
		}
		else if (key=='-' || key==KEY_PPAGE || key=='9')
		{
			if (topline > txt)
			{
				curtxt=topline;
				for (i=0; i<SCREEN_LINES-3; i++)
				{
					back=curtxt;
					curtxt=backline(txt, curtxt);
					if (curtxt<back)
						cline--;
				}
			}
			else
			{
				//	    curtxt=topline;
				nocmd=true;
			}
		}
		else if (key=='q' || key==KEY_ENTER ||
			key==PADENTER || key==KEY_ESC)
				break;
		else
		{
			//	 curtxt=topline;
			nocmd=true;
		}

		if (!nocmd)
		{
			clear_screen();
			topline=curtxt;
			curtxt=displaylines(curtxt, txt, SCREEN_LINES-2);
			bottomline=curtxt;
		}
	}

	disable_scroll();
	delete[] txt;
}

char *nextline(char *curtxt, char *bottom)
{
	char *val;

	val=curtxt;
	while (*val!='\n' && val<bottom)
	{
		if (*val==0)
			return curtxt;
		val++;
	}

	val++;

	if (val>=bottom)
		return curtxt;
	else
		return val;
}

char *backline(char *start, char *curtxt)
{
	bool fl=false;

	curtxt--;
	while (curtxt>start)
	{

		if (*curtxt == '\n')
		{
			if (fl)
				break;
			fl=true;
		}

		curtxt--;
	}

	if (curtxt>start)
		return curtxt+1;
	else
		return start;
}

int32u processtext(char *txt)
{
	char *spc=NULL;
	int32u cy=0, cx=1, wlen;

	while (*txt!=0)
	{
		wlen=0;

		while (!isspace(txt[wlen]) && txt[wlen]!=0)
			wlen++;

		if ((cx+wlen)>=(int32u)SCREEN_COLS && spc!=NULL)
		{
			*spc='\n';
			wlen=0;
			cx=1;
			cy++;
		}

		if (*txt==' ')
			spc=txt;

		if (*txt=='\n')
		{
			cx=1;
			cy++;
		}

		if (wlen)
		{
			txt+=wlen;
			cx+=wlen;
		}
		else
		{
			txt++;
			cx++;
		}
	}

	return cy;
}

char *displaylines(char *txt, char *begintxt, int8u numlines)
{
	char *dptr;
	int32s wlen, slen;
	int8u lc=0;
	bool wrap;

	static int16u lcolor=C_WHITE;

	if (!txt)
	{
		my_printf("Error: textpointer null at displaybuffer(char *txt)\n");
		return NULL;
	}

	if (!CONFIGVARS.colortext)
		lcolor=C_WHITE;

	dptr=txt;

	lc=0;
	slen=0;
	wlen=0;
	my_setcolor(lcolor);

	/* wrap until end reached */
	while (dptr[slen]!=0)
	{
		wrap=false;
		wlen=slen;
		while (dptr[wlen]>32 && !isspace(dptr[wlen]))
			wlen++;

		/* wrap word if going over right border */
		if (is_over_border(wlen-slen))
		{
			addch('\n');
			wrap=true;
			lc++;

			if (lc>=numlines)
				return &dptr[slen];
		}

		/* print the word */
		while (slen<wlen && dptr[slen]!=0)
		{
			addch(dptr[slen]);
			slen++;
		}

		/* color support, STUPID but works */
		if (dptr[slen]>0 && dptr[slen]<8)
		{
			if (CONFIGVARS.colortext)
			{
				lcolor=(int16u)dptr[slen];
				my_setcolor(lcolor);
			}
			slen++;
		}
		else if (dptr[slen]>=20 && dptr[slen]<28)
		{
			if (CONFIGVARS.colortext)
			{
				lcolor=(int16u)(dptr[slen]-12);
				my_setcolor(lcolor);
			}
			slen++;
		}
		/* skip blanks */
		else if (isspace(dptr[slen]) && dptr[slen]!=0)
		{
			/* strip carriage returns or else dos will get wild */
			if (dptr[slen]!='\r')
				addch(dptr[slen]);

			if (get_cursor_x() >= SCREEN_COLS)
			{
				addch('\n');
				lc++;
			}
			else if (dptr[slen]=='\n')
				lc++;

			slen++;

		}
		else
			slen++;

		if (lc>=numlines)
			break;
	}

	return &dptr[slen];
}
