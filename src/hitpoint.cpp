//Legend of Saladir - hitpoint.cpp

//Refactored 2021 - 27.2.2023 by Paul K. Pekkarinen

#include "creature.h"
#include "hitpoint.h"
#include "invnode.h"
#include "storage.h"

void hpslot::Additembonus(invnode *item)
{
	if (item == 0)
		return;

	/* armor class (damage protection) */
	ac += item->i.ac;

	/*
	 * defence value, substracted directly from attack
	 * skill on melee attacks
	 */
	dv += item->i.dv;

	if (item->i.status & ITEM_ENCHANTED)
	{
		res.Modify(item->i.ench.res);
	}
}

void hpslot::Clear()
{
	cur=0;
	ini=0;
	max=0;
	ac=0;
	dv=0;
	res.Clear();
}

void hpslot::Clear_Bonuses()
{
	ac=0;
	res.Clear();
}

void hpslot::Set_Points(int v)
{
	ini=v;
	cur=v;
	max=v;
}

void hpslot::Set_Race_Values(int race)
{
	/* set race ac */
	ac=npc_races[race].ac;

	/* set race resistances */
	res=npc_races[race].res;
}

void hpslot::Save(Tar_Ball &tb)
{
	tb.Put(cur);
	tb.Put(ini);
	tb.Put(max);
	tb.Put(ac);
	tb.Put(dv);
	res.Save(tb);
}

void hpslot::Load(Tar_Ball &tb)
{
	cur=tb.Get_Next_Value();
	ini=tb.Get_Next_Value();
	max=tb.Get_Next_Value();
	ac=tb.Get_Next_Value();
	dv=tb.Get_Next_Value();
	res.Load(tb);
}
