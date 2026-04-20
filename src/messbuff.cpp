//Legend of Saladir - message.cpp

//Refactored 5.12.2022 - 30.9.2024 by Paul K. Pekkarinen

#include "display.h"
#include "input.h"
#include "messbuff.h"
#include "msginfo.h"
#include "options.h"
#include "output.h"

using std::deque;

const char *Message_Buffer::Nullmsg="Null pointer passed to Message::newmsg()";
const char *Message_Buffer::Shortmsg=
	"Zero length message passed to Message::newmsg()";

Message_Buffer::Message_Buffer(int maxmess)
	: Max_Messages(maxmess), messages_per_turn(0)
{

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
	if (oldest!=0 && CONFIGVARS.compactmessages)
	{
		if (oldest->Is_Match(message))
			return; //no new messages added
	}

	messages.push_back(new Msginfo(message, color, delay));
	messages_per_turn++;

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
	messages_per_turn=0;
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
