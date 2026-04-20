//Legend of Saladir - viewtile.h

//Unit viewtile: Tile of a gameview.

#ifndef VIEWTILE_H
#define VIEWTILE_H

struct being;
struct invnode;

//One tile of a gameview.
struct Viewtile
{
	enum Vision_Types {Visible, Dark};

	int vision; //can player see this tile (Visible=yes)
	int room_id; //-1 if not in any room, otherwise the room array index
	being *tonttu; //creature on this tile (excluding player)
	invnode *esine; //topmost item on this tile

	//TRAP_... type, this includes both found or hidden traps, determined by
	//CAVE_TRAPIDENT in level map data
	//note: can't display if doesn't know hidden/visible state
	int trap_type;

	Viewtile();

	bool Show();
	bool Show_Object();
};

#endif
