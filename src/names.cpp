/**************************************************************************
 * names.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 20.8.2021 - 6.9.2025 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstring>
#include "being.h"
#include "body.h"
#include "caves.h"
#include "classes.h"
#include "dice.h"
#include "gameview.h"
#include "invnode.h"
#include "lexicon.h"
#include "names.h"
#include "output.h"

using std::string;

void the_creature_name(string &s, const char *gender, const char *desc, const char *prof);

string player_killer_str;

const char *name_syllables[]=
{
 "jas", "min", "on", "bal", "ta", "zar",
 "men", "thol", "su", "vi", "er", "no",
 "spar", "hawk", "ke", "mi", "ber", "he",
 "dos", "sal", "adir", "na", "tash", "ha",
 "inka", "cae", "sar", "sha", "tho",
 "bel", "gar", "ion", "ath", "pol", "gara",
 "sau", "ron", "gand", "alf", "bil", "bo",
 0
};

const char *syllables[]=
{
   "a", "am", "ain", "aro", "anx", "ax", "ayx",
   "boo", "blah", "benz", "in", "gaz", "olr", "ne",
   "cha", "too", "min", "olth", "rez", "read", "me", "elp",
   "ix", "moron", "utns", "alm", "elm", "ilm", "dona", "dina",
   "six", "in", "nid", "nim", "hop", "melz", "janz", "as",
   "yrn", "asm", "min", "tho", "mas", "moon", "lite", "ern", "otu",
   "omain", "en", "sal", "adir", "leg", "end", "dom", "ains",
   "cux", "bet", "ter", "ton", "not", "to", "tak", "mut",
   "de", "dip", "wat", "rand", "xo", "mel", "pid",
   "eiz", "ex", "epat", "ena", "ble", "fuc", "byt", "me",
   "ux", "ex", "pox", "xep", "soc", "ra", "tes", "uni", "hop",
   0
};

const char *keyname_func[]=
{
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	NULL
};

const char *keyname_ctrl[]=
{
	"?-?",
	"C-a",
	"C-b",
	"C-c",
	"C-d",
	"C-e",
	"C-f",
	"C-g",
	"C-h",
	"C-i",
	"C-j",
	"C-k",
	"C-l",
	"C-m",
	"C-n",
	"C-o",
	"C-p",
	"C-q",
	"C-r",
	"C-s",
	"C-t",
	"C-u",
	"C-v",
	"C-w",
	"C-x",
	"C-y",
	"C-z",
	"ESC",
	NULL
};

char *generate_scrollname()
{
	int snum=0;
	int numsyll=0;

	while (syllables[numsyll]) numsyll++;

	int num=2+RANDU(6);

	for (int i=0; i<num; i++)
	{
		my_printf("%s", syllables[RANDU(numsyll)]);
		if (snum==(1+RANDU(2)))
		{
			if (RANDU(100) < 5)
				my_printf("'");
			else
				my_printf(" ");
			snum=0;
		}
		snum++;
	}
	my_printf("\n");

	return 0;
}

/*
 * My replacement for a routine which returns a (nicer) keyname for
 * given keycode. Final rescue comes with curses unctrl().
 */
const char *get_keyname(int key)
{
	//the usual suspects
	if (key==KEY_ESC)
		return "Esc";

	if (key==KEY_ENTER)
		return "Return";

	//ctrl keys
	if (key<28)
		return keyname_ctrl[key];

	//ascii characters, excluding space
	if (key>32 && key<128)
		return unctrl(key);

	//function keys
	if (key>=MYKEY_CURSESF1 && key<=MYKEY_CURSESF20)
		return keyname_func[key-MYKEY_CURSESF1];

	const char *rv=0;

	//special cases and default name with curses' keyname
	switch (key)
	{
		case ' ': rv="Space"; break;
		case KEY_DOWN: rv="Down"; break;
		case KEY_UP: rv="Up"; break;
		case KEY_LEFT: rv="Left"; break;
		case KEY_RIGHT: rv="Right"; break;
		case KEY_BACKSPACE: rv="Backspace"; break;
		default: rv=keyname(key); break;
	}

	return rv;
}

const char *get_player_killer()
{
	return player_killer_str.c_str();
}

string item_name(invnode *iptr)
{
	string s("A ");
	if (iptr->count > 1)
	{
		s.append("pile of ");
		s.append(iptr->i.name);
		s.append("s");
	}
	else
		s.append(iptr->i.name);

	return s;
}

/* sprintf for monster data */
/* generates a monster name to the destination string... */
/* "the female snake" or "Bill Gates", etc... */
string monster_sprintf(Actor *mptr, bool upcase, bool nameonly)
{
	if (gameview.Is_Visible(mptr->x, mptr->y)==false)
	{
		if (upcase) return "Something";
		return "something";
	}

	string s;
	Gender gen(mptr->m.gender);
	const char *gender_name=gen.Get_Name();

	if(npc_races[mptr->m.race].behave & BEHV_ANIMAL)
	{
		s.append("the ");
		s.append(mptr->m.desc);
	}
	else if(mptr->m.status & MST_KNOWN)
	{
		if(mptr->m.name.size()>0)
		{
			if(nameonly)
				s.append(mptr->m.name);
			else
			{
				if(!mptr->m.special && mptr->Is_Shopkeeper()==false)
				{
					s.append(mptr->m.name);
					s.append(", ");
					the_creature_name(s,
						gender_name,
						mptr->Get_Description(),
						classes[mptr->m.mclass].name);
				}
				else
				{
					s.append(mptr->m.name);
					s.append(", ");
					the_creature_name(s,
						gender_name,
						mptr->Get_Description(),
						0);
				}
			}
		}
		else
		{
			if(!mptr->m.special && mptr->Is_Shopkeeper()==false)
			{
				the_creature_name(s,
					gender_name,
					mptr->Get_Description(),
					classes[mptr->m.mclass].name);
			}
			else
			{
				the_creature_name(s,
					gender_name,
					mptr->Get_Description(),
					0);
			}
		}
	}
	else
	{
		if(!mptr->m.special && mptr->Is_Shopkeeper()==false)
		{
			the_creature_name(s,
				gender_name,
				mptr->Get_Description(),
				classes[mptr->m.mclass].name);
		}
		else
		{
			the_creature_name(s,
				gender_name,
				mptr->Get_Description(),
				0);
		}
	}

	if (upcase)
	{
		s[0]=uppercase(s[0]);
	}

	return s;
}

void player_killer(const char *txt)
{
	player_killer_str=txt;
}

bool random_language(string &str, int maxlen)
{
	int numsyll=0;

	if (str.empty()==false)
		return false;

	maxlen--;

	if (maxlen<=0)
		return false;

	while (syllables[numsyll])
		numsyll++;

	if (!numsyll)
		return false;

	int num=4+RANDU(6);
	int num2=num;
	int space=1;
	int spec=0;

	while (maxlen>0)
	{
		int cur=RANDU(numsyll);

		if ((1+RANDU(100)) < 10)
		{
			if (!spec && num!=num2)
			{
				spec++;
				str.append("'");
				maxlen--;
				continue;
			}
		}
		if ((1+RANDU(100)) < 30 && !space)
		{
			space=1;
			str.append(" ");
			maxlen--;
			continue;
		}
		maxlen-=(int)strlen(syllables[cur]);

		if (maxlen<0 || num==0)
			return true;

		space=0;
		str.append(syllables[cur]);
		num--;
	}
	return true;
}

bool random_name(std::string &str, int maxlen)
{
	int numsyll=0;

	if (str.empty()==false)
		return false;

	maxlen--;
	if (maxlen<=0)
		return false;

	while (name_syllables[numsyll])
		numsyll++;

	if (!numsyll)
		return false;
	str[0]=0;

	int num=2+RANDU(3);

	while (maxlen>0)
	{
		int cur=RANDU(numsyll);

		maxlen-=(int)strlen(name_syllables[cur]);
		if (maxlen<0 || num==0)
			return true;

		str.append(name_syllables[cur]);
		num--;
	}

	return true;
}

bool random_name(char *buffer, int maxlen)
{
	string s;
	if (random_name(s, maxlen)==false) return false;
	
	my_strcpy(buffer, s.c_str(), maxlen);
	return true;
}

bool random_scrollname(char *str, int maxlen)
{
	if (!str)
		return false;

	maxlen--;
	if (maxlen<=0)
		return false;

	int numsyll=0;
	while (syllables[numsyll])
		numsyll++;

	if (!numsyll)
	{
		my_strcpy(str,"error",maxlen);
		return 0;
	}

	str[0]=0;

	int num=1+RANDU(3)+RANDU(3);

	while (maxlen>0)
	{
		const int cur=RANDU(numsyll);

		maxlen-=(int)strlen(syllables[cur]);
		if (maxlen<=0 || num==0)
			return true;

		if (RANDU(100)<10 && strlen(str)>0)
		{
			maxlen--;
			strcat(str," ");
		}

		strcat(str,syllables[cur]);
		num--;
	}

	return true;
}

void the_creature_name(string &s, const char *gender, const char *desc, const char *prof)
{
	s.append("the ");

	s.append(gender);
	s.append(" ");

	s.append(desc);

	if (prof!=0)
	{
		s.append(" ");
		s.append(prof);
	}
}
