//Legend of Saladir - script.cpp

#include <string>
#include "message.h"
#include "rawdata.h"
#include "script.h"
#include "verse.h"

#include "saldebug.h"

using std::string;

Script::Script()
{
	Rawdata rawwie("scripts.txt", Amt_Of_Scripts);

	working=rawwie.Is_Working();

	if (working)
	{
		//add each piece of text to Verse
		for (int t=0; t<Amt_Of_Scripts; t++)
		{
			bool processing=true;
			string s;

			//set starting location of text piece
			rawwie.Set_Index(t);

			while (processing)
			{
				const char ch=rawwie.Get_Next_Char();

				if (ch=='|' || ch==0)
				{
					//add text piece collected from letters
					articles[t].Add(s);

					if (ch==0)
						processing=false;
					else
						s.clear(); //clear for next piece	
				}
				else
					s.push_back(ch);
			}
		}
	}
	else
		debug->Message("Script loading failed.");
}

//Sometimes needed when selecting a random amount as integer.
int Script::Get_Items(int id)
{
	return articles[id].Get_Size();
}

const char *Script::Get_At_Index(int id, int index)
{
	return articles[id].Get_At(index);
}

const char *Script::Get_Random(int id)
{
	return articles[id].Get_Random();
}

void Script::Chain_Message(int id, int color)
{
	msg.newmsg(articles[id].Get_Chained(), color);
}

void Script::Index_Message(int id, int index)
{
	msg.newmsg(articles[id].Get_At(index), C_WHITE);
}

void Script::Message(int id, int color)
{
	msg.newmsg(articles[id].Get(), color);
}

void Script::Random_Message(int id)
{
	msg.newmsg(articles[id].Get_Random(), C_WHITE);
}

void Script::Random_Message(int id, int color)
{
	msg.newmsg(articles[id].Get_Random(), color);
}

void Script::Debug(int id)
{
	articles[id].Show_Data();
}

