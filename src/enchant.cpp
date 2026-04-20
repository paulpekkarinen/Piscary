//Legend of Saladir - enchant.cpp

//Refactored 4.12.2022 - 29.7.2024 Paul K. Pekkarinen

#include "enchant.h"
#include "storage.h"

void enchantments::Clear()
{
	pos_eff=neg_eff=0;

	res.Clear();
	stats.Clear();

	dmat=ds=dt=dmod=0;
}

void enchantments::Save(Tar_Ball &tb)
{
	tb.Put(pos_eff);
	tb.Put(neg_eff);

	res.Save(tb);
	stats.Save(tb);

	tb.Put(dmat);
	tb.Put(ds);
	tb.Put(dt);
	tb.Put(dmod);
}

void enchantments::Load(Tar_Ball &tb)
{
	pos_eff=tb.Get_Next_Unsigned();
	neg_eff=tb.Get_Next_Unsigned();

	res.Load(tb);
	stats.Load(tb);

	dmat=tb.Get_Next_Value();
	ds=tb.Get_Next_Value();
	dt=tb.Get_Next_Value();
	dmod=tb.Get_Next_Value();
}
