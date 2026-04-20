//featlev.h

//Unit featlev: Level features generation.

#ifndef FEATLEV_H
#define FEATLEV_H

#include "genlevel.h"

//Feature level creates large level features and level types.
class Feature_Level : public Genlevel
{
private:
	void Generate_Easymaze();
	void Generate_Town(); //a town level
	void Generate_Wildhunt();
	void Roomylevel();
	void Roomylevel2(); //a level with one big room

public:
	Feature_Level(int width, int height, int bt, int danger);

	void Create(int dtype); //create a level of dtype
};

#endif
