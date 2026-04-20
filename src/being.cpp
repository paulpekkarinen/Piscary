/**************************************************************************
 * being.cpp --                                                           *
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

//Refactored 13.6.2022 - 19.10.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "condit.h"
#include "damage.h"
#include "dice.h"
#include "game.h"
#include "gameview.h"
#include "invnode.h"
#include "lexicon.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "ranged.h"
#include "roleplay.h"
#include "salamath.h"
#include "specmon.h"
#include "storage.h"
#include "tactics.h"

using std::string;

being::being()
	: base_hp(0), target(0),
	rev_x1(0), rev_x2(0), rev_y1(0), rev_y2(0), roomnum(-1), sindex(-1)
{
	Reset();

	/* set monster id number */
	id=Game.get_new_monster_id();
}

being::~being()
{
	inv.Clear();
	path.clear();
}

bool being::Is_Spotting() const
{
	if (spot.x>0 && spot.y>0) return true;
	return false;
}

void being::Checkbody()
{
	/* check for hpslot healing */
	/* and hp warning */
	for (int i=0; i<HPSLOT_MAX; i++)
	{
		/* warn for low hitpoints */
		const int eqslot=eqslot_from_hpslot[i];

		if ((hpp[i].cur > 0)
			&& (equips.is_usable(eqslot)==false))
		{
			/* put the slot back to work */
			equips.make_usable(eqslot);

			if (i==HPSLOT_LEGS)
				conditions.remove(CONDIT_BADLEGS);
			else if (i==HPSLOT_LEFTHAND)
				conditions.remove(CONDIT_BADLARM);
			else if (i==HPSLOT_RIGHTHAND)
				conditions.remove(CONDIT_BADRARM);

			/* note: Should we message the monster somehow now that he's
			   able to use certain bodyparts */
		}
	}

	/* let flying creatures fly when both hands(vanes) have hp's */
	if (npc_races[m.race].behave & BEHV_FLYING)
	{
		if ((hpp[HPSLOT_LEFTHAND].cur>0) && (hpp[HPSLOT_RIGHTHAND].cur>0))
		{
			if ((m.status & MST_CANTMOVE))
				m.status ^= MST_CANTMOVE;
		}
	}
	else
	{
		if ((hpp[HPSLOT_LEGS].cur>0) && (m.status & MST_CANTMOVE))
			m.status ^= MST_CANTMOVE;
	}
}

void being::Checkstat(level_type *level)
{
	//don't kill more than once
	if (Is_Alive()==false)
		return;

	Calculate_Totalhp();

	string monsname=monster_sprintf(this, true, true);

	bool getoutflee=false;

	real rhp=(real)hpp[HPSLOT_HEAD].max;
	rhp=rhp * 0.60;

	/* to get out of flee mode, both head and body must be healthy enough */
	if (hpp[HPSLOT_HEAD].cur > rhp)
	{
		rhp=(real)hpp[HPSLOT_BODY].max;
		rhp=rhp * 0.50;

		if (hpp[HPSLOT_BODY].cur > rhp)
			getoutflee=true;
	}

	/* get out of the flee mode */
	/* should probably watch head and body damage instead */
	if (getoutflee && (m.status & MST_FLEEMODE))
	{
		m.status^=MST_FLEEMODE;

		string s(monsname);
		s.append(" looks healthier.");
		msg.add_dist(level, x, y, s.c_str(), C_YELLOW, NULL, C_CYAN);
	}

	/* check for healing bodyparts and activate them */
	Checkbody();

	/* check for damaging bodyparts and disable them if needed */
	damage_checkbodyparts(level, this);

	/* here dies the monster */
	if (Is_Alive()==false)
	{
		string s;

		if (gameview.Is_Visible(x, y))
			s=monster_sprintf(this, true, false);
		else
			s="Something";

		s.append(" dies!");

		msg.add_dist(level, x, y, s.c_str(), C_RED,
			"Someone died!", C_RED);

		player.quests.ownerdied(this);

		level->crew.Sentenced(); //notify clean up routine
	}
}

void being::Checkturn(level_type *level)
{
	timetaken+=stat[STAT_SPD].Get();

	/* act if time passes over BASE_TIMENEED */
	if (timetaken>=BASE_TIMENEED)
	{
		Noticestuff();

		/* here do special monsters acting, they are good actors :-) */
		if (Get_Special_Id()!=0)
			act_specialmonster(this, level);

		int mtime=monster_ranged_attack(this, level);

		if (!mtime)
		{
			if (m.status & MST_SHOPKEEPER)
				mtime=shopkeeper_move(level, this);
			else
				mtime=move_monster(this, level);
		}

		timetaken-=mtime;
		movecount++;
	}
}

void being::Damage_Message(int damage, int bodypart)
{
	if(bodypart<0 || bodypart>=HPSLOT_MAX)
	{
		if (gameview.Is_Visible(x, y))
		{
			string s(gender_art3[m.gender]);
			s.append(" whole body seems to be damaged.");

			s[0]=toupper(s[0]);
			msg.newmsg(s, C_RED);
		}

		return;
	}

	real rhp=hpp[bodypart].cur;
	const char **bpptr;
	int gindex;

	if(npc_races[m.race].behave & BEHV_ANIMAL)
		gindex=SEX_NEUTRAL;
	else
		gindex=m.gender;

	if(npc_races[m.race].behave & BEHV_FLYING)
		bpptr=bodyparts_flying;
	else
		bpptr=bodyparts;

	string s;
	append_string_with(s, gender_art3[gindex], ' ');
	append_string_with(s, bpptr[bodypart], ' ');
	append_string_with(s, bodypart_art[bodypart], ' ');

	if(damage>rhp)
		s.append("very badly injured!");
	else if(damage > (rhp*0.8))
		s.append("severely injured!");
	else if(damage > (rhp*0.5))
		s.append("moderately injured!");
	else if(damage > (rhp*0.2))
		s.append("slightly injured!");
	else if(damage > (rhp*0.1))
		s.append("minimally damaged!");
	else
		s.append("not damaged!");

	s[0]=toupper(s[0]);
	msg.add_dist(c_level, x, y, s.c_str(), C_RED, NULL, C_RED);
}

void being::Gain_Experience(int gain)
{
	exp+=gain;
	roleplay.Check_Levelraise(c_level, this, false);

	target=0;

	/* clear attack and flee modes */
	if ((m.status & MST_ATTACKMODE))
		m.status^=MST_ATTACKMODE;
	if ((m.status & MST_FLEEMODE))
		m.status^=MST_FLEEMODE;
}

void being::Getangry(level_type *level, Actor *kohde)
{
	/* target must NOT be the monster who is getting angry :) */
	if (this == kohde)
	{
		msg.addwait("Error: Getangry() trying to make monster angry with itself!", CH_RED);
		return;
	}

	const bool is_plr=kohde->Is_Player();

	/* shopkeepers will start hating player */
	if (m.status & MST_SHOPKEEPER)
	{
		if (is_plr)
			m.status |= MST_HATEPLAYER;
		else
			m.status |= MST_KEEPERHATES;
	}

	if (!(m.status & MST_ATTACKMODE) || target!=kohde)
	{
		string moname=monster_sprintf(this, true, true);

		if (is_plr==false)
		{
			string moname2=monster_sprintf(kohde, false, true);
			if (gameview.Is_Visible(kohde->x, kohde->y) &&
				gameview.Is_Visible(x, y))
			{
				msg.vnewmsg(C_RED, "%s gets angry with %s", moname.c_str(), moname2.c_str());
			}
			else
				msg.newmsg("You hear a distant cry.", C_WHITE);
		}
		else
			msg.vnewmsg(C_RED, "%s gets angry with you!", moname.c_str());
	}

	/* make it angry */
	m.status|=MST_ATTACKMODE;
	target=kohde;

	/* for now, use TACTIC_AGGR */
	tactic=TACTIC_AGGR;

	/* forget other special modes */
	Set_Target_Spot(0, 0);
	if (m.status & MST_PURSUEITEM)
		m.status ^= MST_PURSUEITEM;

	/* when attacked, the monster WILL forget it's path, so if the monster
	 * being attacked is a shopkeeper and it's outside its shop then it must
	 * somehow get back to the shop..
	 * the path must be calculated with path_findroute() ...
	 * (for now, shopeeker will recalculate a way back to the shop
	 * when the distance between the shopeeper and his shop (door) is
	 * over SHOPKEEPER_MAXDOORDIST
	 *
	 */
	path.clear();
}

bool being::Handle_Confusion(Condition *cond, int slots)
{
	if (sometimes() && gameview.Is_Visible(x, y))
	{
		string s=monster_sprintf(this, true, true);
		msg.vnewmsg(C_WHITE, "%s looks strangely distorted.", s.c_str());
	}

	cond->val-=slots;

	if (cond->val <= 0)
	{
		string s=monster_sprintf(this, true, true);
		msg.vnewmsg(C_WHITE, "%s looks much more stable now.", s.c_str());

		return true;
	}

	return false;
}

void being::Move_To(int dx, int dy)
{
	Coord oc=Get_Location();
	gameview.Put_Monster(0, oc); //clear old location

	Set_Location(dx, dy);

	Coord nc=Get_Location();
	gameview.Put_Monster(this, nc);
}

bool being::Is_Peaceful()
{
	/* neutral characters don't attack anyone spontaneously */
	if (m.align>=NEUTRAL_S && m.align<=NEUTRAL_E)
		return true;

	/* friendly monsters doesn't attack spontaneously */
	if (m.behave & BEHV_FRIENDLY)
		return true;

	return false;	
}

bool being::Gets_Angry_To(being *other)
{
	if (Is_Peaceful())
		return false;

	/* at this point, special monsters do not attack each other */
	if ((m.special!=0) && (other->m.special!=0))
		return true;

	/* if attacker has same race as target */
	if (m.race == other->m.race)
		return true;

	int align=ABS(m.align - other->m.align) / (LAWFUL/100);
	align+=m.attitude;

	if (RANDU(100) < align)
		return true;

	return false;	
}

bool being::Gets_Angry_To_Player()
{
	if (Is_Peaceful())
		return false;

	/* at this point, special monsters do not get angry spontaneously */
	if (Get_Special_Id()!=0)
		return false;

	int align=ABS(m.align - player.m.align) / (LAWFUL/100);
	align+=m.attitude;

	/* in here I must check the conditions for getting angry to player*/
	if (RANDU(100) < align)
		return true;

	return false;
}

bool being::Noticestuff()
{
	/* don't change attack parameters if monster is
	   already in attack mode chasing something */
	if (m.status & MST_ATTACKMODE)
		return false;

	bool note_items;

	//notice items on level
	if (Is_Spotting()==false &&
		!(npc_races[m.race].behave & BEHV_ANIMAL) &&
		!(m.status & MST_SHOPKEEPER))
	{
		note_items=true;
	}
	else
		note_items=false;

	return gameview.Notice_Something(this, note_items);
}

bool being::Pick_Up_Item(level_type *level, invnode *itemptr)
{
	if (!itemptr)
		return false;

	/* animals doesn't take items */
	if (npc_races[m.race].behave & BEHV_ANIMAL)
		return false;

	/* and monsters don't take shop items (unpaid!) */
	if ((itemptr->i.status & ITEM_UNPAID))
		return false;

	/* copy the item and other info */
	inv.Add_Item(level->inv.remove_n_items(itemptr, itemptr->count));

	/* print a short message about the action */
	if (gameview.Is_Visible(x, y))
	{
		string s=monster_sprintf(this, true, true);
		s.append(" just took something.");
		msg.add_dist(level, x, y,
			s.c_str(), C_WHITE, 0, C_CYAN);
	}

	/* after getting an item, monster should decide what to do what it */
	/* scan all items with this function */
	Useitems(level);

	return true;
}

void being::Regenerate(level_type *level, int ctime, int slots)
{
	if (Regenerate_Health(slots, ctime))
		Checkstat(level);

	/* monster needs atleast half of it's max health to get out from fleemode */
}

void being::Set_Target_Spot(int dx, int dy)
{
	spot.Set(dx, dy);
}

void being::Shouldflee(level_type *level)
{
	bool shouldflee=false;

	real rhp=(real)hpp[HPSLOT_HEAD].max;
	rhp=rhp*0.60;

	/* flee if head too weak */
	if (hpp[HPSLOT_HEAD].cur <= rhp)
		shouldflee=true;

	rhp=(real)hpp[HPSLOT_BODY].max;
	rhp=rhp*0.40;

	/* flee if body too weak */
	if (hpp[HPSLOT_BODY].cur <= rhp)
		shouldflee=true;

	if (shouldflee)
	{
		if (!(m.status & MST_FLEEMODE))
		{
			string moname;
			if (gameview.Is_Visible(x, y))
				moname=monster_sprintf(this, true, true);
			else
				moname="It";

			tactic=TACTIC_VERYDEF;

			m.status|=MST_FLEEMODE;
			moname.append(" flees.");
			msg.add_dist(level, x, y, moname.c_str(), C_YELLOW, NULL, C_CYAN);
		}
	}
}

bool being::Useitems(level_type *level)
{
	invnode *bow=inv.Find_Best_Item(IS_MISWEAPON, -1);

	/* first, find a best melee weapon */
	invnode *usenode = inv.Find_Best_Item(IS_WEAPON2H, -1);

	if (!usenode)
		usenode = inv.Find_Best_Item(IS_WEAPON1H, -1);

	bool mistat=false;

	/* for now, give priority for ranged weapons */
	if (bow)
	{
		if (bow->slot < 0)
		{
			/* ready missiles for the bow */
			if (bow->i.group == WPN_BOW)
				mistat=inv.ready_newmissile(equips,
					IS_MISSILE, WPN_MISSILE_1);
			else if (bow->i.group == WPN_CROSSBOW)
				mistat=inv.ready_newmissile(equips,
					IS_MISSILE, WPN_MISSILE_2);

			/* equip bow if there're missiles also */
			if (mistat)
				usenode=bow;
		}
	}

	if (!usenode)
		return false;

	/* is the item already used */
	if (usenode->slot >= 0)
		return true;

	equips.monster_equip(level, this, usenode);
	roleplay.Calculate_Itembonus(this);

	return true;
}

void being::Save(Tar_Ball &tb)
{
	Actor::Save(tb);

	tb.Put(id);
	tb.Put(base_hp);

	spot.Save(tb);

	tb.Put(rev_x1);
	tb.Put(rev_x2);
	tb.Put(rev_y1);
	tb.Put(rev_y2);
	tb.Put(roomnum);
	tb.Put(sindex);
}

void being::Load(Tar_Ball &tb, level_type *lvl)
{
	Actor::Load(tb);

	id=tb.Get_Next_Unsigned();
	base_hp=tb.Get_Next_Value();

	spot.Load(tb);

	//check and restore target (note: if targetting an item, this kind of fails)
	if (spot.x>0 && spot.y>0)
		target=lvl->crew.Find_Monster_At(spot);
	else
		target=0;

	rev_x1=tb.Get_Next_Value();
	rev_x2=tb.Get_Next_Value();
	rev_y1=tb.Get_Next_Value();
	rev_y2=tb.Get_Next_Value();

	//restore room owner id if monster has one
	roomnum=tb.Get_Next_Value();
	if (roomnum!=-1)
		lvl->set_room_owner(roomnum, this);

	sindex=tb.Get_Next_Value();
}
