//stats.h

//Unit stats: Stats data and routines.

#ifndef STATS_H
#define STATS_H

#include "types.h"

/* index of status array */
#define STAT_ARRAYSIZE  9   /* the size of whole stat array */
#define STAT_BASICARRAY 7  /* number of basic stats */
#define STAT_STR	0
#define STAT_TGH	1
#define STAT_CON	2
#define STAT_CHA	3
#define STAT_DEX	4
#define STAT_WIS	5
#define STAT_INT	6
#define STAT_LUC	7
#define STAT_SPD	8

#define STATMAX_SPEED	180
#define STATMAX_LUCK	20
#define STATMAX_GEN		99
#define STATMIN_GEN		0

struct statpack
{
	int initial;	/* initial status value */
	int temp;	/* temporary change */
	int perm;      /* permanent status change */
	int min;	/* minimum value */
	int max;       /* maximum value */

	int Get();

	void Change_Initial(int amount, bool player); /* CHANGE initial stat */
	void Change_Perm(int amount, bool player);
	void Change_Temp(int amount, bool player);
	void Reset(int i);
	void Set_Initial(int value, int imax, int imin);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern const char *txt_statnames[];
extern const char *txt_statnames_short[];
extern const char *txt_statrating[];

#endif
