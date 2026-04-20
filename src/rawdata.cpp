//Legend of Saladir - rawdata.cpp

#include <fstream>
#include "lexicon.h"
#include "rawdata.h"

#include "saldebug.h"

using std::string;

Rawdata::Rawdata(const char *fn, const int numtags)
	: Number_Of_Tags(numtags), tag_offsets(0), index(0)
{
	tag_offsets=new int[numtags];

	std::ifstream file(fn, std::ios::binary);

	working=false;

	//load data to 'text' string
	if (file.is_open())
	{
		char ch='?';
		char last='?';
		bool pending_space=false;
		
		while (file.get(ch))
		{
			//store only letter ascii data, but check line feed codes
			if (ch<32)
			{
				if (ch==10 || ch==13)
					pending_space=true;

				continue;
			}

			//transform /r and/or /n to space in texts with multiple
			//lines, but only if the next index start with [ is not
			//found, or last char was not | (end of text piece)
			if (pending_space)
			{
				if (ch!='[' && last!='|')
					text.push_back(' ');
					
				pending_space=false;
			}
				
			text.push_back(ch);

			last=ch;
		}

		bytes=(int)text.size();

		debug->Message("%d bytes stored from script data in %d tags.",
			bytes, Number_Of_Tags);

		//check the wanted size of the data file
		//if (bytes==16616)
		//	working=true;

		working=true;
	}

	//if working for now, set offsets to each data block
	if (working)
	{
		//if finding offsets somehow fails, invalidate the database
		working=Find_Offsets();
	}
}

Rawdata::~Rawdata()
{
	delete[] tag_offsets;
}

bool Rawdata::Find_Offsets()
{
	int tags=0;

	//search the database buffer by using the now set size
	for (int t=0; t<bytes; t++)
	{
		//find and parse index number of text piece
		if (text[t]=='[')
		{
			t++;
			string s;
			while (text[t]!=']')
			{
				s.push_back(text[t++]);
			}

			const int id=from_string(s);

			tag_offsets[id]=t+1; //+1 = skip ]

			tags++;
		}
	}

	//debug->Message("Script tags found %d, number of tags is %d.", tags, Number_Of_Tags);

	if (tags!=Number_Of_Tags)
		return false;

	return true;
}

char Rawdata::Get_Next_Char()
{
	//end of file reached
	if (index>=bytes)
		return 0;

	char ch=text[index];

	//if next text piece reached
	if (ch=='[')
		return 0;
	
	index++;
	return ch;
}

void Rawdata::Set_Index(int tag)
{
	//starting point of a tag
	index=tag_offsets[tag];
}

