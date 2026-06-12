//target.h

//Unit target: Target type.

#ifndef TARGET_H
#define TARGET_H

#include "geometry.h"

class Actor;
struct invnode;

/* this is used when something is targetted by player */
struct Target
{
	Actor *olento; //monster targetted
	invnode *invitem; //item targetted

	/* x and y coordinates of the target(s) */
	Coord pos;

	Target() : olento(0), invitem(0) { }

	bool Is_Active();

	void Clear();
	void Set(const Coord &c);
	void Set(Actor *o);
	void Set(invnode *i);
	void Set(invnode *i, const Coord &c);
};

#endif
