//Legend of Saladir - msginfo.cpp

//Refactored 5.12.2022 - 30.9.2024 by Paul K. Pekkarinen

#include "msginfo.h"
#include "lexicon.h"
#include "options.h"
#include "output.h"

bool Msginfo::Is_Match(const char *src)
{
	const bool rv=is_same_string(msg, src);
	
	if (rv)
		count++;

	return rv;
}

void Msginfo::Set_Color()
{
	//use color in messages if the user wants so
	int c;
	
	if (CONFIGVARS.colormessages)
		c=color;
	else
		c=C_WHITE;

	set_color(c);
}

void Msginfo::Show_In_List(int i)
{
	set_color(C_WHITE);
	my_printf("%2u) %s", i+1, msg.c_str());

	if (count>1)
		my_printf(" (x%u)", count);

	my_printf("\n");
}

void Msginfo::Show_Message()
{
	print_toc(0, MSGLINE, msg.c_str(), color);
}
