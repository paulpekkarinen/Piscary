//Legend of Saladir - traps.h

//Unit traps: Trap object and list of traps.

#ifndef TRAPS_H
#define TRAPS_H

#include <vector>
#include "geometry.h"
#include "types.h"

#define TRAP_NOTRAP		0
#define TRAP_DISARMED	1
#define TRAP_BOULDER	2
#define TRAP_BOMB		3
#define TRAP_ESHOCK		4
#define TRAP_MAXNUM		5

#define TRAPF_DOORTRAP 0x0001
#define TRAPF_CAVETRAP 0x0002
#define TRAPF_NOREMOVE 0x0004 /* do not remove the trap when goes on */

//Static data for traps types.
struct Trap_Data
{
	const char *name;
	char out; //ascii character
	int color;
	int16u flags;
};

//Trap is a harmful and invisible gadget on ground or in an object.
class Trap
{
private:
	int type; //trap type, TRAP_... values
	int damage; //amount of damage the trap has
	Coord pos; //where the trap is in the level

public:
	Trap(); //creates a dummy trap
	Trap(int tt, const Coord &c);

	Coord &Get_Location();
	int Get_Damage() const { return damage; }
	int Get_Type() const { return type; }

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

//Container class for trap objects.
class Traps
{
private:
	std::vector<Trap> traplist;
	typedef std::vector<Trap>::iterator trap_iter;

public:
	Trap &Get(const Coord &c);
	void Create(int type, const Coord &c, int16u trapflag);
	bool Remove(const Coord &c);

	void Refresh();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern Trap_Data list_traps[];

#endif

