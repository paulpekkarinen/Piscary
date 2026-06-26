//Legend of Saladir - amount.h

//Unit amount: Amount of things.

#ifndef AMOUNT_H
#define AMOUNT_H

struct Amount
{
	int num_armors;
	int num_artifacts;
	int num_dungeons;
	int num_lairs;
	int num_materials;
	int num_miscitems;
	int num_npcraces;
	int num_rooms;
	int num_scrolls;
	int num_shopkeepers;
	int num_specials;
	int num_valuables;
	int num_weapons;

	void Initialize();
	void Show_Data();
};

extern Amount mucho;

#endif
