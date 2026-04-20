//Legend of Saladir - verse.cpp

#include "dice.h"
#include "verse.h"

#include "saldebug.h"

using std::string;

Verse::Verse()
	: index(0)
{

}

void Verse::Add(string &txt)
{
	fragments.push_back(txt);
}

const char *Verse::Get()
{
	return fragments[0].c_str();
}

//Get from index 'i', check illegal index.
const char *Verse::Get_At(int i)
{
	if (i>=0 && i<Get_Size())
		return fragments[i].c_str();

	return "poopie";	
}

//Get from current index, then move to next piece and reset if end reached.
const char *Verse::Get_Chained()
{
	const char *rv=fragments[index].c_str();
	index++;
	if (index>=Get_Size())
		index=0;

	return rv;
}

const char *Verse::Get_Random()
{
	int i=0;
	const int sz=Get_Size();

	if (sz>1)
		i=random_number(0, sz-1);

	return fragments[i].c_str();
}

int Verse::Get_Size()
{
	return (int)fragments.size();
}

void Verse::Show_Data()
{
	for (int t=0; t<Get_Size(); t++)
	{
		debug->Message("%d: %s", t, fragments[t].c_str());	
	}
}

