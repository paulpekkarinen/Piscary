/**************************************************************************
 * stats.cpp --                                                           *
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

//Refactored 20.8.2021 - 27.2.2023 Paul K. Pekkarinen

#include "game.h"
#include "stats.h"
#include "storage.h"

const char *txt_statnames[]=
{
   "Strength",
   "Toughness",
   "Constitution",
   "Charisma",
   "Dexterity",
   "Wisdom",
   "Intelligence",
   "Luck",
   "Speed",
   0
};

const char *txt_statnames_short[]=
{
   "STR",
   "TGH",
   "CON",
   "CHA",
   "DEX",
   "WIS",
   "INT",
   "LUC",
   "SPD",
   0
};

const char *txt_statrating[]=
{
   "Very bad",
   "Bad",
   "Low",
   "Poor",
   "Fair",
   "Average",
   "Nice",
   "Good",
   "Very good",
   "Superhuman",
   "Immortal",
   0
};

int statpack::Get()
{
	int result=0;

	result=initial + temp + perm;

	if (result < min)
		result = min;
	else if (result > max)
		result = max;

	//	if(result >=100) result=99;

	return result;
}

void statpack::Reset(int i)
{
	initial=i;
	temp=0;
	perm=0;
	min=0;
	max=99;
}

void statpack::Save(Tar_Ball &tb)
{
	tb.Put(initial);
	tb.Put(temp);
	tb.Put(perm);
	tb.Put(min);
	tb.Put(max);
}

void statpack::Load(Tar_Ball &tb)
{
	initial=tb.Get_Next_Value();
	temp=tb.Get_Next_Value();
	perm=tb.Get_Next_Value();
	min=tb.Get_Next_Value();
	max=tb.Get_Next_Value();
}

void statpack::Change_Initial(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	initial+=amount;

	if (initial > max)
		initial = max;
	if (initial < min)
		initial = min;
}

void statpack::Change_Perm(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	perm+=amount;

	if (perm > STATMAX_GEN)
		perm = STATMAX_GEN;

	if (perm < -STATMAX_GEN)
		perm = -STATMAX_GEN;
}

void statpack::Change_Temp(int amount, bool player)
{
	if (player)
		GAME_NOTIFYFLAGS|=GAME_ATTRIBCHG;

	temp+=amount;

	if (temp > STATMAX_GEN)
		temp = STATMAX_GEN;

	if (temp < -STATMAX_GEN)
		temp = -STATMAX_GEN;
}

void statpack::Set_Initial(int value, int imax, int imin)
{
	max=imax;
	min=imin;
	temp=0;

	//	pstat->initial+=amount;
	initial=value;

	if (initial > max)
		initial = max;
	if (initial < min)
		initial = min;
}
