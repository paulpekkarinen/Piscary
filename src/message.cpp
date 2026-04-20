/**************************************************************************
 * message.cpp --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 15.7.2022 - 12.4.2026 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cctype>
#include <cstdarg>
#include <cstring>
#include "caves.h"
#include "gameview.h"
#include "codex.h"
#include "input.h"
#include "message.h"
#include "msginfo.h"
#include "output.h"

using std::deque;

Message msg;

Message::Message()
	: Message_Buffer(60),
	msgdelay(0), last(0, 0), oneword{0}
{
	mbuffer=new char[1024];
}

Message::~Message()
{
	Clear();
	delete[] mbuffer;
}

/* add a message with variable args */
void Message::vnewmsg(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(mbuffer, format, ap);
#endif
	va_end(ap);

	newmsg(mbuffer, color);
}

/* similar to above, exept that the message will be displayed immendiately */
void Message::vadd(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(mbuffer, format, ap);
#endif
	va_end(ap);

	newmsg(mbuffer, color);
	notice();
}

void Message::vaddwait(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(mbuffer, format, ap);
#endif
	va_end(ap);

	newmsg(mbuffer, color);
	notice();
	showmore(false, true);
	update();
}

void Message::vnewdist(level_type *level, int x, int y,
	int color2, int color1,
	const char *message2,
	const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(mbuffer, format, ap);
#endif
	va_end(ap);

	if (gameview.Is_Visible(x, y))
	{
		newmsg(mbuffer, color1);
	}
	else
	{
		if (message2)
			newmsg(message2, color2);
	}
}

/*
** Add a new message to the queue. If location x, y can see to the
** location of player -> add message . Else add message2
*/
void Message::add_dist(level_type *level, int x, int y, const char *message,
	int color, const char *message2, int color2)
{
	//note: is this distance calculation useful, it was commented out
	// Coord src(x, y);
	//   if(cansee(level, src, player.Get_Location(), 10, true)) {
	if (gameview.Is_Visible(x, y))
	{
		if (message)
			newmsg(message, color);
	}
	else
		if (message2)
			newmsg(message2, color2);
}

/* add a new message, notice immendiately */
void Message::add(const char *message, int color)
{
	newmsg(message, color);
	notice();
}

/* add a new message, notice immendiately */
void Message::addwait(const char *message, int color)
{
	newmsg(message, color);
	notice();
	showmore(false, true);
	update();
}

void Message::newmsg(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(mbuffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(mbuffer, format, ap);
#endif
	va_end(ap);

	newmsg(mbuffer, color);
}

void Message::newmsg(const char *message)
{
	Add(message, C_WHITE, msgdelay);
}

void Message::newmsg(const char *message, int color)
{
	Add(message, color, msgdelay);
}

void Message::newmsg(std::string &s, int color)
{
	newmsg(s.c_str(), color);
}

void Message::notice()
{
	static char countstr[20];

	deque<Msginfo*>::iterator ii = messages.end()-messages_per_turn;

	//show messages that were collected in one turn
	while (ii!=messages.end())
	{
		gotoxy(last.x, last.y);

		const char *dptr=(*ii)->Get_Raw_String();
		int index=0;
		bool looping=true;

		while (looping)
		{
			int i=0;

			//first skip any spaces
			while (isspace(dptr[index])!=0 && dptr[index]!=0)
				index++;

			//collect a word until space comes
			while (isspace(dptr[index])==0 && dptr[index]!=0)
				oneword[i++]=dptr[index++];

			if (dptr[index]==0)
			{
				const int msg_count=(*ii)->Get_Count();

				if (msg_count>1)
				{
					sprintf(countstr, "(x%d)", msg_count);
					strcat(oneword, countstr);
				}
				else oneword[i]=0;

				looping=false;
			}
			else
				oneword[i]=0;

			if (get_cursor_y()==0 && is_over_border(strlen(oneword)))
			{
				my_printf("\n");
				clearline(get_cursor_y());
			}

			if ((get_cursor_x()+strlen(oneword) > (unsigned int)(SCREEN_COLS-6))
				&& (get_cursor_y()==1))
			{
				showmore(false, true);

				for (i=0; i<MSGLINES; i++)
					clearline(MSGLINE+i);

				gotoxy(0, MSGLINE);
			}

			(*ii)->Set_Color();

			my_printf("%s ", oneword);
		}

		//my_printf(" ");

		last=get_cursor();

		refresh();

		const int d=(*ii)->Get_Delay_Time();
		if (d>0) delay(d);

		++ii; //handle next message until the end of list
	}

	messages_per_turn=0;
}

void Message::showall()
{
	if (Is_Empty())
		newmsg("Message buffer is empty.", C_WHITE);
	else
		Show_All("Previous messages");
}

void Message::showlast()
{
	Msginfo *m=Get_Last_Message();

	if (m!=0)
	{
		update();
		m->Show_Message();
	}
}

void Message::update()
{
	if (last.x!=0 || last.y!=MSGLINE)
	{
		last.Set(0, MSGLINE);
		for (int i=0; i<MSGLINES; i++)
			clearline(MSGLINE+i);
	}
}

void Message::setdelay(int delaytime)
{
	msgdelay=delaytime;
}
