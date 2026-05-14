//Legend of Saladir - message.cpp

//Refactored 5.12.2022 - 30.9.2024 by Paul K. Pekkarinen

#include <cstring>
#include "codex.h"
#include "display.h"
#include "input.h"
#include "messbuff.h"
#include "msginfo.h"
#include "options.h"
#include "output.h"

using std::deque;
using std::string;

const char *Message_Buffer::Nullmsg="Null pointer passed to Message::newmsg()";
const char *Message_Buffer::Shortmsg=
	"Zero length message passed to Message::newmsg()";

Message_Buffer::Message_Buffer(int maxmess)
	: Max_Messages(maxmess), countstr{0}
{
	origin=messages.end();
}

Message_Buffer::~Message_Buffer()
{
	Clear();
}

Msginfo *Message_Buffer::Get_Last_Message()
{
	if (messages.empty()) return 0;
	return messages.back();
}

bool Message_Buffer::Is_Empty()
{
	return messages.empty();
}

void Message_Buffer::Add(const char *message, int color, int delay)
{
	if (!message)
	{
		color=CHB_RED;
		message=Nullmsg;
	}
	else if (message[0]==0)
	{
		color=CHB_RED;
		message=Shortmsg;
	}

	Msginfo *oldest=Get_Last_Message();

	/* handle duplicate messages */
	if (oldest!=0) // && CONFIGVARS.compactmessages) //note: possibly remove this option
	{
		if (oldest->Is_Match(message))
		{
			Set_Origin(oldest); //shows this message again
			return; //no new messages added
		}
	}
	
	oldest=new Msginfo(message, color, delay);
	messages.push_back(oldest);

	Set_Origin(oldest);
	
	while (messages.size()>=Max_Messages)
	{
		Msginfo *m=messages.front();
		delete m;
		messages.pop_front();
	}
}

void Message_Buffer::Clear()
{
	for (mitr ii = messages.begin(); ii != messages.end(); ++ii)
	{
		delete (*ii);
	}
	messages.clear();
	origin=messages.end();
}

void Message_Buffer::notice()
{
	//show messages that were collected in one turn
	while (origin!=messages.end())
	{
		gotoxy(last.x, last.y);

		string viesti=(*origin)->Get();
		const int vlen=(int)viesti.size();
		string oneword;
		int index=0;
		bool looping=true;

		while (looping)
		{
			//collect a word until space comes
			while (viesti[index]!=' ' && index<vlen)
				oneword+=viesti[index++];

			index++; //skip that space

			//end of message, add count if repeated
			if (index>=vlen)
			{
				const int msg_count=(*origin)->Get_Count();

				if (msg_count>1)
				{
					sprintf(countstr, " (x%d)", msg_count);
					oneword.append(countstr);
				}

				looping=false;
			}

			//check if the word goes over the width of message area
			const int cy=get_cursor_y();
			const int wordsize=(int)oneword.size();

			if (cy==MSGLINE && is_over_border(wordsize))
				gotoxy(0, MSGLINE+1);

			if (cy==MSGLINE+1 && (get_cursor_x()+wordsize > (SCREEN_COLS-6)))
			{
				showmore(false, true);
				update();
			}

			//display one word of the message
			(*origin)->Set_Color();
			my_printf("%s ", oneword.c_str());
			oneword.clear();
		}

		//move to next message until the end of list
		last=get_cursor();
		refresh();

		const int d=(*origin)->Get_Delay_Time();
		if (d>0) delay(d);

		++origin;
	}
}

void Message_Buffer::Set_Origin(Msginfo *here)
{
	//new origin already set
	if (origin!=messages.end())
		return;
		
	for (mitr ii = messages.begin(); ii != messages.end(); ++ii)
	{
		if ((*ii)==here)
		{
			origin=ii;
			break;
		}
	}	
}

void Message_Buffer::Show_All(const char *header)
{
	int i=0;
	int cy=2;

	display->Header(header, CH_GREEN);
	goto_content();

	mitr index = messages.begin();

	while (index!=messages.end())
	{
		if (cy==SCREEN_LINES-1)
		{
			wait_more_lines(cy, "More");
			cy=2;

			display->Header(header, CH_GREEN);
			goto_content();
		}

		(*index)->Show_In_List(i);

		i++;
		cy++;
		++index;
	}

	wait_more_lines(SCREEN_LINES, "Press a key");
}

void Message_Buffer::update()
{
	if (last.x!=0 || last.y!=MSGLINE)
	{
		last.Set_Location(0, MSGLINE);
		for (int i=0; i<MSGLINES; i++)
			clearline(MSGLINE+i);

		gotoxy(last.x, last.y);
	}
}
