/**************************************************************************
 * magic.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the winter of 1998                 *
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

//Refactored 26.9.2021 - 24.6.2025 Paul K. Pekkarinen

#include <cctype>
#include "avatar.h"
#include "being.h"
#include "caves.h"
#include "condit.h"
#include "display.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "magic.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "randgen.h"
#include "selpack.h"
#include "scrolls.h"
#include "target.h"

using std::string;

bool spell_teleport(level_type *level, being *caster,
		    Target *target, int skill);
bool spell_curse(level_type *level, being *caster,
		 Target *target, int skill);
bool spell_uncurse(level_type *level, being *caster,
		 Target *target, int skill);
bool spell_bless(level_type *level, being *caster,
		 Target *target, int skill);
bool spell_confuze(level_type *level, Target *target, int skill);
bool spell_identify(level_type *level, Target *target, int skill);
bool spell_magicmap(level_type *level, being *caster, int skill);

/*******************************************************/
/*******************************************************/
/* SPELLS HERE                                         */
/*******************************************************/
/*******************************************************/

Spell list_spells[]=
{
   { "heal", SPF_ALTERATION,
     4, 1, -1, -1, -1, -1
   },
   { "teleport", SPF_MYSTICISM,
     4, 1, 1, 1, -1, 1
   },
   { "identify", SPF_OBSERVATION,
     4, -1, -1, -1, 1, -1
   },
   { "bless", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "curse", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "remove curse", SPF_MYSTICISM,
     4, -1, -1, -1, 1, -1
   },
   { "confuze", SPF_MYSTICISM,
     4, 1, 1, -1, -1, 1
   },
   { "magic map", SPF_OBSERVATION,
     4, -1, -1, -1, -1, -1
   },

   { NULL, 0 }
};

/* players spell casting routine */
bool spell_zap(
	playerinfo *plr, level_type *level, int spell, int skill, bool scroll)
{
	Target t;
	bool noneedtarget=false;
	string chstr;

	if ((list_spells[spell].self < 0) &&
		(list_spells[spell].other < 0) &&
		(list_spells[spell].levitem < 0) &&
		(list_spells[spell].direction < 0) &&
		(list_spells[spell].invitem < 0))
		noneedtarget=true;

	if (!noneedtarget)
	{
		string s("Which type of target,");
		chstr[0]=0;
		if ((skill > list_spells[spell].self && list_spells[spell].self > 0) ||
			(skill > list_spells[spell].other && list_spells[spell].other > 0) ||
			(skill > list_spells[spell].levitem && list_spells[spell].levitem > 0))
		{
			s.append(" [T]arget");
			chstr.append("t");
		}
		if (skill > list_spells[spell].direction &&
			list_spells[spell].direction > 0)
		{
			s.append(" [D]ir");
			chstr.append("d");
		}
		if (skill > list_spells[spell].invitem &&
			list_spells[spell].invitem > 0)
		{
			s.append(" [I]nv");
			chstr.append("i");
		}

		const int chlen=(int)chstr.size();

		if (chlen>1)
		{
			s.append("? ");

			char data;
			bool DONE=false;
			while (!DONE)
			{
				msg.add(s.c_str(), C_WHITE);

				data=(char)my_getch();
				data=tolower(data);

				for (int i=0; i<chlen; i++)
				{
					if (data == chstr[i])
					{
						chstr[0]=(char)data;
						DONE=true;
					}
				}

				if (!DONE)
				{
					msg.update();
					msg.add("Not a valid choice!", C_RED);
				}
			}
		}

		if (chstr[0]=='d')
		{
			t.direction = dir_askdir("Cast into", true);

			if (t.direction > 9)
				t.direction=0;
			else
			{
				t.pos=move_to_direction(t.direction, plr);
			}
		}
		else if (chstr[0]=='i')
		{
			plr->backpack->Set_Filter(-1); //all items
			t.invitem=plr->backpack->Get_Handle("Target to which item?");
		}
		else
		{
			if ((skill > list_spells[spell].self && list_spells[spell].self > 0) &&
				(skill < list_spells[spell].other || list_spells[spell].other < 0) &&
				(skill<list_spells[spell].levitem || list_spells[spell].levitem<0))
			{
				t.pos=plr->Get_Location();
			}
			else
				player_gettarget(level, &t, &list_spells[spell]);
		}


		if (t.invitem==0 && (!t.pos.x && !t.pos.y) && !t.direction)
		{
			msg.newmsg("No target selected.", C_YELLOW);
			return false;
		}
	}

	if (!scroll)
	{
		if ((plr->mana.value < list_spells[spell].sp))
		{
			msg.newmsg("You don't have enough mana.", C_RED);
			return false;
		}

		/* subsctract the sp amount needed */
		plr->mana.value -= list_spells[spell].sp;
		GAME_NOTIFYFLAGS|=GAME_HPSPCHG;

	}

	bool rescode;

	switch (spell)
	{
		case SPELL_TELEPORT:
			rescode=spell_teleport(level, NULL, &t, skill);
			break;
		case SPELL_CURSE:
			rescode=spell_curse(level, NULL, &t, skill);
			break;
		case SPELL_UNCURSE:
			rescode=spell_uncurse(level, NULL, &t, skill);
			break;
		case SPELL_BLESS:
			rescode=spell_bless(level, NULL, &t, skill);
			break;
		case SPELL_CONFUZE:
			rescode=spell_confuze(level, &t, skill);
			break;
		case SPELL_IDENTIFY:
			rescode=spell_identify(level, &t, skill);
			break;
		case SPELL_MAGICMAP:
			rescode=spell_magicmap(level, NULL, skill);
			break;
		default:
			rescode=false;
			msg.newmsg(CH_RED, "Error! Trying to zap an unknown spell %d.", spell);
			break;
	}

	if (rescode==false)
		msg.newmsg("Spell failed!", C_RED);

	return true;
}

//Curse, will turn item immendiately cursed!
bool spell_curse(level_type *level, being *caster,
		 Target *target, int skill)
{
	/* curse the item */
	if (target->invitem)
	{
		if (target->invitem->i.status & ITEM_BLESSED)
			target->invitem->i.status ^= ITEM_BLESSED;

		target->invitem->i.status |= ITEM_CURSED;

		if (caster)
		{
			string s=monster_sprintf(caster, true, true);
			s.append("'s backpack seems to shake violently.");
			msg.add_dist(level, caster->x, caster->y, s.c_str(), CH_DGRAY,
				NULL, C_CYAN);
		}
		else
		{
			string s("Your ");
			s.append(target->invitem->i.name);

			if (target->invitem->count > 1)
				s.append("s ");
			else
				s.append(" ");

			s.append("vibrates violently for a moment.");

			msg.newmsg(s.c_str(), CH_DGRAY);
		}
	}

	return true;
}

//Uncurse, which actually is "Neutralise", removes cursed or blessed status
bool spell_uncurse(level_type *level, being *caster,
		 Target *target, int skill)
{
	/* uncurse/unbless the item */
	if (target->invitem)
	{
		if (target->invitem->i.status & ITEM_CURSED)
			target->invitem->i.status ^= ITEM_CURSED;
		else if (target->invitem->i.status & ITEM_BLESSED)
			target->invitem->i.status ^= ITEM_BLESSED;

		if (caster)
		{
			string s=monster_sprintf(caster, true, true);
			s.append("'s backpack glows light blue for a moment.");
			msg.add_dist(level, caster->x, caster->y, s.c_str(), CH_BLUE,
				NULL, C_CYAN);
		}
		else
		{
			string s("Your ");
			s.append(target->invitem->i.name);

			if (target->invitem->count > 1)
				s.append("s ");
			else
				s.append(" ");

			s.append("glows bright light blue for a moment.");

			msg.newmsg(s.c_str(), CH_BLUE);
		}
	}

	return true;
}

//Bless, will turn item towards blessed status
bool spell_bless(level_type *level, being *caster,
		 Target *target, int skill)
{
	/* bless the item, if cursed first remove cursing. */
	if (target->invitem)
	{
		if (target->invitem->i.status & ITEM_CURSED)
		{
			target->invitem->i.status ^= ITEM_CURSED;
		}
		else
			target->invitem->i.status |= ITEM_BLESSED;

		if (caster)
		{
			string s=monster_sprintf(caster, true, true);
			s.append("'s backpack glows bright white for a moment.");

			msg.add_dist(level, caster->x, caster->y, s.c_str(), CH_WHITE,
				NULL, C_CYAN);
		}
		else
		{
			string s("Your ");
			s.append(target->invitem->i.name);

			if (target->invitem->count > 1)
				s.append("s ");
			else
				s.append(" ");

			s.append("glows bright white for a moment.");

			msg.newmsg(s.c_str(), CH_WHITE);
		}
	}

	return true;
}

//Teleport spell
bool spell_teleport(level_type *level, being *caster,
	Target *target, int skill)
{
	bool rv=true;

	if (player.Is_At(target->pos))
		teleport_player(level, true, false);
	else
	{
		being *mptr=gameview.Get_Monster(target->pos);
		if (mptr)
			rv=teleport_monster(level, mptr);
		else
			rv=level->inv.teleport_items(level, target);
	}

	return rv;
}

bool spell_confuze(level_type *level, Target *target, int skill)
{
	being *mptr=gameview.Get_Monster(target->pos);

	if (target->pos == player.Get_Location())
	{
		msg.newmsg("The world spins around you.", C_CYAN);
		player.conditions.add(CONDIT_CONFUSED, skill * 5);
	}
	else if (mptr)
	{
		string s=monster_sprintf(mptr, true, true);
		s.append(" looks quite confuzed.");

		msg.add_dist(level, mptr->x, mptr->y, s.c_str(), C_CYAN,
			NULL, C_CYAN);

		mptr->conditions.add(CONDIT_CONFUSED, skill * 5);
	}
	else
		return false;

	return true;
}

bool spell_identify(level_type *level, Target *target, int skill)
{
	if (target->invitem)
	{
		if (target->invitem->i.status & ITEM_IDENTIFIED)
			return false;

		/* identify scroll */
		if (target->invitem->i.type == IS_SCROLL)
		{
			list_scroll[target->invitem->i.group].flags |= SCFLAG_IDENTIFIED;
		}

		target->invitem->i.status |= ITEM_IDENTIFIED;

		display->Item_Info(&target->invitem->i, target->invitem->i.weight,
			target->invitem->count, "This item is actually");
	}
	else
		return false;

	return true;
}

bool spell_magicmap(level_type *level, being *caster, int skill)
{
	if (caster)
		return false;

	//note: test this, might not work as expected
	for (int y=1; y<level->sizey-1; y++)
	{
		for (int x=1; x<level->sizex-1; x++)
		{
			if (sur_countall(level, x, y) > 0)
				//&& !(level->loc[y][x].flags & CAVE_NOLIT))
				level->loc[y][x].flags|=CAVE_EXPLORED;
		}
	}

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
	msg.newmsg("Suddenly you can remember the locations in this level.",
		C_WHITE);

	return true;
}

/*
 * Heal self spell (note: commented out)
 *
 */
/*
bool spell_heal_self_minor(level_type *level, being *caster,
			   int16u skill)
{
   if(caster) {

      return true;
   }



   return true;
}
*/
