//Legend of Saladir - bytegrid.cpp

//Written in 29.3.2023 - 21.6.2025 by Paul K. Pekkarinen

#include "bytegrid.h"

Bytegrid::Bytegrid(int w, int h)
	: width(w), height(h)
{
	grid=new int8u[w*h];
}

Bytegrid::~Bytegrid()
{
	delete[] grid;
}

int8u Bytegrid::Get(int x, int y)
{
	if (Is_Outside(x, y))
		return Outside_Value;
	return grid[y*width+x];
}

bool Bytegrid::Is_Outside(int x, int y)
{
	if (x<0 || y<0 || x>=width || y>=height) return true;
	return false;
}

void Bytegrid::Clear(int8u v)
{
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			Set(x, y, v);
		}
	}
}

void Bytegrid::Resize(int w, int h)
{
	delete[] grid;
	width=w;
	height=h;
	grid=new int8u[w*h];
}

void Bytegrid::Set(int x, int y, int8u v)
{
	if (Is_Outside(x, y))
		return;
	grid[y*width+x]=v;
}
