/**************************************************************************
 * rig.cpp --                                                             *
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

//Refactored 4.9.2021 - 26.3.2026 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "items.h"
#include "names.h"
#include "output.h"
#include "rig.h"
#include "roleplay.h"
#include "script.h"
#include "selpack.h"
#include "storage.h"

using std::string;

const char *equipment::equip_slotdesc[]=
{
	"head",		/* crown, helmet, jewelry */
	"neck",
	"left ring",
	"right ring",

	"left hand",	/* weapons, tools */
	"right hand",
	"missiles",
	"tool",

	"cloak",		/* clothes */
	"shirt",
	"gloves",
	"pants",
	"boots",

	"upper body", 	/* protective armor */
	"left arm",
	"right arm",
	"legs",
	"Odd slot",
	"?????????",
	"?????????",
	"?????????",
	0
};

const equipment::Equipment_Event equipment::events[Amt_Of_Events]=
{
	{C_GREEN, "(equipped)"}, //messages with () a special routine is used
	{C_GREEN, "Item removed."},
	{C_RED, "You have no items to equip!"},
	{C_YELLOW, "Nothing selected."},
	{C_YELLOW, "That slot is reserved!"},

	{C_YELLOW, "Item doesn't fit there!"},
	{C_RED, "Can't use two handed weapons now!"},
	{C_RED, "(item unpaid)"},
	{C_RED, "This item is cursed, can't remove!"},
	{C_RED, "Item is already in use!"},

	{C_RED, "The item mysteriously disappeared!"}
};

equipment::equipment()
{
	for (int t=0; t<MAX_EQUIP; t++)
		equip[t].Clear();
}

void equipment::clear_slot(int slot)
{
	/* stupid check for 2handed weapons */
	/* 2 handed weapons can only be held in HANDS so it works */
	if (equip[slot].item->i.type == IS_WEAPON2H ||
		equip[slot].item->i.type == IS_MISWEAPON)
	{
		equip[EQUIP_LHAND].reserv=0;
		equip[EQUIP_RHAND].reserv=0;
	}

	equip[slot].Clear();
}

bool equipment::decide_meleeweapon(bool *useleft, bool *useright)
{
	*useleft = false;
	*useright = false;
	item_def *rweap=0;
	item_def *lweap=0;

	if (equip[EQUIP_RHAND].item)
		rweap=&equip[EQUIP_RHAND].item->i;

	if (equip[EQUIP_LHAND].item)
		lweap=&equip[EQUIP_LHAND].item->i;

	/* no weapons in hand then use hand damage */
	if (!rweap && !lweap)
		return false;

	if (rweap)
	{
		if (rweap->type==IS_WEAPON2H)
		{
			*useright = true;
			return true;
		}

		if (rweap->type==IS_WEAPON1H)
			*useright = true;
	}

	if (lweap)
	{
		if (lweap->type==IS_WEAPON2H)
		{
			*useleft = true;
			return true;
		}

		if (lweap->type==IS_WEAPON1H)
			*useleft = true;
	}

	return true;
}

int equipment::equip_checkfit(int slot, item_def *item)
{
	//note: why missile and tool return -1? Also, check return values...
	if (slot==EQUIP_MISSILE)
		return -1;

	if (slot==EQUIP_TOOL)
		return -1;

	if (slot==EQUIP_LRING || slot==EQUIP_RRING)
	{
		if (item->type==IS_RING)
			return -1;
	}

	if (item->type == IS_MISSILE)
	{
		//note: checking for missile can't be done, because of the first check
		if (slot==EQUIP_MISSILE || slot==EQUIP_RHAND || slot==EQUIP_LHAND)
			return -1;
	}
	else if (item->type == IS_ARMOR)
	{
		if (slot==item->group)
			return -1;
	}
	else if (item->type == IS_WEAPON1H || item->type == IS_MISWEAPON)
	{
		if (slot==EQUIP_LHAND || slot==EQUIP_RHAND)
			return -1;
	}
	else if (item->type == IS_WEAPON2H)
	{
		if (slot==EQUIP_LHAND && equip[EQUIP_RHAND].in_use==false)
		{
			if (equip[EQUIP_RHAND].status==EQSTAT_OK)
				return EQUIP_RHAND;
			else
				return -9;
		}
		if (slot==EQUIP_RHAND && equip[EQUIP_LHAND].in_use==false)
		{
			if (equip[EQUIP_LHAND].status==EQSTAT_OK)
				return EQUIP_LHAND;
			else
				return -9;
		}
		return -10;
	}

	/* any item can be used in hand */
	if (slot==EQUIP_LHAND || slot==EQUIP_RHAND)
		return -1;

	return -10;
}

void equipment::display_result(int what, int slot)
{
	const int y=SCREEN_LINES-2;

	if (what<0 || what>=Amt_Of_Events)
	{
		set_color(CHB_RED);
		print_centered(y, "Error! Unknown result code!");
		return;
	}

	my_setcolor(events[what].color);

	switch (what)
	{
		case Item_Unpaid:
			print_centered(y, texts->Get_Random(Script::Unpaid_Item));
		break;
		case Equipped:
		{
			string s("Equipped ");
			s.append(get_equipment_name(slot));
			s.append(".");
			print_centered(y, s.c_str());
		}
		break;
		default:
			print_centered(y, events[what].message);
		break;
	}
}

void equipment::display_description(int slot)
{
	my_printf("[%s] ", equip_slotdesc[slot]);
}

int equipment::equipitem(int slot, playerinfo &plr)
{
	if (equip[slot].Is_Reserved())
		return Slot_Reserved;

	if (equip[slot].Is_Empty())
	{
		int res=0; //note: always set to zero, see check below
		plr.backpack->Set_Filter(-1);
		invnode *equipit=plr.backpack->Get_Handle("What do you want to equip?");

		if (equipit)
		{
			if ((equipit->i.status & ITEM_UNPAID))
				return Item_Unpaid;

			if (equipit->slot >= 0)
				return Already_In_Use;

			int eres=equip_checkfit(slot, &equipit->i);

			if (eres>0)
				reserve(eres, slot);

			if (eres == -9)
				return Cant_Use_Twohanded;

			if (eres >= -1)
			{
				/* identify item when equipped */
				if (equipit->Is_Weapon() || equipit->Is_Armor())
					equipit->Identify();

				put_on(equipit, slot);

				/* if equipping light, make it visible */
				if (equipit->Is_Lightsource())
				{
					if (equipit->i.group>=LIGHT_MAXNUM)
						equipit->i.group=LIGHT_MAXNUM-1;
					plr.light=equipit->i.pmod1;
				}

				display->Item_Info(equipit, 1, "Equipped");

				return Equipped;
			}
			else return Does_Not_Fit;
		}
		else
		{
			if (res==0) //note: this is always true
			{
				/* no items to equip */
				return No_Items_To_Equip;
			}
			return Nothing_Selected;
		}
	}
	else
	{
		invnode *equipit=get_inventory_item(slot);

		if (equipit->i.status & ITEM_CURSED)
			return Cursed;

		if (equipit->i.type==IS_LIGHT)
			plr.light=1;

		/* mark item unequipped to enable piling again */
		equipit->slot=-1;

		clear_slot(slot);

		display->Item_Info(equipit, 1, "Removed");

		return Removed;
	}
}

bool equipment::Get_Ranged_Gear(int &wpntype, int &mistype, int &bonus)
{
	/* first check if there is something to throw/shoot */
	if (!equip[EQUIP_MISSILE].item)
		return false; /* nothing */

	/* do we have a missile weapon equipped, get it's type to wpntype */
	if (equip[EQUIP_RHAND].item)
	{
		if (equip[EQUIP_RHAND].item->i.type == IS_MISWEAPON)
		{
			wpntype=equip[EQUIP_RHAND].item->i.group;
			bonus=equip[EQUIP_RHAND].item->i.misdam_mod;
		}
	}

	if (equip[EQUIP_LHAND].item)
	{
		if (equip[EQUIP_LHAND].item->i.type == IS_MISWEAPON)
		{
			wpntype=equip[EQUIP_LHAND].item->i.group;
			bonus=equip[EQUIP_LHAND].item->i.misdam_mod;
		}
	}

	/* then check if the item is an arrow or a bolt (IS_MISSILE)*/
	if (equip[EQUIP_MISSILE].item->i.type == IS_MISSILE)
		mistype=equip[EQUIP_MISSILE].item->i.group;

	return true;
}

const char *equipment::get_equipment_name(int slot)
{
	item_def *i=get_item(slot);
	if (i==0) return "mysterious banana";
	return i->name.c_str();
}

item_def *equipment::get_item(int slot)
{
	invnode *in=equip[slot].item;
	if (in==0)
		return 0;

	//return invnode's item_def
	return &in->i;
}

invnode *equipment::get_inventory_item(int slot)
{
	return equip[slot].item;
}

bool equipment::is_usable(int slot)
{
	return equip[slot].Is_Usable();
}

void equipment::make_usable(int slot)
{
	set_status(slot, EQSTAT_OK);
}

bool equipment::monster_equip(level_type *level, being *monster, invnode *useitem)
{
	int stat1=Removed, stat2=Removed;
	int useslot=EQUIP_RHAND;

	/* put twhohanded weapons always on RIGHT hand */
	if (useitem->i.type == IS_WEAPON2H ||
		useitem->i.type == IS_MISWEAPON)
	{
		invnode *eitem=get_inventory_item(EQUIP_RHAND);

		if (eitem)
			stat1=monster_unequip(level, monster, eitem);

		eitem=get_inventory_item(EQUIP_LHAND);

		if (eitem)
			stat2=monster_unequip(level, monster, eitem);
	}
	else if (useitem->i.type == IS_WEAPON1H)
	{
		//check right hand
		if (equip[useslot].Is_Empty()==false)
			useslot=EQUIP_LHAND;

		//note: THIS NEED TO BE FIXED ! ..wonder why.
		//check also left hand
		if (equip[useslot].Is_Empty()==false)
			return false;
	}

	if (stat1 == Removed && stat2 == Removed)
	{
		/* equip the item */
		put_on(useitem, useslot);

		if (gameview.Is_Visible(monster->x, monster->y))
		{
			//      if(vision->Cansee(player.Get_Location(), monster->Get_Location(),
			//		player.sight, true)) {

			string s=monster_sprintf(monster, true, true);
			s.append(" just equipped");
			display->Item_Info(useitem, s.c_str());
		}
		return true;
	}

	return false;
}

int equipment::monster_unequip(level_type *level, being *monster, invnode *removeitem)
{
	if (removeitem->slot < 0)
		return Already_In_Use;

	if (removeitem->i.status & ITEM_CURSED)
		return Cursed;

	if (gameview.Is_Visible(monster->x, monster->y))
	{
		string s=monster_sprintf(monster, true, true);
		s.append(" unequipped");
		display->Item_Info(removeitem, s.c_str());
	}

	/* remove equipped item from the slot */
	clear_slot(removeitem->slot);

	return Removed;
}

void equipment::player_equip()
{
	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
	const int y=SCREEN_LINES-1;

	/* show equip slots */
	show();

	int key=1;
	while (key!=32)
	{
		key=my_getch();
		int slot=-1;

		if (key>='A' && key <='A'+MAX_EQUIP-1)
			slot=key-'A';
		else if (key>='a' && key <='a'+MAX_EQUIP-1)
			slot=key-'a';

		if (slot==-1)
		{
			drawline(y, ' ');
			continue;
		}

		if (is_usable(slot)==false)
		{
			print_centered(y, "That slot is unusable!");
			continue;
		}

		const int result=equipitem(slot, player);
		show();
		display_result(result, slot);
	}

	/* calculate effect from items */
	roleplay.Calculate_Itembonus(&player);
}

void equipment::put_on(invnode *ni, int slot)
{
	equip[slot].in_use=true;
	equip[slot].item=ni;
	ni->slot = slot;
}

void equipment::reserve(int slot, int dest_slot)
{
	equip[slot].reserv=dest_slot;
}

void equipment::set_status(int slot, int8u st)
{
	equip[slot].status=st;
}

void equipment::show()
{
	display->Header("Equipment in use", CH_WHITE);

	goto_content();

	for (int i=0; i<MAX_EQUIP; i++)
	{
		my_setcolor(C_GREEN);

		if (i==0)
			my_printf("Helmet, crown & jewelry\n");
		if (i==4)
			my_printf("Weapons & Tools\n");
		if (i==8)
			my_printf("Clothes, Protection & Armour\n");

		if (is_usable(i)==false)
		{
			set_color(CH_RED);
			my_printf(" !%c) %-10s  broken or unusable!", 'A'+i, equip_slotdesc[i]);
		}
		else if (equip[i].Is_Reserved()==false)
		{
			set_color(CH_RED);
			my_printf("  %c", 'A'+i);
			set_color(C_YELLOW);
			my_printf(") %-10s ", equip_slotdesc[i]);
			set_color(C_WHITE);
			my_printf(": ");
			invnode *item=get_inventory_item(i);

			if (item==0)
				my_printf("no item");
			else
				display->Item_Info(item, 1, 0);
		}
		else
		{
			my_setcolor(CH_DGRAY);
			my_printf("  %c) %-10s  reserved by another item!",
				'A'+i, equip_slotdesc[i]);
		}

		my_printf("\n");
	}

	display->Footer("Select slot to access      [SPACE] to return", CH_WHITE);
}

void equipment::save(Tar_Ball &tb)
{
	for (int i=0; i<MAX_EQUIP; i++)
		equip[i].Save(tb);
}

void equipment::load(Tar_Ball &tb, inventory &inv)
{
	for (int i=0; i<MAX_EQUIP; i++)
		equip[i].Load(tb, inv, i);
}
