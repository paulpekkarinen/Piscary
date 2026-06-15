/*
 * Scrolls.cpp for the Legend of Saladir
 * (C) 1997/1998 Erno Tuomainen
 *
 * This contains scroll specific routines
 *
 */

//Refactored 15.7.2022 - 1.4.2026 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include "caves.h"
#include "dice.h"
#include "game.h"
#include "magic.h"
#include "message.h"
#include "script.h"
#include "scrolls.h"

Scroll list_scroll[]=
{
	{"my own", "", "", SCROLLGROUP_GENERIC, SCROLL_SELFWRITTEN,
	100, SCFLAG_IDENTIFIED},
	{"blank paper", "", "", SCROLLGROUP_GENERIC, SCROLL_BLANK,
	100, 0},
	{"rumour", "", "", SCROLLGROUP_GENERIC, SCROLL_RUMOUR,
	100, SCFLAG_AUTOIDENTIFY},
	{"healing", "", "", SCROLLGROUP_MAGIC, SPELL_HEALING,
	40, 0},
	{"teleporting", "", "", SCROLLGROUP_MAGIC, SPELL_TELEPORT,
	40, 0},
	{"cursing", "", "", SCROLLGROUP_MAGIC, SPELL_CURSE,
	40, 0},
	{"uncursing", "", "", SCROLLGROUP_MAGIC, SPELL_UNCURSE,
	40, 0},
	{"blessing", "", "", SCROLLGROUP_MAGIC, SPELL_BLESS,
	40, 0},
	{"confuzion", "", "", SCROLLGROUP_MAGIC, SPELL_CONFUZE,
	40, 0},
	{"identify", "", "", SCROLLGROUP_MAGIC, SPELL_IDENTIFY,
	40, SCFLAG_AUTOIDENTIFY},
	{"amnesia", "", "", SCROLLGROUP_GENERIC, SCROLL_AMNESIA,
	100, SCFLAG_AUTOIDENTIFY},
	{"magic mapping", "", "", SCROLLGROUP_MAGIC, SPELL_MAGICMAP,
	40, SCFLAG_AUTOIDENTIFY},

	{NULL, "", "", 0, 0, 0, 0},
};

void scroll_rumour()
{
	msg.newmsg("The scroll contains a message:", C_WHITE);
	texts->Random_Message(Script::Rumours, C_GREEN);
}

void scroll_blank()
{
	texts->Random_Message(Script::Empty_Scroll);
}

void scroll_amnesia(level_type *level)
{
	//note: test scroll of amnesia

	msg.newmsg("What? Who? Where? ... There's a hole in your mind!", C_WHITE);

	Coord c;
	for (c.y=0; c.y<level->sizey; c.y++)
		for (c.x=0; c.x<level->sizey; c.x++)
			level->Clear_Flag(c, CAVE_EXPLORED);

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;
}
