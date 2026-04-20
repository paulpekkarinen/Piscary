//tactics.h

//Unit tactics: Combat tactics.

#ifndef TACTICS_H
#define TACTICS_H

#include "types.h"

#define TACTIC_COWARD	0
#define TACTIC_VERYDEF	1
#define TACTIC_DEF		2
#define TACTIC_NORMAL	3
#define TACTIC_AGGR		4
#define TACTIC_VERYAGGR	5
#define TACTIC_BERZERK	6

struct tactics
{
	int dam; /* damage modifier */
	int spd;	/* speed modify */
	int hit; /* hit modify */
	int pv;	/* absorbed hp */
	int dv; /* defense value, */
};

extern tactics tacticeffects[];
extern const char *tacticnames[];

#endif
