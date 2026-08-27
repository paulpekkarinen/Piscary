/**************************************************************************
 * outworld.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 10.07.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 19.07.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************
 * eventually intended to be a random outworld generator                  *
 **************************************************************************/

//Refactored 25.9.2021 - 21.9.2025 Paul K. Pekkarinen

#include <cmath>
#include "build.h"

#include "caves.h"
#include "dice.h"
#include "log.h"
#include "outworld.h"
#include "randgen.h"
#include "terrain.h"
#include "way.h"
#include "world.h"

/*
 * fractRand is a useful interface to randrealnum.
 */
#define fractRand(v) randrealnum (-v, v)

//weight stencil values range from 0 (middle) to 100 (no change to original)
//to level the path in the middle as walkable grass terrain and gradually
//smooth to the original terrain.
const char Outworld::weight_stencil[]=
{
	100, 100, 75, 100, 100,
	100, 50, 25, 50, 100,
	75, 25, 0, 25, 75,
	100, 50, 25, 50, 100,
	100, 100, 75, 100, 100
};

Outworld::Outworld(level_type *luola)
	: level(luola)
{
	dsize=(DARRAYSIZE+1) * (DARRAYSIZE+1);
	dterrain=new float[dsize];
}

Outworld::~Outworld()
{
	delete[] dterrain;
}

float Outworld::Get(int x, int y)
{
	if (Is_Outside(x, y))
		return 0.0f;

	return dterrain[(y * DARRAYSIZE) + x];
}

bool Outworld::Is_Outside(int x, int y)
{
	if (x<0 || y<0 || x>=DARRAYSIZE || y>=DARRAYSIZE)
		return true;

	return false;
}

void Outworld::Clear_Table()
{
	for (int i=0; i<dsize; i++)
		dterrain[i]=0.0f;
}

void Outworld::Create_Passage(const Coord &src, const Coord &dest, float v)
{
	Coord c=src;
	Coord d;
	int movecount=random_number(3, 7);
	bool vertical=true;

	for (int t=0; t<10000; t++)
	{
		//cover 5x5 area as defined in weight_stencil data
		int index=0;
		for (d.y=c.y-2; d.y<=c.y+2; d.y++)
		{
			for (d.x=c.x-2; d.x<=c.x+2; d.x++)
			{
				const int wp=weight_stencil[index];
				if (wp!=100)
				{
					const float percentage=(float)wp/100.0f;
					const float original=Get(d.x, d.y);
					Set(d.x, d.y, (v+original)*percentage);
				}

				index++;
			}
		}

		if (c==dest) break;

		if (vertical)
		{
			if (c.y>dest.y) c.y--;
			else if (c.y<dest.y) c.y++;
		}
		else
		{
			if (c.x>dest.x) c.x--;
			else if (c.x<dest.x) c.x++;
		}

		movecount--;
		if (movecount<0)
		{
			movecount=random_number(3, 7);
			if (vertical)
				vertical=false;
			else
				vertical=true;
		}
	}
}

void Outworld::Create(World *myworld)
{
	/* generate water, lakes */
	//Dolakes(10); //note: generating lakes was commented out

	//create the height map of the terrain
	Diamond_Square(0.47f);

	//find min and max values of the generated height map
	float minval=0.0f;
	float maxval=0.0f;

	for (int i=0; i<dsize; i++)
	{
		const float v=dterrain[i];
		if (minval>v) minval=v;
		if (maxval<v) maxval=v;
	}

	//use average value to create corridors between dungeon locations
	const float avg=(minval+maxval)/2.0f;

	//height map to terrain map conversion
	Dsq_2_Map();
}

void Outworld::Dsq_2_Map()
{
	float val, max=0, min=999.0, avg=0;
	int x, y, count=0;

	for(y=0; y<DARRAYSIZE; y++)
	{
		for(x=0; x<DARRAYSIZE; x++)
		{
			val = Get(x, y);

			if(val>max)
				max=val;
			if(val<min)
				min=val;
			avg+=val;
			count++;
		}
	}
	avg = avg / count;

	if(min < 0)
	{
		min = -1 * min;

		for(y=0; y<DARRAYSIZE; y++)
		{
			for(x=0; x<DARRAYSIZE; x++)
			{
				dterrain[y * DARRAYSIZE + x]+=min;
			}
		}
		max+=min;
		avg+=min;
		min=0;
	}

	/*
	  level_water = 0.55f * level_ground;
	  level_ground = 0.70f * avg;
	  level_hill = 1.40f * level_ground;
	  level_mount = 1.80f * level_ground;
	*/
	float level_water = 0.20f * max;
	float level_ground = 0.70f * avg ;
	float level_hill = 1.30f * avg;
	float level_mount = 0.80f * max;

#ifdef saladir_debug
	diary.Write("Max = %3.4f\nMin = %3.4f\nAvg = %3.4f\n",
		max, min, avg);

	diary.Write("   Water level: %4.2f - %4.2f\n", min, level_ground);
	diary.Write("  Ground level: %4.2f - %4.2f\n", level_ground, level_hill);
	diary.Write("    Hill level: %4.2f - %4.2f\n", level_hill, level_mount);
	diary.Write("Mountain level: %4.2f - %4.2f\n", level_mount, max);

	diary.Write("Creating the map... (%d * %d)\n", level->sizex, level->sizey);
#endif

	for(y=0; y<DARRAYSIZE; y++)
	{
		if(y>(level->sizey-1)) break;
		for(x=0; x<DARRAYSIZE; x++)
		{

			if(x>(level->sizex-1)) break;
			val = Get(x, y);

			int tt=TYPE_CORNFIELD;

			if(val < level_water)
				tt=TYPE_SEA;
			else if(val < level_ground)
				tt=TYPE_WATER;
			else if(val < level_hill)
				tt=TYPE_GRASS;
			else if(val < level_mount)
				tt=TYPE_HILLS;
			else
				tt=TYPE_MOUNTAIN;

			level->Set_Terrain(x, y, tt);
		}
	}
}

void Outworld::Set(int x, int y, float v)
{
	if (Is_Outside(x, y)) return;

	dterrain[y * DARRAYSIZE + x] = v;
}

float Outworld::Avg_Square(int delta, int x, int y)
{
	float avg = (
		Get(x-delta, y-delta) +
		Get(x+delta, y-delta) +
		Get(x+delta, y+delta) +
		Get(x-delta, y+delta)) * 0.25f;

	return avg;
}

float Outworld::Avg_Diamond(int delta, int x, int y)
{
	float avg;

	if(!x && y>0)
	{
		avg = (
			Get(x, y-delta) +
			Get(x, y+delta) +
			Get(x+delta, y) +
			Get(x+DSIZE-delta, y)) * 0.25f;
	}
	else if(x==DSIZE && y>0)
	{
		avg = (
			Get(x, y-delta) +
			Get(x, y+delta) +
			Get(delta, y) +
			Get(x-delta, y)) * 0.25f;
	}
	else if(x>0 && !y)
	{
		avg = (
			Get(x, y+DSIZE-delta) +
			Get(x, y+delta) +
			Get(x-delta, y) +
			Get(x+delta, y)) * 0.25f;
	}
	else if(x>0 && y==DSIZE)
	{
		avg = (
			Get(x, y-delta) +
			Get(x, delta) +
			Get(x-delta, y) +
			Get(x+delta, y)) * 0.25f;
	}
	else
	{
		avg = (
			Get(x, y-delta) +
			Get(x, y+delta) +
			Get(x+delta, y) +
			Get(x-delta, y)) * 0.25f;
	}

	return avg;
}

void Outworld::Diamond_Square(float h)
{
	int x, y;
	float heightScale = 100;
	float ratio = (float)pow (2.0, -h);
	float scale = heightScale * ratio;
	int delta = DSIZE / 2;

	Clear_Table();

	while(delta)
	{
		// generate squares
		for(y=delta; y<DSIZE; y+=delta)
		{
			for(x=delta; x<DSIZE; x+=delta)
			{
				Set(x, y, scale * fractRand (.5f) + Avg_Square(delta, x, y));
				x+=delta;
			}
			y+=delta;
		}

		// generate diamonds
		for(y=delta; y<DSIZE; y+=delta)
		{
			for(x=delta; x<DSIZE; x+=delta)
			{
				float rr = scale * fractRand (.5f);

				Set(x, y-delta, rr + Avg_Diamond(delta, x, y-delta));
				Set(x, y+delta, rr + Avg_Diamond(delta, x, y+delta));
				Set(x-delta, y, rr + Avg_Diamond(delta, x-delta, y));
				Set(x+delta, y, rr + Avg_Diamond(delta, x+delta, y));
				x+=delta;
			}
			y+=delta;
		}
		delta >>= 1;
		scale *= ratio;
		//		delta = 0;
	}
}

bool Outworld::Advnext_Point(int mode, int *x, int *y)
{
	switch(mode)
	{
		case DIR_RIGHT:
		if(*x == level->sizex-1)
		{
			if(*y == level->sizey-1)
			{
				return true;
			}
			*x=0;
			(*y)++;
		}
		else
			(*x)++;

		break;
		case DIR_LEFT:
		break;
		default:
		break;
	}

	return false;
}

bool Outworld::Dolakes(int percentage)
{
	real amt = ((real)level->sizex * level->sizey) / 100 * percentage;
	int tiles_left = (int)amt;

	diary.Write("Generated %d tiles of water.", tiles_left);

	/* how may "lake seeds" to be generated */
	int seeds_left = throwdice(3, 5, 0);

	diary.Write("Generated %d water seeds.", seeds_left);

	if(tiles_left <= seeds_left)
		return false;

	tiles_left -= seeds_left;

	/* seed lake spots */
	while(seeds_left--)
	{
		create_terrain_on_passable(level, TYPE_WATER);
	}

	Coord c;
	bool prev_set=false;
	bool now_clear=false;

	while (tiles_left)
	{
		if(level->Get_Terrain(c) == TYPE_WATER)
			prev_set=true;
		else
			prev_set=false;

		if (Advnext_Point(DIR_RIGHT, &c.x, &c.y))
			c.Set_Location(0, 0);

		if(level->Get_Terrain(c) == TYPE_GRASS)
			now_clear=true;
		else
			now_clear=false;

		if(prev_set && now_clear)
		{
			if(RANDU(100) < 60)
			{
				level->Set_Terrain(c, TYPE_WATER);
				tiles_left--;
				prev_set=now_clear=false;
			}
		}
	}

	return true; //note: returns before next code

	/* now start iterating lakes from their seedpoints */
	while (1)
	{
		c.x=1+RANDU(level->sizex-2);
		c.y=1+RANDU(level->sizey-2);

		if(level->Get_Terrain(c) == TYPE_WATER)
		{
			for (int i=0; i<8; i++)
			{
				Coord d=c;
				d.Move_Direction(Way::Get_Direction(i));

				if(level->Get_Terrain(d) == TYPE_GRASS)
				{
					level->Set_Terrain(d, TYPE_WATER);
					tiles_left--;
					break;
				}
			}
		}

		if(!tiles_left)
			break;
	}

	return true;
}
