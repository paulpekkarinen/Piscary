/**************************************************************************
 * traps.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 25.9.2021 - 21.9.2025 Paul K. Pekkarinen

#include "dice.h"
#include "gameview.h"
#include "input.h"
#include "output.h"
#include "storage.h"
#include "traps.h"

using std::string;

Trap_Data list_traps[]=
{
	{"no trap", '?', C_WHITE, 0}, //this should not be displayed
	{"disarmed", '^', C_WHITE, 0},
	{"boulder", '~', C_RED, TRAPF_DOORTRAP},
	{"bomb", '^', C_RED, TRAPF_DOORTRAP|TRAPF_CAVETRAP},
	{"electric shock", '^', CH_BLUE, TRAPF_CAVETRAP|TRAPF_NOREMOVE},
	{NULL, 0}
};

//return this if trap no found from the list
Trap dummy_trap;

Trap::Trap()
	: type(TRAP_NOTRAP), damage(0)
{

}

Trap::Trap(int tt, const Coord &c)
	: type(tt), damage(throwdice(4, 4, 1)), pos(c)
{

}

Coord &Trap::Get_Location()
{
	return pos;
}

void Trap::Show_Data()
{
	my_printf("%s (damage: %d) at %d, %d\n",
		list_traps[type].name, damage, pos.x, pos.y);
}

void Trap::Save(Tar_Ball &tb)
{
	tb.Put(type);
	tb.Put(damage);
	pos.Save(tb);
}

void Trap::Load(Tar_Ball &tb)
{
	type=tb.Get_Next_Value();
	damage=tb.Get_Next_Value();
	pos.Load(tb);
}

//===

Trap &Traps::Get(const Coord &c)
{
	for (trap_iter i=traplist.begin(); i!=traplist.end(); ++i)
	{
		if((*i).Get_Location()==c)
		{
			return (*i);
		}
	}

	//this shouldn't happen, but just in case
	return dummy_trap;
}

void Traps::Create(int type, const Coord &c)
{
	traplist.push_back(Trap(type, c));
}

bool Traps::Debug_List()
{
	clear_screen();

	if (traplist.empty())
	{
		print_text("No traps on this level.");
		wait_key();
		return false;
	}

	for (trap_iter i=traplist.begin(); i!=traplist.end(); ++i)
	{
		(*i).Show_Data();
	}

	return true;
}

bool Traps::Remove(const Coord &c)
{
	bool rv=false;

	for (trap_iter i=traplist.begin(); i!=traplist.end(); ++i)
	{
		if((*i).Get_Location()==c)
		{
			traplist.erase(i);
			rv=true; //return true if trap was removed at this location
			break;
		}
	}

	return rv;
}

void Traps::Refresh()
{
	for (trap_iter i=traplist.begin(); i!=traplist.end(); ++i)
	{
		Trap &t=(*i);
		gameview.Put_Trap(t.Get_Location(), t.Get_Type());
	}
}

void Traps::Save(Tar_Ball &tb)
{
	const int amt=(int)traplist.size();
	tb.Put(amt);
	for (trap_iter i=traplist.begin(); i!=traplist.end(); ++i)
	{
		(*i).Save(tb);
	}
}

void Traps::Load(Tar_Ball &tb)
{
	traplist.clear();

	const int amt=tb.Get_Next_Value();
	for (int t=0; t<amt; t++)
	{
		Trap ansa; //use dummy in construction
		ansa.Load(tb); //load saved values
		traplist.push_back(ansa);
	}
}
