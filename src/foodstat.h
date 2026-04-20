//foodstat.h

//Unit foodstat: Food status.

#ifndef FOODSTAT_H
#define FOODSTAT_H

/* food levels, max values of level */
#define FOOD_MAXNUTR    24000
#define FOOD_BLOATED    24000
#define FOOD_SATIATED   19200
#define FOOD_FULL       14400
#define FOOD_HUNGRY     9600
#define FOOD_STARVING   4800
#define FOOD_FAINTING   2400
#define FOOD_FAINTED	0

struct foodstat //note: not used
{
	const char *desc;
	int hour;
	int min;
};

#endif
