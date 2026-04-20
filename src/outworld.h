//Legend of Saladir - outworld.h

//Unit outworld: Outworld terrain generation routines.

#ifndef OUTWORLD_H
#define OUTWORLD_H

#include "types.h"

class World;

class Outworld
{
private:
	const int DSIZE=256;
	const int DARRAYSIZE=DSIZE+1;
	static const char weight_stencil[];

	level_type *level;
	int dsize;
	float *dterrain;

	float Get(int x, int y); //get value from 'dterrain' height map
	bool Is_Outside(int x, int y); //check height map boundary

	bool Advnext_Point(int mode, int *x, int *y);
	float Avg_Diamond(int delta, int x, int y);
	float Avg_Square(int delta, int x, int y);
	void Clear_Table(); //clears height map with 0.0 value
	void Diamond_Square(float h);
	bool Dolakes(int percentage);
	void Dsq_2_Map(); //transform height map to tile map
	void Set(int x, int y, float v); //set height map value

public:
	explicit Outworld(level_type *luola);
	~Outworld();

	void Create(World *myworld);
	void Create_Passage(const Coord &src, const Coord &dest, float v);
};

#endif
