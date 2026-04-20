// itemdata.h

//Unit itemdata: Misc. item data.

#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <string>

/* food definition struct */
struct food_data
{
	const char *name;
	int nutr;
};

//item category data
struct item_category
{
	char out;
	const char *name;
};

struct special_item_outface
{
	char out;
	int color;
};

struct torch_data
{
	int sx;
	int sy;
	int dx, dy;	/* delta x, delta y from player position */
	const char *data;
};

struct valuable_data
{
	std::string name;	// name when not identified
	int type;			// item type, weapon, armor, tool etc..
	int material;		// item material
	int weight;
	int value;			// coin value in copper unit (1/10 of gold coin)
};

extern item_category gategories[];
extern special_item_outface item_outfaces[];
extern valuable_data valuables[];

#endif
