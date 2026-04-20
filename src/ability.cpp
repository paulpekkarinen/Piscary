//Legend of Saladir - ability.cpp

//Written in 16.9.2022 - 29.7.2024 by Paul K. Pekkarinen

#include "ability.h"
#include "stats.h"
#include "storage.h"

//return matching value for statpack index
int Ability::Get_Statpack_Value(int slot)
{
	int rv;

	switch (slot)
	{
		case STAT_STR: rv=STR; break;
		case STAT_TGH: rv=TGH; break;
		case STAT_CON: rv=CON; break;
		case STAT_CHA: rv=CHA; break;
		case STAT_DEX: rv=DEX; break;
		case STAT_WIS: rv=WIS; break;
		case STAT_INT: rv=INT; break;
		case STAT_LUC: rv=LUC; break;
		case STAT_SPD: rv=SPD; break;
		default: rv=0;
	}

	return rv;
}

void Ability::Clear()
{
	STR=WIS=DEX=CON=CHA=INT=TGH=LUC=SPD=0;
}

void Ability::Save(Tar_Ball &tb)
{
	tb.Put(STR);
	tb.Put(WIS);
	tb.Put(DEX);
	tb.Put(CON);
	tb.Put(CHA);

	tb.Put(INT);
	tb.Put(TGH);
	tb.Put(LUC);
	tb.Put(SPD);
}

void Ability::Load(Tar_Ball &tb)
{
	STR=tb.Get_Next_Value();
	WIS=tb.Get_Next_Value();
	DEX=tb.Get_Next_Value();
	CON=tb.Get_Next_Value();
	CHA=tb.Get_Next_Value();

	INT=tb.Get_Next_Value();
	TGH=tb.Get_Next_Value();
	LUC=tb.Get_Next_Value();
	SPD=tb.Get_Next_Value();
}
