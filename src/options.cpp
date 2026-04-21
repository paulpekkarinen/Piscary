//options.cpp - game options

//Refactored 1.10.2021 - 19.4.2026 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cctype>
#include <cstring>
#include "gametime.h"
#include "input.h"
#include "lexicon.h"
#include "options.h"
#include "output.h"
#include "storage.h"

using std::string;

/* place where options are stored */
gameconfig CONFIGVARS;

//for text input, need 21 characters for item categories + trailing zero
char opt_txt[22]={};

int Option::Get_Input_Offset()
{
	//return location where new value is input
	return strlen(name)+6;
}

void Option::Show()
{
	zprintf("%c) %s : ", letter, name);
	Show_Value();
}

void Option::Show_Description()
{
	zprintf("%s", desc);
}

void Bool_Option::Change()
{
	if (value)
		value=false;
	else
		value=true;
}

void Bool_Option::Show_Value()
{
	if (value)
		print_text("true");
	else
		print_text("false");
}

void Bool_Option::Load(Tar_Ball &tb)
{
	value=tb.Get_Next_Bool();	
}

void Bool_Option::Save(Tar_Ball &tb)
{	
	tb.Put_Bool(value);
}
	
void Integer_Option::Change()
{
	my_gets(opt_txt, sizeof(opt_txt)-1);
	value=atoi(opt_txt);

	if (value<min_value)
		value=min_value;
	if (value>max_value)
		value=max_value;
}

void Integer_Option::Show_Value()
{
	my_printf("%d (%d - %d)", value, min_value, max_value);
}

void Integer_Option::Load(Tar_Ball &tb)
{
	value=tb.Get_Next_Value();		
}

void Integer_Option::Save(Tar_Ball &tb)
{
	tb.Put(value);
}

void Array_Option::Change()
{
	my_gets(arr, size);
}

void Array_Option::Show_Value()
{
	for (int i=0; i<size; i++)
	{
		const char ch=arr[i];
		if (ch==0) break;
		addch(ch);
	}
}

void Array_Option::Load(Tar_Ball &tb)
{
	//load array as string
	string joukko=tb.Get_Next_String();

	//this should work relatively safely...
	sprintf(arr, "%s", joukko.c_str());
}

void Array_Option::Save(Tar_Ball &tb)
{
	//save array as string
	string joukko(arr);
	tb.Put_String(joukko);
}

//Initialize configuration variables
void gameconfig::Reset()
{
	anykeymore=false;
	autodoor=true;
	autopickup=false;
	autopush=true;
	colormessages=true;
	colortext=true;
	compactmessages=true;
	door_disturb=true;
	droppiles=false;
	foodwarn=true;
	getallmoney=true;	
	health_alarm=50;
	item_disturb=true;
	monster_disturb=true;
	my_strcpy(pickuptypes, "$%?", sizeof(pickuptypes)-1);
	repeatcount=50;
	repeatupdate=true;
	stair_disturb=true;
	targetline=true;
	ticksperminute=TIME_1MIN;
}
