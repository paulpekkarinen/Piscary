/**************************************************************************
 * spawner.cpp --                                                         *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 29.5.2023 - 2.4.2026 Paul K. Pekkarinen

#include "amount.h"
#include "avatar.h"
#include "caves.h"
#include "dice.h"
#include "gameview.h"
#include "invnode.h"
#include "itempack.h"
#include "material.h"
#include "message.h"
#include "spawner.h"

Spawner::Spawner(level_type *luola)
	: lvl(luola)
{

}

invnode *Spawner::Create_Item(const Coord &c, int type, int subtype, int count, int material)
{
	if (lvl->Is_Outside(c))
	{
		msg.newmsg("Error: level's create item was outside level.", CHB_RED);
		return 0;
	}

	invnode *inode = lvl->inv.Create_Item(
		Itempack(type, subtype, count, material));

	if (!inode)
	{
		msg.newmsg("Error: level's create item returned a NULL pointer.",
			CHB_RED);
		msg.vnewmsg(CHB_RED, "Type = %d, Subtype = %d, Count = %d!",
			type, subtype, count);
		return 0;
	}

	/* should set some random stats for the item in question */
	Set_Random_Condition(&inode->i);

	/* init level coordinates */
	inode->x = c.x;
	inode->y = c.y;

	gameview.Put_Item(inode);

	/* if we created a item which can't be walked on */
	/* mark item not passable */
	if (inode->i.status & ITEM_NOTPASSABLE)
		lvl->Set_Impassable(c);

	return inode;
}

int Spawner::Get_Random_Item(int type)
{
	item_def *igptr;
	int igmax;
	int result = -1;

	if(type == IS_FOOD || type==IS_LIGHT || type == IS_CONTAINER)
	{
		igptr = miscitems;
		igmax = mucho.num_miscitems;
	}
	else
		return -1;

	while(1)
	{
		result = RANDU(igmax);

		if((igptr+result)->type == type)
			return result;
	}

	return result;
}

void Spawner::Set_Random_Condition(item_def *item)
{
	if (item->status & ITEM_ARTIFACT)
		return;

	if (item->type == IS_SPECIAL || item->type == IS_MONEY)
		return;

	item->icond = COND_USED;

	int pluck = player.stat[STAT_LUC].Get() * (RANDU(100/STATMAX_LUCK));

	for (int i=0; i<5; i++)
	{
		if (RANDU(100) < pluck)
		{
			if (item->icond > 0)
				item->icond--;
		}
		else if (item->icond < COND_BROKEN)
			item->icond++;
		else
			item->icond = RANDU(COND_BROKEN);
	}

	if (item->icond==COND_BROKEN)
		item->icond--;
}

void Spawner::Create_Shop_Item(const Coord &c, int shoptype, int sellp)
{
	invnode *iptr=0;
	int ritem;
	int itype;
	real price, pr2;

	/* food shop */
	if(shoptype==Shoppe::Food)
	{
		ritem = Get_Random_Item(IS_FOOD);
		if(ritem>=0 && ritem<mucho.num_miscitems)
		{
			iptr=Create_Item(c, ISMG_MISCITEM, ritem, 1, -1);
		}
	}
	else if(shoptype==Shoppe::Weapon)
	{
		itype=RANDU(100);
		if(itype < 90)
			iptr=Create_Item(c, IS_WEAPON1H, -1, 1, -1);
	}
	else if(shoptype==Shoppe::Armour)
	{
		iptr=Create_Item(c, IS_ARMOR, -1, 1, -1);
	}
	/* general shop items */
	else
	{
		itype=RANDU(100);
		if(itype<10)
			iptr=Create_Item(c, IS_SCROLL, -1, 1, -1);
		else if(itype<25)
			iptr=Create_Item(c, ISMG_MISCITEM, -1, 1, -1);
		else if(itype < 60)
			iptr=Create_Item(c, IS_WEAPON1H, -1, 1, -1);
		else if(itype < 90)
			iptr=Create_Item(c, IS_ARMOR, -1, 1, -1);
	}

	if(iptr)
	{
		if(iptr->i.type != IS_FOOD)
		{
			pr2=(real)sellp;
			pr2=pr2/100;
			price=(real)iptr->i.price;
			price=price * materials[iptr->i.material].vmod * pr2;
			iptr->i.price=(int32u)price;
			/* make the item unpaid */
		}
		iptr->i.status|=ITEM_UNPAID;
	}
}
