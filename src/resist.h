//Legend of Saladir - resist.h

//Unit resist: Resistances to elements.

#ifndef RESIST_H
#define RESIST_H

#include "types.h"

struct resistpack
{
	int fire;
	int poison;
	int cold;
	int magic;
	int elec;
	
	int water;
	int acid;

	resistpack()
		: fire(0), poison(0), cold(0), magic(0), elec(0), water(0), acid(0)
	{
	}

	resistpack(int f, int p, int c, int m, int e, int w, int a)
		: fire(f), poison(p), cold(c), magic(m), elec(e), water(w), acid(a)
	{
	}

	resistpack& operator=(const resistpack& right)
	{
		if(this == &right) return *this;

		fire=right.fire;
		poison=right.poison;
		cold=right.cold;
		magic=right.magic;
		elec=right.elec;

		water=right.water;
		acid=right.acid;

		return *this;
	}

	void Clear();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);	
};

#endif
