//Legend of Saladir - gearslot.h

//Unit gearslot: Equipment slot.

#ifndef GEARSLOT_H
#define GEARSLOT_H

#include "types.h"

#define EQSTAT_NOLIMB   11
#define EQSTAT_BROKEN   10
#define EQSTAT_OK       0

//One equipment slot.
struct Equipslot
{
	invnode *item;
	bool in_use;
	int reserv;
	int8u status;

	bool Is_Empty();
	bool Is_Reserved();
	bool Is_Usable();

	void Clear();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, inventory &inv, int slot);
};

#endif
