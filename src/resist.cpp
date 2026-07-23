//Legend of Saladir - resist.cpp

//Refactored 27.6.2021 - 4.12.2022 Paul K. Pekkarinen

#include "damage.h"
#include "output.h"
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

int resistpack::Get_Damage_Protection(int dam, int elem)
{
	int rv;

	switch (elem)
	{
		case ELEMENT_FIRE: rv=fire; break;
		case ELEMENT_POISON: rv=poison; break;
		case ELEMENT_COLD: rv=cold; break;
		case ELEMENT_ELEC: rv=elec; break;
		case ELEMENT_WATER: rv=water; break;
		case ELEMENT_ACID: rv=acid; break;
		default: rv=-1; break;
	}

	if (rv==-1)
		return 0;

	return (int)((real)dam/100.0) * rv;
}

void resistpack::Display_Status()
{
	my_printf(" Fire%03d Posn%03d Cold%03d Elec%03d Watr%03d Acid%03d\n%16c",
		fire, poison, cold, elec, water, acid, ' ');
}

void resistpack::Modify(resistpack &r)
{
	fire+=r.fire;
	poison+=r.poison;
	cold+=r.cold;
	magic+=r.magic;
	elec+=r.elec;
	water+=r.water;
	acid+=r.acid;
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
