/**************************************************************************
 * gearslot.cpp --                                                        *
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

//Refactored 8.6.2023 Paul K. Pekkarinen

#include "gearslot.h"
#include "log.h"
#include "purse.h"
#include "storage.h"

void Equipslot::Clear()
{
	item=0;
	in_use=false;
	reserv=0;
	status=0;
}

void Equipslot::Save(Tar_Ball &tb)
{
	if (item==0)
		tb.Put(0); //zero indicates no item was saved, the slot was empty
	else
		tb.Put(1); //1 = item saved

	tb.Put_Bool(in_use);
	tb.Put(reserv);
	tb.Put_Char(status);
}

void Equipslot::Load(Tar_Ball &tb, inventory &inv, int slot)
{
	const int i=tb.Get_Next_Value();

	if (i==1)
	{
		//if item was saved, search the pointer from inventory items
		item=inv.Find_By_Slot_Id(slot);
		if (item==0)
			diary.Write("Item not found in Equipslot::Load().");
	}
	else
		item=0; //set to empty slot, no item data was saved

	in_use=tb.Get_Next_Bool();
	reserv=tb.Get_Next_Value();
	status=tb.Get_Next_Char();
}
