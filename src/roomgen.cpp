/**************************************************************************
 * roomgen.cpp --                                                         *
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

//Refactored 4.9.2021 - 27.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "codex.h"
#include "dice.h"
#include "factory.h"
#include "genlevel.h"
#include "lexicon.h"
#include "randgen.h"
#include "roomgen.h"
#include "rooms.h"
#include "saldebug.h"
#include "terrain.h"

/* directions of movement (for level generator)
   also directions of rooms and doors etc.
   */
#define DIR_NORTH    0
#define DIR_EAST     1
#define DIR_SOUTH    2
#define DIR_WEST     3
#define DIR_UNKNOWN  100

/* Here're the templates for lairs */
/* descriptions:
   xsize, ysize,
   door direction,
   string for the room,

   # unpassable wall
   & dark unpassable wall (will not show up)
   . room floor
   , dark room floor
   ! door will be created here
   ? monster will be created here
   a-Z specific monster
*/

Roomtemplate lairtemplates[]=
{
  { 	7, 7,
   	DIR_EAST,
	ROOM_SHOP,
	"#######" /* small shop */
	"#.....#"
	"#.....#"
	"#.....!"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	7, 7,
   	DIR_WEST,
	ROOM_SHOP,
	"#######"	/* small shop */
	"#.....#"
	"#.....#"
	"!.....#"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	7, 7,
   	DIR_SOUTH,
	ROOM_SHOP,
	"#######"	/* small shop */
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"###!###"
  },
  { 	7, 7,
   	DIR_NORTH,
	ROOM_SHOP,
	"###!###"	/* small shop */
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"#######"
  },
  { 	10, 10,
   	DIR_WEST,
	ROOM_LAIR,
	"##&&&&&&##"
	"##&,,,,&##"
	"#&&,,,,&&#"
	"#&,,,,,,&&"
	"!,,,,?,,,&"
	"#&,,,,,,&&"
	"#&,,,,,,&#"
	"#&&,,,,&&#"
	"##&&,,&&##"
	"###&&&&###"
  },
  { 	10, 10,
   	DIR_WEST,
	ROOM_LAIR,
	"##########"	/* zoo */
	"#........#"
	"#.b..b...#"
	"#...b..b.#"
	"!.b...b..#"
	"#...b..b.#"
	"#.b...b..#"
	"#...b..b.#"
	"#........#"
	"##########"
  },
  { 	15, 10,
   	DIR_EAST,
	ROOM_LAIR,
	"###############"
	"###....###....#"
	"###....###....#"
	"##......####..#"
	"#...?....###..!"
	"##......####..#"
	"##......###..##"
	"###....###..###"
	"####.......####"
	"###############"
  },
  { 	15, 10,
   	DIR_NORTH,
	ROOM_SMALL_CAVE,
	"####!##########"
	"####.#####....#"
	"###..k.###....#"
	"##.k....####..#"
	"#.....k.k###..#"
	"##..k..k####..#"
	"##k.....###..##"
	"###..k.###..###"
	"####.......####"
	"###############"
  },
  { 	15, 10,
   	DIR_EAST,
	ROOM_LAIR,
	"&&&&&&&&&&&&&&&" /* very dark lair of monsters */
	"&,,,g,&&&,,s,,&"
	"&&,G,&&,&,&,&&&"
	"&&,,&&,g,,&,&&&"
	"&,g,,&&,&&&,&,!"
	"&,g,g&&,,s&,&,&"
	"&,,s,&&,&&&,&,&"
	"&&,,&&&,&#&,&,&"
	"&&&,,,,,&#&,,,&"
	"#&&&&&&&&&&&&&&"
  },
  { 	11, 15,
   	DIR_SOUTH,
	ROOM_LAIR,
	"###########"
	"#.........#"
	"#.#######.#"
	"#.#..####.#"
	"#.#...###.#"
	"#.#.?...#.#"
	"#.#...#.#.#"
	"#.#..##.#.#"
	"#.#####.#.#"
	"#.....#.#.#"
	"#####.#.#.#"
	"#...#.#.#.#"
	"#.#.#.#.#.#"
	"#.#...#...#"
	"#!#########"
  },
  { 	15, 15,
   	DIR_NORTH,
	ROOM_LAIR,
	"############!##"
	"#.....#.####.##"
	"#####.#.......#"
	"#...#.#.#######"
	"#.#.#.#.#.#...#"
	"#.#.#.#.#.#.?.#"
	"#.###.#.#.#...#"
	"#.......#.#...#"
	"#.#####.#.##.##"
	"#.#.#.#...##.##"
	"#.....######.##"
	"#####........##"
	"#,,,#.#########"
	"#,,,..........#"
	"###############"
  },
  { 	30, 30,
   	DIR_EAST,
	ROOM_LAIR,
	"##############################"	   /* big lair */
	"##,,,,,##########..####,,,,,,#"
	"#,,,,,,,########....###,,,,,,#"
	"##,,,,,,,###.....?...##,,#,,,#"
	"###,,,######.#.......##,###,,#"
	"####,#######.####...###,###,##"
	"####,#####....#########,##,,##"
	"####,#####.......######,##,###"
	"####.........###..#####,,,,###"
	"####.############.#####,,#####"
	"####.############..###,,######"
	"####..##....######.,,,,,######"
	"#####....##..############.####"
	"#######.####..##########...###"
	"#######.#####.........#......!"
	"#######.#############......###"
	"#######.####....########..####"
	"##......####.##.....###..#####"
	"##..###...##.###...###..######"
	"##.######.#..###..####.#######"
	"##.######....###..##....######"
	"##.#########..##.##..##.######"
	"##..##..#####.##.##.###...####"
	"###.#...#####..####.#####..###"
	"###.......####.####.######.###"
	"###......#####......######.###"
	"###..?.....########.######.###"
	"##.....###..######..######...#"
	"###.#######........#########.#"
	"##############################"
  },
  { 	30, 22,
   	DIR_SOUTH,
	ROOM_LAIR,
	"##############################"	   /* big lair */
	"##.########################.##"
	"#....##..######..####.......##"
	"#.........#####.............##"
	"##...?...######.......###...##"
	"###.....########.....###..####"
	"###....#####...............###"
	"####....####...........G..#.##"
	"#####..###...................#"
	"####.............####.#...#..#"
	"##......######..########..####"
	"####..#########..########...##"
	"#....##########...########...#"
	"#....#########.....#######...#"
	"##......#####........#####...#"
	"##.......##.....#...#####....#"
	"##............####..#####...##"
	"###...........#,,##..###....##"
	"####..##.....##,,,##......####"
	"##....###..#,,,,,,,##...######"
	"###...######,,,,,,,###########"
	"################!#############"
  },
  { 0, 0, 0, 0, NULL }	/* end of the list */
};

/* desc:
   & do not change type
   | wooden bridge
   # wall
   . room floor
   = water
*/

Roomtemplate towntemplates[]=
{
  { 	15, 6,
   	DIR_SOUTH,
	ROOM_SHOP,
	"###############"	/* shop */
	"#.............#"
	"#.............#"
	"#.............#"
	"#....##!##....#"
	"######&&&######"
  },
  { 	15, 6,
   	DIR_NORTH,
	ROOM_SHOP,
	"######&&&######"	/* shop */
	"#....##!##....#"
	"#.............#"
	"#.............#"
	"#.............#"
	"###############"
  },
  { 	11, 6,
   	DIR_NORTH,
	ROOM_SHOP,
	"#####!#####"	/* small shop */
	"#.........#"
	"#.........#"
	"#.........#"
	"#.........#"
	"###########"
  },
  { 	11, 6,
   	DIR_SOUTH,
	ROOM_SHOP,
	"###########"	/* small shop */
	"#.........#"
	"#.........#"
	"#.........#"
	"#.........#"
	"#####!#####"
  },
  { 	8, 6,
   	DIR_EAST,
	ROOM_SHOP,
	"########"	/* small shop */
	"#......#"
	"#......!"
	"#......#"
	"#......#"
	"########"
  },
  { 	8, 6,
   	DIR_WEST,
	ROOM_SHOP,
	"########"	/* small shop */
	"#......#"
	"!......#"
	"#......#"
	"#......#"
	"########"
  },
  { 	21, 17,
   	DIR_NORTH,
	ROOM_CASTLE,
	"&========|||========&"  /* small castle */
	"==########!########=="
	"=##......#.#......##="
	"=#....####.####....#="
	"=##+###.......###+##="
	"=#....#............#="
	"=#....#............#="
	"=#....#.....#####+##="
	"=#....#.....#......#="
	"=#....#.....#......#="
	"=#....#.....#......#="
	"=#....#.....#####+##="
	"=#....#.....#......#="
	"=#....+.....+......#="
	"=##...#.....#.....##="
	"==#################=="
	"&===================&"
  },
  { 	15, 10,
   	DIR_SOUTH,
	ROOM_FARM,
	"#######&#######"
	"#.....#&#.....#"
	"#.....#&#.....#"
	"##+####&#.....#"
	"&&&&&&&&#.....#"
	"##+####&###!###"
	"#.....#&&&&&&&&"
	"#.....#&&&&&&&&"
	"#.....#&&&&&&&&"
	"#######&&&&&&&&"
  },
  { 	7, 6,
   	DIR_SOUTH,
	ROOM_HOUSE,
	"#######"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"##!####"
  },
  { 	7, 8,
   	DIR_NORTH,
	ROOM_HOUSE,
	"&##!##&"
	"##...##"
	"#.....#"
	"#.....#"
	"#.....#"
	"#.....#"
	"##...##"
	"&#####&"
  },
  { 0, 0, 0, 0, NULL }	/* end of the list */
};

const int delta_x[]={0, 1, 0, -1, 0};
const int delta_y[]={-1, 0, 1, 0, 0};

/* Plant a room in to the level!
**
** It returns the topleft and bottomright coordinates for the room
*/
bool createroom(Genlevel *level, int *rx1, int *ry1, int *sizex, int *sizey)
{
	int x1, y1, i, j;
	int rsizex, rsizey;
	int maxtries=100;
	int8u newflags=CAVE_LIGHT;

	/* create dark room */
	if (RANDU(100)>80)
		newflags=0;

	/* random size */
	rsizex=ROOM_MINX+RANDU(ROOM_MAXX-ROOM_MINX);
	rsizey=ROOM_MINY+RANDU(ROOM_MAXY-ROOM_MINY);

	/* random upper left corner */
	while (1)
	{
		x1=3+RANDU(level->sizex - rsizex - 6);
		y1=3+RANDU(level->sizey - rsizey - 6);

		if (checkregion(level, x1-2, y1-2, rsizex+4, rsizey+4))
			break;

		if (!maxtries--)
		{
			*rx1=*ry1=0;
			*sizex=*sizey=0;
			return false;
		}
	}

	/* if roomtable is already full */
	if (level->max_rooms_created())
	{
		*rx1=*ry1=0;
		*sizex=*sizey=0;
		return false;
	}

	roomdef ordroom(ROOM_DUNGEON, x1+1, y1+1, x1+rsizex-2, y1+rsizey-2);
	level->rooms.push_back(ordroom);

	for (j=0; j<rsizey; j++)
	{
		for (i=0; i<rsizex; i++)
		{
			Coord c(x1+i, y1+j);

			level->Create_Floor(c, TYPE_ROOMFLOOR, terrains[TYPE_ROOMFLOOR].flags | newflags);

			if (j==0 || i==0 || j==rsizey-1 || i==rsizex-1)
			{
				level->Create_Wall(c, TYPE_WALLIP);
			}
		}
	}

	*rx1=x1;
	*ry1=y1;
	*sizex=rsizex;
	*sizey=rsizey;

	return true;
}

bool generatelair(Genlevel *level)
{
	/* if roomtable is already full */
	if (level->max_rooms_created())
		return false;

	Roomtemplate *rooms=lairtemplates+RANDU(mucho.num_lairs);
	int x1, y1, i, j;
	int wd=rooms->sx;
	int hg=rooms->sy;
	int maxr=0;

	while (1)
	{
		x1=4+RANDU(level->sizex-8-wd);
		y1=4+RANDU(level->sizey-8-hg);

		if (checkregion(level, x1-1, y1-1, wd+2, hg+2))
		{
			break;
		}
		maxr++;
		if (maxr>=99)
			return false;
	}

	roomdef lairroom(rooms->roomtype, x1, y1, x1+wd-1, y1+hg-1);

	//sets to 'no shop', this is the default value so it's set just in case
	if (rooms->roomtype == ROOM_SHOP)
		lairroom.kauppa.Shopify(Shoppe::None);

	int16u lastattr=terrains[TYPE_ROOMFLOOR].flags;
	int32u index=0;

	for (j=0; j<hg; j++)
	{
		for (i=0; i<wd; i++)
		{
			Coord c(x1+i, y1+j);

			/* create passable areas (floor) */
			level->Create_Floor(c, TYPE_ROOMFLOOR, lastattr);

			const char tile=rooms->room[index++];

			switch (tile)
			{
				case '#': //impassable wall
					level->Create_Wall(c, TYPE_WALLIP, true, false);
				break;
				case '&': //non-lit wall
					level->Create_Wall(c, TYPE_WALLIP, true, true);
				break;
				case ',':
					level->Clear_Flag(c, CAVE_LIGHT);
					clear_flag_bit(lastattr, CAVE_LIGHT);
				break;
				case '.':
					level->Set_Flag(c, terrains[TYPE_ROOMFLOOR].flags);
					lastattr=terrains[TYPE_ROOMFLOOR].flags;
				break;
				case '?': //add room owner
					factory.Add_Monster(level, c.x, c.y, 0);
				break;
				case '!': //door
					level->Create_Door(c.x, c.y, false);
					lairroom.set_door(c.x, c.y);
				break;
				default:
				{
					//note: works only if each monster has a unique letter, because
					//Add_Monster's 'type' parameter is the ascii letter of
					//the monster (should probably replace with enum for types)
					if (is_alpha(tile))
						factory.Add_Monster(level, c.x, c.y, tile);
					else
						debug->Message(
							"Lair generator, unknown type: %c.", tile);
				}
				break;
			}
		}
	}

	makehall_only(level, lairroom.doorx, lairroom.doory, rooms->doordir);

	/* clear the sval */
	level->Clear_Svalbards();

	level->rooms.push_back(lairroom);

	return true;
}

bool generate_townroom(Genlevel *level, int roomtype)
{
	/* if roomtable is already full */
	if (level->max_rooms_created())
		return false;

	Roomtemplate *rooms=towntemplates+roomtype;
	int x1, y1, i, j;
	int wd=rooms->sx;
	int hg=rooms->sy;
	int maxr=0;

	while (1)
	{
		x1=4+RANDU(level->sizex-8-wd);
		y1=4+RANDU(level->sizey-8-hg);

		if (checkregion_inv(level, x1-2, y1-2, wd+4, hg+4))
		{
			break;
		}
		maxr++;
		if (maxr>=99)
			return false;
	}

	roomdef townroom(rooms->roomtype, x1, y1, x1+wd-1, y1+hg-1);
	int32u index=0;

	for (j=0; j<hg; j++)
	{
		for (i=0; i<wd; i++)
		{
			int tt = -1; //-1 if no terrain created
			Coord c(x1+i, y1+j);
			const int tile=rooms->room[index++];

			switch (tile)
			{
				case '#': tt=TYPE_WALLIP; break;
				case '&': break; //skip
				case '=': tt=TYPE_WATER; break;
				case '.':
				case ',':
					tt=TYPE_ROOMFLOOR;
				break;
				case '|': tt=TYPE_BRIDGEV; break;
				case '-': tt=TYPE_BRIDGEH; break;
				case '+': //normal door
					level->Create_Door(c.x, c.y, false);
				break;
				case 'T': tt=TYPE_TREE; break;
				case '!': // exitpoint (door)
					//generate door and add its coordinates to the roomlist
					level->Create_Door(x1+i, y1+j, false);
					townroom.set_door(x1+i, y1+j);
				break;
				default:
					debug->Message("Lair generator, unknown type: %c.", tile);
				break;
			}

			if (tt!=-1)
				level->Set_Terrain(c.x, c.y, tt);
		}
	}
	level->rooms.push_back(townroom);
	return true;
}

/* create a random length hall from x1,y1 to direction x */
void makehall(Genlevel *level, int x1, int y1, int dir, int recd)
{
	if (recd>1000)
		return;

	/* random length */
	int len=4+RANDU(8);

	for (int i=0; i<len; i++)
	{
		/* move to dir */
		x1+=delta_x[dir];
		y1+=delta_y[dir];

		/* check boundaries */
		if (x1>=level->sizex-1 || y1>=level->sizey-1
			|| x1<1 || y1<1)
			return;

		Coord c(x1, y1);
		const int tt=level->Get_Terrain(c);

		if (tt==TYPE_PASSAGE || tt==TYPE_ROOMFLOOR)
			return;

		if (dir==0 || dir==2)
		{
			if (level->Is_Passable(x1-1, y1))
				return;
			if (level->Is_Passable(x1+1, y1))
				return;
		}
		if (dir==1 || dir==3)
		{
			if (level->Is_Passable(x1, y1-1))
				return;
			if (level->Is_Passable(x1, y1+1))
				return;
		}

		/* place terrain */
		level->Set_Terrain(x1, y1, TYPE_PASSAGE);

		if (i==0)
			level->Create_Door(x1, y1, true);
	}

	len=RANDU(100);
	if (len>50)
	{
		int dir2;

		while (1)
		{
			dir2=RANDU(4);
			if (dir2!=dir)
				break;
		}

		makehall(level, x1, y1, dir2, recd+1);
	}
	else if (len>4)
		makeroom(level, x1, y1, dir, recd+1);
}

void makehall_only(Genlevel *level, int x1, int y1, int dir)
{
	int Maxretr=10000;

	int len=10+RANDU(20);
	while (1 && Maxretr>0)
	{
		/* move to dir */
		x1+=delta_x[dir];
		y1+=delta_y[dir];

		/* try to avoid infinite loops */
		Maxretr--;

		/* check boundaries */
		if (x1>=level->sizex-2 || y1>=level->sizey-2
			|| x1<1 || y1<1)
		{
			x1-=delta_x[dir];
			y1-=delta_y[dir];
			dir=RANDU(4);
			continue;
		}

		Coord c(x1, y1);

		if (level->Is_Passable(c))
		{
			if (level->Is_Carveable(c)==false)
			{
				dir=RANDU(4);
				continue;
			}
			/* create a door there */
			level->Create_Door(x1-delta_x[dir], y1-delta_y[dir], true);

			return;
		}
		/* place terrain */
		/* sval must be cleared after this routine (note: is it cleared?) */
		level->Protect_From_Carving(c);
		level->Set_Terrain(c, TYPE_PASSAGE);

		len--;
		if (len==0)
		{
			len=10+RANDU(20);
			dir=RANDU(4);
		}
	}
}

void makeroom(Genlevel *level, int x1, int y1, int dir, int recd)
{
	/* if roomtable is already full */
	if (level->max_rooms_created())
		return;

	if (recd>1000)
		return;

	int x2=x1;
	int y2=y1;

	//create light room
	int16u newflags=CAVE_LIGHT;

	//or dark room randomly
	if (RANDU(100)>80)
		newflags=0;

	int hg, wd;
	while (1)
	{
		wd=ROOM_MINX+RANDU(ROOM_MAXX-ROOM_MINX);
		if (!(wd%2))
			break;
	}
	while (1)
	{
		hg=ROOM_MINY+RANDU(ROOM_MAXY-ROOM_MINY);
		if (!(hg%2))
			break;
	}
	if (dir==0)
	{	/* north */
		y2-=hg;
		x2-=wd/2;
	}
	else if (dir==1)
	{ /* east */
		y2-=hg/2;
	}
	else if (dir==2)
	{ /* south */
		x2-=wd/2;
	}
	else if (dir==3)
	{ /* west */
		x2-=wd;
		y2-=hg/2;
	}

	if (x2==0)
		x2++;
	if (y2==0)
		y2++;

	if (checkregion(level, x2, y2, wd, hg))
	{
		//note: could have a bug, didn't check location parameters
		//before cut-pasting this to here
		roomdef ordroom(ROOM_DUNGEON, x1, y1, x1+wd-1, y1+hg-1);
		level->rooms.push_back(ordroom);

		//carve floor area of the room into bedrock
		for (int j=0; j<hg; j++)
		{
			for (int i=0; i<wd; i++)
			{
				Coord c(x2+i, y2+j);
				level->Create_Floor(c, TYPE_ROOMFLOOR,
					terrains[TYPE_ROOMFLOOR].flags | newflags);
			}
		}

		for (dir=0; dir<4; dir++)
		{
			//determine doorway location for each wall facings
			x1=x2;
			y1=y2;
			if (dir==0)
			{
				x1+=1+RANDU(wd-2);
			}
			else if (dir==1)
			{
				x1+=wd-1;
				y1+=1+RANDU(hg-2);
			}
			else if (dir==2)
			{
				x1+=1+RANDU(wd-2);
				y1+=hg-1;
			}
			else if (dir==3)
			{
				y1+=1+RANDU(hg-2);
			}
			makehall(level, x1, y1, dir, recd+1);
		}
	}
	else
	{
		dir=RANDU(4);
		makehall(level, x1, y1, dir, recd+1);
	}
}
