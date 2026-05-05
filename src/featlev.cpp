/**************************************************************************
 * featlev.cpp --                                                         *
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

//Refactored 1.3.2023 - 7.8.2024 Paul K. Pekkarinen

#include "avatar.h"
#include "dice.h"
#include "factory.h"
#include "featlev.h"
#include "outworld.h"
#include "randgen.h"
#include "roomgen.h"
#include "spawner.h"
#include "terrain.h"
#include "world.h"

Feature_Level::Feature_Level(int width, int height, int bt, int danger)
	: Genlevel(width, height, bt, danger)
{

}

void Feature_Level::Generate_Easymaze()
{
	recurse_maze(this, 1, 1);

	int sx=sizex;
	int sy=sizey;

	int holes=(sx*sy)/(30+RANDU(20));

	while (holes>0)
	{
		sx=1+RANDU(sizex-2);
		sy=1+RANDU(sizey-2);

		if (Is_Passable(sx, sy))
			continue;

		Set_Terrain(sx, sy, TYPE_PASSAGE);

		holes--;
	}

	holes=15+RANDU(20);

	while (holes)
	{
		sx=5+RANDU(10);
		sy=5+RANDU(5);

		int by=1+RANDU(sizey - sy - 2);
		int bx=1+RANDU(sizex - sx - 2);

		int cy=sy;
		while (cy>0)
		{
			int cx=sx;
			while (cx>0)
			{
				Set_Terrain(bx+cx, by+cy, TYPE_PASSAGE);
				cx--;
			}
			cy--;
		}
		holes--;
	}
}

/*
** This will be called when a new level is needed
*/
void Feature_Level::Create(int dtype)
{
	//create level topology based on its type
	switch (dtype)
	{
		case DTYPE_OUTWORLD:
		{
			//rediscover dungeon locations for the overworld
			Plane p(sizex, sizey);
			world->Determine_Dungeon_Locations(p);

			Outworld ow(this);
			ow.Create(world);

			//put entrance tiles to the level
			world->Create_Dungeon_Entrances(this);
		}
		break;
		case DTYPE_MAZE: recurse_maze(this, 1, 1); break;
		case DTYPE_MAZE2: Generate_Easymaze(); break;
		case DTYPE_ROOMY: Roomylevel(); break;
		case DTYPE_ROOMY2: Roomylevel2(); break;
		case DTYPE_TOWN: Generate_Town(); break;
		default:
			// if all else fails, we create a ROOMY level
			Roomylevel();
		break;
	}

	//skip everything else if these types
	if (dtype==DTYPE_WILDHUNT || dtype==DTYPE_OUTWORLD)
		return;

	factory.Add_Special_Monsters(this);

	int i, num;
	Spawner spw(this);

	if (dtype==DTYPE_TOWN)
	{
		//create these manually
		Shop_Init();
		Create_Stairs(); //note: sometimes stairs are created in a shop...
		return;
	}

	Create_Stairs();

	num=3+RANDU(10);	// how many items to create
	for (i=0; i<num; i++)
	{
		Coord c=find_random_location(this, 1);

		const int rnum=RANDU(100);

		if (rnum < 15)
			spw.Create_Item(c, IS_SCROLL, -1, 1, -1);
		else if (rnum < 30)
			spw.Create_Item(c, IS_WEAPON1H, -1, 1, -1);
		else if (rnum < 70)
			spw.Create_Item(c, IS_ARMOR, -1, 1, -1);
		else
			spw.Create_Item(c, ISMG_MISCITEM, -1, 1, -1);
	}

	// add gold and coins
	for (i=0; i<RANDU(5); i++)
	{
		Coord c=find_random_location(this, 1);

		spw.Create_Item(c, IS_MONEY, -1, RANDU(128), -1);
	}

	// create monsters but not to players location
	for (i=0; i<5+RANDU(6); i++)
	{
		Coord c=find_random_location(this, 2, true);

		factory.Add_Monster(this, c.x, c.y, 0);
	}

	/* generate lairs and dungeon shops */
	//note: wouldn't a lair created now erase stairs randomly?
	generatelair(this);

	/* init shops if needed */
	Shop_Init();
}

void Feature_Level::Generate_Town()
{
	create_random_terrain(this, TYPE_TREE, 150);

	generate_townroom(this, ROOMTMPL_CASTLE1);

	if (generate_townroom(this, ROOMTMPL_SHOP1))
	{
		roomdef &r=get_last_created_room();
		r.kauppa.Shopify(Shoppe::General);
	}

	if (generate_townroom(this, ROOMTMPL_SHOP5))
	{
		roomdef &r=get_last_created_room();
		//note: shoptype was set to none, is this right?
		r.kauppa.Shopify(Shoppe::None);
	}

	generate_townroom(this, ROOMTMPL_FARM);
	generate_townroom(this, ROOMTMPL_HOUSE1);
	generate_townroom(this, ROOMTMPL_HOUSE1);
	generate_townroom(this, ROOMTMPL_HOUSE2);
	generate_townroom(this, ROOMTMPL_HOUSE1);
}

void Feature_Level::Generate_Wildhunt()
{
	//note: wildhunt is never used, but if it's used again the level needs to be
	//created the same way as in Levelnode::Visit with size above and TYPE_GRASS
	//as basetile

	create_random_terrain(this, TYPE_TREE, 150);
	player.Go_Hunting();
}

void Feature_Level::Roomylevel()
{
	int i;
	//   int16u ocx=0, ocy=0, ncx=0, ncy=0;

	//   int16u tlx, tly, brx, bry;
	/*
	  for(i=0; i<8; i++) {
	  if(ncx && ncy) {
	  ocx=ncx;
	  ocy=ncy;
	  }
	  createroom(level, &ncx, &ncy);
	//      createroom(level, &tlx, &tly, &brx, &bry);

	if(ocx && ocy && ncx && ncy)
	carvepassage2(level, ocx, ocy, ncx, ncy, true);
	}
	*/

	int x1=0, y1=0, sx=0, sy=0;
	int ox1=0, oy1=0, osx=0, osy=0;	/* old values */

	int cx, cy, difx, dify;
	int ocx, ocy;
	int pasy, pasx;
	int doory, doorx;
	int dir;

	for (i=0; i<4+RANDU(ROOM_MAXNUM-4); i++)
	{
		if (x1 && y1 && sx && sy)
		{
			ox1=x1;
			oy1=y1;
			osx=sx;
			osy=sy;
		}

		/* plant a room */
		if (!createroom(this, &x1, &y1, &sx, &sy))
			continue;

		//      my_printf("old room at %d:%d, %d,%d\n", ox1, oy1, osx, osy);
		//      my_printf("created room at %d:%d, %d,%d\n", x1, y1, sx, sy);
		//		my_getch();
		if (x1 && y1 && ox1 && oy1)
		{

			/* get center coords for rooms */
			cx=x1+(sx/2);
			cy=y1+(sy/2);
			ocx=ox1+(osx/2);
			ocy=oy1+(osy/2);

			difx=ocx-cx;
			dify=ocy-cy;

			if (abs(difx) > abs(dify))
			{
				if (cx > ocx)
				{
					doorx=x1;
					doory=y1+sy/2;
					pasx=x1-1;
					pasy=y1+sy/2;
					dir=3;
				}
				else
				{
					doorx=x1+sx-1;
					doory=y1+sy/2;
					pasx=x1+sx;
					pasy=y1+sy/2;
					dir=1;
				}

			}
			else
			{
				if (cy > ocy)
				{
					doorx=x1+sx/2;
					doory=y1;
					pasx=x1+sx/2;
					pasy=y1-1;
					dir=0;

				}
				else
				{
					doorx=x1+sx/2;
					doory=y1+sy-1;
					pasx=x1+sx/2;
					pasy=y1+sy;
					dir=2;
				}
			}
			Create_Door(doorx, doory, true);
			carvepassage2(this, pasx, pasy, ocx, ocy, true, dir);
		}
	}
}

void Feature_Level::Roomylevel2()
{
	makeroom(this, sizex/2, sizey/2, -1, 0);
}
