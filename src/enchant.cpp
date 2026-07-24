//Legend of Saladir - enchant.cpp

//Refactored 4.12.2022 - 29.7.2024 Paul K. Pekkarinen

#include "enchant.h"
#include "storage.h"

void enchantments::Clear()
{
	pos_eff=0;
	neg_eff=0;

	res.Clear();
	stats.Clear();

	dmat=0;
	dice_sides=0;
	dice_times=0;
	damage_mod=0;
}

void enchantments::Save(Tar_Ball &tb)
{
	tb.Put(pos_eff);
	tb.Put(neg_eff);

	res.Save(tb);
	stats.Save(tb);

	tb.Put(dmat);
	tb.Put(dice_sides);
	tb.Put(dice_times);
	tb.Put(damage_mod);
}

void enchantments::Load(Tar_Ball &tb)
{
	pos_eff=tb.Get_Next_Unsigned();
	neg_eff=tb.Get_Next_Unsigned();

	res.Load(tb);
	stats.Load(tb);

	dmat=tb.Get_Next_Value();
	dice_sides=tb.Get_Next_Value();
	dice_times=tb.Get_Next_Value();
	damage_mod=tb.Get_Next_Value();
}
