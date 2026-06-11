/**************************************************************************
 * ranged.cpp --                                                          *
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

//Refactored 28.9.2021 - 12.4.2026 Paul K. Pekkarinen

#include "aim.h"
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "codex.h"
#include "damage.h"
#include "dice.h"
#include "display.h"
#include "gametime.h"
#include "gameview.h"
#include "input.h"
#include "itemdata.h"
#include "material.h"
#include "message.h"
#include "names.h"
#include "output.h"
#include "pack.h"
#include "quote.h"
#include "ranged.h"
#include "salamath.h"

using std::string;

bool ranged_checkhit(level_type *level, Actor *ranger,
	int x, int y, bool visible, int skill);

int monster_ranged_attack(being *mptr, level_type *level)
{
	/* if not in attack mode, return */
	if (!(mptr->m.status & MST_ATTACKMODE))
		return 0;

	int askill, agrp;
	if (!mptr->Get_Rangedskill(&agrp, &askill))
		return 0;

	item_def *projectile=mptr->equips.get_item(EQUIP_MISSILE);

	const int itype=projectile->type;
	const int igroup=projectile->group;
	bool misres=true;
	int ticks=0; //how much time ranged attack takes

	Coord c=mptr->Get_Location();

	if (mptr->target==0)
	{
		int skillval=mptr->skills.check(agrp, askill);

		if (gameview.Cansee(c, player.Get_Location(), 10))
		{
			hidecursor();
			misres=ranged_line(level, 0, 60, true,
				mptr->x, mptr->y,
				c.x, c.y,
				ranged_checkhit, mptr, skillval);
			showcursor();

			ticks=mptr->Calculate_Time(TIME_MISSILEATTACK);
		}
	}

	if (!misres)
	{
		invnode *dip=mptr->equips.get_inventory_item(EQUIP_MISSILE);

		mptr->Drop_Single_Item(dip, c);
	}

	/* equip a new item from reserve if possible */
	mptr->inv.ready_newmissile(mptr->equips, itype, igroup);

	return ticks;
}

void ranged_attack(playerinfo *plr, level_type *level)
{
	int agrp, askill;
	if (!plr->Get_Rangedskill(&agrp, &askill))
		return;

	Aim ai(level, Aim::Target);

	if (ai.Select()==false)
	{
		msg.newmsg("No target selected.", C_WHITE);
		return;
	}

	gameview.Show();

	item_def *ranged=plr->equips.get_item(EQUIP_MISSILE);

	const int itype=ranged->type;
	const int igroup=ranged->group;

	int skillval=plr->skills.check(agrp, askill);

	/* give atleast some chance to hit the target */
	if (!skillval)
		skillval = 1 + RANDU(6);

	/* OBS!
	 *
	 * note: Etaisyysvaikutus ja atribuuttien vaikutus osuma
	 * tarkkuuteen ja ampumaetaisyyteen
	 *
	 */

	msg.vnewmsg(C_WHITE, "Skill value is %d!", skillval);

	char outchar=0;
	if (itype==IS_MISSILE)
		set_color(CH_WHITE);
	else
	{
		outchar=gategories[itype].out;
		set_color(materials[ranged->material].color);
	}
	hidecursor();

	Coord pc=plr->Get_Location();
	Coord tar=ai.Get_Target_Location();

	bool misres=ranged_line(level, outchar, 60, true,
		pc.x, pc.y,
		tar.x, tar.y, ranged_checkhit, plr, skillval);

	showcursor();

	/* if we didn't hit anything, we need to remove the item here! */
	if (!misres)
	{
		msg.newmsg("You missed everything.", C_WHITE);

		invnode *dip=plr->equips.get_inventory_item(EQUIP_MISSILE);

		plr->Drop_Single_Item(dip, tar);
	}

	/* equip a new item from reserve if possible */

	if (!plr->inv.ready_newmissile(plr->equips, itype, igroup))
		msg.newmsg("You've run out of missiles.", C_WHITE);

	plr->Spend_Time(TIME_MISSILEATTACK);
}

/*
 * Function which is called when a player throws a missile item,
 * ie. with a bow or with hands. This function is called for every
 * grid location the missile passes.
 *
 * The item used for throw (equip[EQUIP_MISSILE]) should NOT be removed
 * from the player's inventory
 * until the missile throwing sequence has ended.
 * This function needs that item.
 *
 * Remove the throw item at the end of this (hit) function.
 *
 * Return 'true' if the item hit something and was removed.
 *               in this case the ranged line function will END.
 * Return 'false' if not.
 *               in this case the ranged line will continue.
 *
 */
bool ranged_checkhit(level_type *level, Actor *ranger,
	int x, int y, bool visible, int skill)
{
	int damage;
	int i, j;
	const bool playerhere=player.Is_At(x, y);

	/* an array for target bodyparts */
	int bparts[HPSLOT_MAX+1]={0};

	//if at the origin of shooting
	if (x==ranger->x && y==ranger->y)
		return false;

	Coord c(x, y);

	/* is there a monster? */
	being *mptr=gameview.Get_Monster(c);

	/* no, no action required */
	if (!mptr && !playerhere)
		return false;

	int trace;
	if (mptr)
		trace=mptr->Get_Race();
	else if (playerhere)
		trace=player.Get_Race();

	/* collect target bodyparts which the race has */
	for (j=0, i=0; i<HPSLOT_MAX; i++)
	{
		if (npc_races[trace].bodyparts[i]>=0)
			bparts[j++]=i;
	}

	/* now get a random target bodypart */
	int bodyp=bparts[RANDU(j)];

	inventory *inv=&ranger->inv;
	const int bonpts=ranger->attackbonus;
	item_def *projectile=ranger->equips.get_item(EQUIP_MISSILE);
	equipment &gear=ranger->equips;
	const bool is_plr=ranger->Is_Player();

	string shootmess;

	/* if the player is acting */
	if (is_plr)
	{
		if (mptr)
		{
			string moname=monster_sprintf(mptr, false, true);
			shootmess="Your ";
			shootmess+=noun_verbs_something(projectile->name, "hits", moname);
		}
		else if (playerhere)
		{
			shootmess="You shoot your ";
			shootmess.append(bodyparts[bodyp]);
			shootmess.append("!");
		}
	}
	else
	{
		if (mptr)
		{
			string moname=monster_sprintf(mptr, false, true);
			shootmess=noun_verbs_something(projectile->name, "hits", moname);
		}
		else if (playerhere)
		{
			string bp(bodyparts[bodyp]);
			shootmess=noun_verbs_something(projectile->name, "hits your", bp);
		}
		shootmess[0]=toupper(shootmess[0]);
	}

	/* first check against the skill if we hit */
	if (throwdice(1, 100, 0) > skill)
	{
		if (!mptr)
			msg.vadd(C_WHITE, "An %s misses your %s.",
				projectile->name.c_str(),
				bodyparts[bodyp]);

		return false;
	}

	/* we hit! */
	damage=throwdice(projectile->missi_dt,
		projectile->missi_ds,
		projectile->missi_ds + bonpts);

	/* issue damage to target */
	if (mptr)
	{
		if (visible)
			msg.add(shootmess.c_str(), C_WHITE);

		if (damage_issue(level, mptr, ranger, ELEMENT_NOTHING, damage,
			bodyp, NULL) <= 0)
		{
			if (is_plr==false)
				ranger->Gain_Experience(mptr->Experience_Points_Earned());
			else
				player.Killedmonster(mptr);
		}
	}
	else if (is_plr==false)
	{
		if (damage_issue(level, &player, ranger, ELEMENT_NOTHING, damage,
			bodyp, NULL) <= 0)
		{
			ranger->Gain_Experience(player.Experience_Points_Earned());
		}
	}

	/* drop the item to the ground near target */
	invnode *dip=gear.get_inventory_item(EQUIP_MISSILE);
	ranger->Drop_Single_Item(dip, c);

	return true;
}

/*
 * Return 'true' if the item was removed from the inventory.
 * meaning that the item hit something.
 */
bool ranged_line(level_type *level, int out, int output_delay, bool single,
		 int x1, int y1, int x2, int y2,
		 bool (*hitfunc)(level_type *, Actor *ranger, int, int, bool, int),
		 Actor *ranger, int skill)
{
	int d;
	int outchar;
	bool vistat;
	Coord old;
	Coord levpos;
	Coord scpos;

	int dx = x2-x1;
	int dy = y2-y1;
	int sx = SIGN(dx);
	int sy = SIGN(dy);
	int ax = ABS(dx)<<1;
	int ay = ABS(dy)<<1;

	/* get a correct line character for output */

	if (!out)
		outchar=decidelinechar(x1, y1, x2, y2);
	else
		outchar=out;

	levpos.Set_Location(x1, y1);
	
	if (ax>ay)
	{		/* x dominant */
		d = ay-(ax>>1);
		for (;;)
		{
			if (gameview.Is_Visible(levpos))
			{
				vistat=true;
				old=gameview.Get_Screen_Location(levpos);
				
				gotoxy(old.x, old.y);
				addch(outchar);
				refresh();

				if (output_delay>0)
					delay(output_delay);
				if (single)
					gameview.Show();
			}
			else
				vistat=false;

			if (hitfunc!=NULL)
			{
				if (hitfunc(level, ranger, levpos.x, levpos.y, vistat, skill))
					return true;
			}

			if (levpos.x==x2) return false;

			old=levpos;

			if (d>=0)
			{
				levpos.y += sy;
				d -= ax;
			}
			levpos.x += sx;
			d += ay;

			if (level->Is_Passable(levpos)==false)
				return false;

			if (!out)
				outchar=decidelinechar(old.x, old.y, levpos.x, levpos.y);
		}
	}
	else /* y dominant */
	{
		d = ax-(ay>>1);
		for (;;)
		{
			if (gameview.Is_Visible(levpos))
			{
				vistat=true;

				old=gameview.Get_Screen_Location(levpos);

				gotoxy(old.x, old.y);
				addch(outchar);
				refresh();

				if (output_delay>0)
					delay(output_delay);
				if (single)
					gameview.Show();
			}
			else
				vistat=false;

			if (hitfunc!=NULL)
			{
				if (hitfunc(level, ranger, levpos.x, levpos.y, vistat, skill))
					return true;
			}

			if (levpos.y==y2) return false;

			old=levpos;

			if (d>=0)
			{
				levpos.x += sx;
				d -= ay;
			}
			levpos.y += sy;
			d += ax;

			if (level->Is_Passable(levpos)==false)
				return false;

			if (!out)
				outchar=decidelinechar(old.x, old.y, levpos.x, levpos.y);
		}
	}
}
