//Legend of Saladir - rooms.h

//Unit rooms: Room dungeon feature.

#ifndef ROOMS_H
#define ROOMS_H

#include <string>
#include "geometry.h"
#include "shoppe.h"

//room types in roomdef's 'type'
#define ROOM_DUNGEON 0 //regular rectangular dungeon room
#define ROOM_LAIR 1
#define ROOM_SHOP 2
#define ROOM_VAULT 3
#define ROOM_CASTLE 4
#define ROOM_FARM 5
#define ROOM_HOUSE 6
#define ROOM_SMALL_CAVE 7

#define ROOM_MINX		6
#define ROOM_MINY		6
#define ROOM_MAXX		20
#define ROOM_MAXY		10

#define ROOM_MAXNUM		20
#define ROOM_MIN		4

//special flags for a room
#define ROOM_IS_VISITED	0x01
#define ROOM_PLAYERHERE 0x80 //note: can be removed later

#define ROOMTMPL_SHOP1		0
#define ROOMTMPL_SHOP2		1
#define ROOMTMPL_SHOP3		2
#define ROOMTMPL_SHOP4		3
#define ROOMTMPL_SHOP5		4
#define ROOMTMPL_SHOP6		5
#define ROOMTMPL_CASTLE1	6
#define ROOMTMPL_FARM		7
#define ROOMTMPL_HOUSE1		8
#define ROOMTMPL_HOUSE2		9

struct roomdef : public Area
{
	int type;		// room type, shop, vault etc..
	int8u flags;	// room special flags

	Shoppe kauppa; //shop, if any

	int doorx;		// coords for the door
	int doory;

	being *owner;	// room owner

	roomdef();
	roomdef(int rt, int x, int y, int a, int b);

	Area &Get_Area() { return *this; }
	const char *Get_Name();
	bool Is_Shop();
	bool Is_Visited();

	void Check_Visit();
	void set_door(int a, int b);
	void make_shop(level_type *level); //change this room to a shop
	void Project_Room_Id(int id);

	void Display_Data(int rindex);

	void Save(Tar_Ball &tb, level_type *lvl);
	void Load(Tar_Ball &tb, level_type *lvl);
};

#endif
