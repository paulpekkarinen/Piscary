//Legend of Saladir - resist.cpp

//Refactored 27.6.2021 - 4.12.2022 Paul K. Pekkarinen

#include "resist.h"
#include "storage.h"

void resistpack::Clear()
{
	fire=0;
	poison=0;
	cold=0;
	magic=0;
	elec=0;
	water=0;
	acid=0;
}

void resistpack::Save(Tar_Ball &tb)
{
	tb.Put(fire);
	tb.Put(poison);
	tb.Put(cold);
	tb.Put(magic);
	tb.Put(elec);
	tb.Put(water);
	tb.Put(acid);
}

void resistpack::Load(Tar_Ball &tb)
{
	fire=tb.Get_Next_Value();
	poison=tb.Get_Next_Value();
	cold=tb.Get_Next_Value();
	magic=tb.Get_Next_Value();
	elec=tb.Get_Next_Value();
	water=tb.Get_Next_Value();
	acid=tb.Get_Next_Value();
}
