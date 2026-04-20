//material.h

//Unit material: Material type.

#ifndef MATERIAL_H
#define MATERIAL_H

#include "types.h"

#define MAT_LEATHER	 0 /* armor materials only */
#define MAT_CHAIN	 1 /* armor materials only */
#define MAT_WOOD	 2
#define MAT_GLASS	 3 /* not armor */
#define MAT_COPPER	 4
#define MAT_IRON	 5
#define MAT_STEEL	 6
#define MAT_STONE	 7
#define MAT_SILVER	 8
#define MAT_GOLD	 9
#define MAT_CRYSTAL	 10 /* not armor */
#define MAT_ELVEN	 11
#define MAT_DWARVEN	 12
#define MAT_ORCISH	 13
#define MAT_MITHRILL	14
#define MAT_PLATINIUM	15
#define MAT_EBONY		16 /* not armor */
#define MAT_DIAMOND		17
#define MAT_ADAMANTIUM	18
#define MAT_DRAGONWHITE	19	/* armor materials */
#define MAT_DRAGONRED	20
#define MAT_DRAGONGREEN	21
#define MAT_DRAGONBLUE	22
#define MAT_DRAGONBLACK	23

/* for cloths and some special items */
#define MAT_FABRIC       24
#define MAT_PAPYRUS      25
#define MAT_ORGANIC      26 //corpses, food etc.

/* special material status flags */
/* material flags */
#define MATSTAT_NOTARMOR    0x00000001	/* material is not for armor */
#define MATSTAT_NOTBOWS	    0x00000002	/* not for bow weapons */
#define MATSTAT_NOTWEAPON   0x00000004	/* material is not for weapons */
#define MATSTAT_NOTMISSILE  0x00000008	/* material is not for weapons */
#define MATSTAT_RING        0x00000010  /* for rings too */
#define MATSTAT_SPECIAL     0x40000000  /* for cloth/scrolls only */
#define MATSTAT_CANSHARD    0x80000000	/* can break immendiately */

struct matlist
{
	const char *name;		// description of the material
	int dam;		// damage/ac modifier
	int hit;     // change to hit mod
	int dv;		// defense value
	int speed;   // speed modifier
	int color;	// material color
	int32u status;	// special material flags
	int32u resist; // resistances
	int durability; // break propability (condition goes worse)
	int appearprob; // how often appear (0-100)
	real wmod;    // weight modifier, multiplier
	real vmod;    // item (gold) value modifier
};

extern matlist materials[];

#endif
