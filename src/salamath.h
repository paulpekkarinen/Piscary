//Legend of Saladir - salamath.h

//Unit salamath: Misc. math functions and macros.

#ifndef SALAMATH_H
#define SALAMATH_H

#include "types.h"

/* utility macros */
#define ABS(u)    ( (u) >= 0 ? (u) : -(u))
#define SIGN(u)   ((u)==0 ? 0 : _sign_(u))
#define _sign_(u) ( (u) > 0 ? 1 : -1 )

int distance(int x1, int y1, int x2, int y2);
int get_distance(const Coord &src, const Coord &dest);

struct Math
{
	static const real pii;
};

#endif
