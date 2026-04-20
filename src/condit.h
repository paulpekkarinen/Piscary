//Legend of Saladir - condit.h

//Unit condit: Conditions for creatures.

#ifndef CONDIT_H
#define CONDIT_H

#include "types.h"

/* conditions */
#define CONDIT_BLOATED    0
#define CONDIT_SATIATED   1
#define CONDIT_HUNGRY     2
#define CONDIT_STARVING   3
#define CONDIT_FAINTING   4
#define CONDIT_FAINTED    5
#define CONDIT_BURDENED   6
#define CONDIT_STRAINED   7
#define CONDIT_OVERLOADED 8
#define CONDIT_CONFUSED   9
#define CONDIT_BLESSED    10
#define CONDIT_CURSED     11
#define CONDIT_STUN       12
#define CONDIT_LUCKY      13
#define CONDIT_BLEEDING   14
#define CONDIT_POISONED   15
#define CONDIT_PARALYSED  16
#define CONDIT_BADLEGS    17
#define CONDIT_BADLARM    18
#define CONDIT_BADRARM    19
#define CONDIT_MAX        20 

/* condition groups */
#define CONDGRP_NOTSET    0
#define CONDGRP_FOOD      1
#define CONDGRP_PW        2

/* condition flags */
#define CONDF_NOVIS    0x0001 /* don't show on status line */

/* condition data struct */
struct Condition
{
	int type;
	int val;
	int time;

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
