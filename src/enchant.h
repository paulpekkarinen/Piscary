//enchant.h

//Unit enchant: Enchantments for items.

#ifndef ENCHANT_H
#define ENCHANT_H

#include "ability.h"
#include "resist.h"

/* pack for enchantments */
struct enchantments
{
	/* misc enchantments, bit flags on/off */
	int32u pos_eff; /* positive effects */
	int32u neg_eff; /* negative effects */

	/* material/magic resistances (0=nothing)*/
	resistpack res;

	/* stats enchantments */
	Ability stats;

	/* special damage */
	int dmat;  /* fire, cold etc. */
	int dice_sides;
	int dice_times;
	int damage_mod;

	enchantments& operator=(const enchantments& right)
	{
		if(this == &right) return *this;

		pos_eff=right.pos_eff;
		neg_eff=right.neg_eff;

		res=right.res;
		stats=right.stats;

		dmat=right.dmat;
		dice_sides=right.dice_sides;
		dice_times=right.dice_times;
		damage_mod=right.damage_mod;

		return *this;
	}

	void Clear();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
