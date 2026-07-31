//Legend of Saladir - hitpoint.h

//Unit hitpoint: Hitpoint datatype.

#ifndef HITPOINT_H
#define HITPOINT_H

#include "resist.h"

/* hitpoint modifiers */
#define HP_HEADMOD	0.50
#define HP_HANDMOD	0.75
#define HP_LEGSMOD	0.75

#define HPSLOT_MAX       5	/* num of hpstat slots */
#define HPSLOT_HEAD	 0
#define HPSLOT_LEFTHAND	 1
#define HPSLOT_RIGHTHAND 2
#define HPSLOT_BODY	 3
#define HPSLOT_LEGS      4

/* hitpoints pack */
struct hpslot
{
	int cur;     /* current value */
	int ini;     /* initial value */
	int max;     /* maximum value */
	int ac;     /* armor class for bodypart */
	int dv;     /* defence value */
	resistpack res; /* element resistances */

	void Additembonus(invnode *item);
	void Clear();
	void Clear_Bonuses();
	void Set_Points(int v);
	void Set_Race_Values(int race);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
