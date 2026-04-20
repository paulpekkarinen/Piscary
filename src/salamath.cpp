/**************************************************************************
 * salamath.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 3.9.2021 - 7.4.2023 by Paul K. Pekkarinen

#include <cmath>
#include "geometry.h"
#include "salamath.h"

const real Math::pii=3.141592654;

// returns the distance between 2 points
int distance(int x1, int y1, int x2, int y2)
{
	return (int)sqrt( (x2-x1)*(x2-x1) + (y2-y1) * (y2-y1) );
}

int get_distance(const Coord &src, const Coord &dest)
{
	return (int)sqrt( (dest.x-src.x)*(dest.x-src.x) + (dest.y-src.y) * (dest.y-src.y) );
}
