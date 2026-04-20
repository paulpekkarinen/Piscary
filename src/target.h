//target.h

//Unit target: Target type.

#ifndef TARGET_H
#define TARGET_H

#include "geometry.h"
#include "types.h"

/* this is used when magic is targetted by player */
struct Target
{
	invnode *invitem;      /* inventory item targetted */

	/* x and y coordinates of the target(s) */
	Coord pos;

	int direction;

	Target() : invitem(0), direction(0) { }

	void clear();
};

#endif
