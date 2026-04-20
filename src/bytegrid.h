//Legend of Saladir - bytegrid.h

//Unit bytegrid: 2D char array.

#ifndef BYTEGRID_H
#define BYTEGRID_H

#include "types.h"

//2D char array with limits check.
class Bytegrid
{
private:
	int width;
	int height;
	int8u *grid;

	bool Is_Outside(int x, int y);

public:
	static const int8u Outside_Value=255;

	Bytegrid(int w, int h);
	~Bytegrid();

	int8u Get(int x, int y);

	void Clear(int8u v);
	void Resize(int w, int h);
	void Set(int x, int y, int8u v);
};

#endif
