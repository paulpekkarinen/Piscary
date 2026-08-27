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
#include "gameview.h"
#include "input.h"
#include "message.h"
#include "msginfo.h"

using std::deque;

Message msg;

Message::Message()
	: Message_Buffer(60), msgdelay(0)
{
	mbuffer=new char[Buf_Size];
}

Message::~Message()
{
	delete[] mbuffer;
}

/* add a message with variable args */
void Message::vnewmsg(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnprintf(mbuffer, Buf_Size, format, ap);
	va_end(ap);

	newmsg(mbuffer, color);
}

/* similar to above, exept that the message will be displayed immendiately */
void Message::vadd(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnprintf(mbuffer, Buf_Size, format, ap);
	va_end(ap);

	newmsg(mbuffer, color);
	notice();
}

void Message::vaddwait(int color, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsnprintf(mbuffer, Buf_Size, format, ap);
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
	vsnprintf(mbuffer, Buf_Size, format, ap);
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

void Message::Add_Dist(int x, int y, const char *mess1, int color1,
	const char *mess2, int color2)
{
	if (gameview.Is_Visible(x, y))
	{
		if (mess1)
			newmsg(mess1, color1);
	}
	else
		if (mess2)
			newmsg(mess2, color2);
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
	vsnprintf(mbuffer, Buf_Size, format, ap);
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

void Message::setdelay(int delaytime)
{
	msgdelay=delaytime;
}
