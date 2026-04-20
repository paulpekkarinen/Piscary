//Legend of Saladir - geometry.cpp

//Refactored 30.11.2022 - 1.12.2022 Paul K. Pekkarinen

#include <cmath>
#include "geometry.h"
#include "move.h"
#include "storage.h"

void Coord::Reset()
{
	x=0;
	y=0;
}

void Coord::Set(int sx, int sy)
{
	x=sx;
	y=sy;
}

void Coord::Move_Direction(int dir)
{
	x+=move_dx[dir];
	y+=move_dy[dir];
}

void Coord::Save(Tar_Ball &tb)
{
	tb.Put(x);
	tb.Put(y);
}

void Coord::Load(Tar_Ball &tb)
{
	x=tb.Get_Next_Value();
	y=tb.Get_Next_Value();
}

Coordist::Coordist(Coordist *src)
	: Coord(src->x, src->y), dist(src->dist)
{

}

/*
 * This function is used when we calculate an estimate of the remaining
 * path.
 */
int Coordist::Distance(const Coordist &dest)
{
   int dx=x - dest.x;
   int dy=y - dest.y;

   return (int)(sqrt( (float)(dx*dx + dy*dy)) * 1.5f);
}

void Coordist::Save(Tar_Ball &tb)
{
	tb.Put(x);
	tb.Put(y);
	tb.Put(dist);
}

void Coordist::Load(Tar_Ball &tb)
{
	x=tb.Get_Next_Value();
	y=tb.Get_Next_Value();
	dist=tb.Get_Next_Value();
}

void Plane::Set(int sw, int sh)
{
	width=sw;
	height=sh;
}
