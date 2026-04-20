/**************************************************************************
 * viewtile.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 8.12.2022 - 10.4.2026 Paul K. Pekkarinen

#include "being.h"
#include "invnode.h"
#include "itemdata.h"
#include "material.h"
#include "output.h"
#include "traps.h"
#include "viewtile.h"

Viewtile::Viewtile()
	: vision(Dark), room_id(-1), tonttu(0), esine(0), trap_type(TRAP_NOTRAP)
{

}

//Shows tile at screen coordinates if it's visible to player.
bool Viewtile::Show()
{
	//if not in FOV, show darkness (when this tile is explored)
	if (vision==Dark)
	{
		put_char(' ', C_BLACK);
		return true;
	}

	return Show_Object();
}

//Show only objects, this is also used by debug view level.
bool Viewtile::Show_Object()
{
	if (tonttu!=0)
	{
		put_char(npc_races[tonttu->m.race].out,
			npc_races[tonttu->m.race].color);
		return true;
	}

	if (esine!=0)
	{
		//note: pmod4 was probably supposed to be the color of fabric
		//	 if(ptr->i.material!=MAT_FABRIC)
		//	 else
		//	    my_setcolor( ptr->i.pmod4 );

		if (esine->i.type==IS_SPECIAL)
		{
			put_char(item_outfaces[esine->i.group].out,
				item_outfaces[esine->i.group].color);
		}
		else
		{
			put_char(gategories[esine->i.type].out,
				materials[esine->i.material].color);
		}

		return true;
	}

	//return false if no objects on this tile
	return false;
}
