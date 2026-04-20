/**************************************************************************
 * condit.cpp --                                                          *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 2021 - 8.6.2023 by Paul K. Pekkarinen

#include "condit.h"
#include "storage.h"

void Condition::Save(Tar_Ball &tb)
{
	tb.Put(type);
	tb.Put(val);
	tb.Put(time);
}

void Condition::Load(Tar_Ball &tb)
{
	type=tb.Get_Next_Value();
	val=tb.Get_Next_Value();
	time=tb.Get_Next_Value();
}
