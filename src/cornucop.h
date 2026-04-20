//Legend of Saladir - cornucop.h

//Unit cornucop: Creates items for a creature.

#ifndef CORNUCOP_H
#define CORNUCOP_H

#include "itempack.h"
#include "types.h"

class skillset;

//Creates and determines what items creatures have.
class Cornucopia
{
private:
	static const Itempack starting_items[];

	inventory &inv;

	void Armour(int ar);
	void Chain_Armour(int ar);
	void One_Handed_Weapon(int st, int amt, int mat);

public:
	explicit Cornucopia(inventory &i)
		: inv(i) { }

	bool Giveskillweapon(skillset &skills);

	void Hunter_Items();
	void Initpack(skillset &skills);
	void Knight_Items();
	void Merchant_Items();
	void Ninja_Items();
	void Rogue_Items();
};

#endif
