//randgen.cpp - basic random generation routines

//Refactored 26.9.2021 - 21.9.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "log.h"
#include "randgen.h"
#include "spot.h"
#include "terrain.h"

void carvepassage(level_type *level, int sx, int sy, int tx, int ty)
{
	if (sx==tx && sy==ty) return;

	int prop_lr=0, prop_ud=0;
	int dir;

	int x_now=sx;	/* initialize starting coords */
	int y_now=sy;

	int rnum=RANDU(100);
	while (1)
	{
		if (x_now==tx)
			prop_lr=50; 		/* equal probability left and right */
		else if (x_now>tx) prop_lr=95;	/* we need to move left more */
		else prop_lr=5;		/* we need to move right more */

		if (y_now==ty)
			prop_ud=50;			/* up and down equal */
		else if (y_now>ty) prop_ud=95;	/* more downwards */
		else prop_ud=5;		/* more upwards */

		if (RANDU(100)<2)
			rnum=RANDU(100);

		if (RANDU(100)>30)
		{
			if (rnum >= prop_lr) dir=1; // right
			else dir=2;						// left
		}
		else
		{
			if (rnum < prop_ud) dir=4;	// down
			else dir=3;						// up
		}
		/*
		  if(x_now <= 1 || x_now >= level->sizex-1) {
		  dir=4;
		  if(prop_ud>50) dir=3;
		  }
		  if(y_now <= 1 || y_now >= level->sizey-1) {
		  dir=1;
		  if(prop_lr>50) dir=2;
		  }
		*/
		switch (dir)
		{
			case 1:	x_now++;
				break;
			case 2:	x_now--;
				break;
			case 3:	y_now++;
				break;
			case 4:	y_now--;
				break;
			default: break;

		}
		if (x_now <= 1 || x_now >= level->sizex-1) break;
		if (y_now <= 1 || y_now >= level->sizey-1) break;

		Coord c(x_now, y_now);

		level->Set_Terrain(c, TYPE_PATH);
		level->Set_Seen(c);

		if (x_now > tx+ROOM_MINX/2 && x_now < tx-ROOM_MINX/2 &&
			y_now > ty+ROOM_MINY/2 && y_now < ty-ROOM_MINY/2) break;
	}
}

void carvepassage2(level_type *level, int sx, int sy, int tx, int ty, bool stopopen, int dir)
{
	/* this shouldn't be possible */
	if (sx==tx && sy==ty) return;

	int y_now=sy;
	int x_now=sx;
	while (1)
	{
		if (x_now>=level->sizex || y_now>=level->sizey)
		{
			diary.Write("OHHOH! Carving over level boundaries at %d, %d.",
				x_now, y_now);
		}

		/* stop carving if digging on open passage */
		if (level->Is_Passable(x_now, y_now) && stopopen)
			return;

		level->Set_Terrain(x_now, y_now, TYPE_PASSAGE);

		if (x_now==tx && y_now==ty) break;

		if (dir==0 || dir==2)
		{
			if (level->Is_Passable(x_now-1, y_now))
				return;
			if (level->Is_Passable(x_now+1, y_now))
				return;
		}
		if (dir==1 || dir==3)
		{
			if (level->Is_Passable(x_now, y_now-1))
				return;
			if (level->Is_Passable(x_now, y_now+1))
				return;
		}

		if (x_now<tx)
		{
			x_now++;
			dir=1;
		}
		else if (x_now>tx)
		{
			x_now--;
			dir=3;
		}
		else if (y_now<ty)
		{
			y_now++;
			dir=2;
		}
		else if (y_now>ty)
		{
			y_now--;
			dir=0;
		}
	}
}

/* returns false if ROOMFLOOR or WALL inside room candidant */
bool checkregion_inv(level_type *level, int x1, int y1, int rsizex, int rsizey)
{
	if (x1==0 || y1==0)
		return false;

	for (int j=0; j<rsizey; j++)
	{
		for (int i=0; i<rsizex; i++)
		{
			Coord c(i+x1, j+y1);

			/* check boundaries */
			if (level->Is_Outside(c))
				return false;

			const int tt=level->Get_Terrain(c);

			if (tt==TYPE_WALLIP || tt==TYPE_ROOMFLOOR)
				return false;
		}
	}

	return true;
}

//check only regular walls
bool checkregion(level_type *level, int x1, int y1, int rsizex, int rsizey)
{
	if (x1==0 || y1==0)
		return false;

	for (int j=0; j<rsizey; j++)
	{
		for (int i=0; i<rsizex; i++)
		{
			Coord c(x1+i, y1+j);

			if (level->Is_Wall(c)==false)
				return false;
		}
	}

	return true;
}

void create_random_terrain(level_type *level, int type, int count)
{
	int num=count;

	while (num>0)
	{
		num--;

		Coord c=find_random_location(level, 1);
		if (c.x==-1) break;

		level->Set_Terrain(c, type);
	}
}

//Create terrain only on a passable tile, also exclude the same tile.
void create_terrain_on_passable(level_type *level, int type)
{
	Coord c;
	int maxtries = 1000;

	while(maxtries--)
	{
		c.x=1+RANDU(level->sizex-2);
		c.y=1+RANDU(level->sizey-2);

		if(level->Is_Passable(c)==false || level->Get_Terrain(c) == type)
			continue;

		level->Set_Terrain(c, type);
		break;
	}
}

/* This function builds a maze by recursively calling itself
** there is only one solution for this maze
*/
void recurse_maze(level_type *maze, int x, int y)
{
	int p = 0, d[4]={0};

	maze->Plot_Maze(x, y);

	if ((x > 1) && maze->Is_Wall(x-2, y))
		d[p++] = 1;
	if ((y > 1) && maze->Is_Wall(x, y-2))
		d[p++] = 2;
	if ((x < (maze->sizex - 2)) && maze->Is_Wall(x+2, y))
		d[p++] = 3;
	if ((y < (maze->sizey - 2)) && maze->Is_Wall(x, y+2))
		d[p++] = 4;
	if (!p) return;

	p = d[rand() % p];

	switch (p)
	{
		case 1:
			maze->Plot_Maze(x-1, y);
			recurse_maze(maze, x - 2, y);
			break;
		case 2:
			maze->Plot_Maze(x, y-1);
			recurse_maze(maze, x, y - 2);
			break;
		case 3:
			maze->Plot_Maze(x+1, y);
			recurse_maze(maze, x + 2, y);
			break;
		case 4:
			maze->Plot_Maze(x, y+1);
			recurse_maze(maze, x, y + 2);
			break;
		default: break;
	}
	recurse_maze(maze, x, y);
}
