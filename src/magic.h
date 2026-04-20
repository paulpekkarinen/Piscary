//Legend of Saladir - magic.h

//Unit magic: Spell data and casting.

#ifndef MAGIC_H
#define MAGIC_H

#include "types.h"

struct playerinfo;

/* spells */
#define SPELL_HEALING   0
#define SPELL_TELEPORT  1
#define SPELL_IDENTIFY  2
#define SPELL_BLESS     3
#define SPELL_CURSE     4
#define SPELL_UNCURSE   5
#define SPELL_CONFUZE   6
#define SPELL_MAGICMAP  7

/* spell flags */
#define SPF_ALTERATION  0x00000001
#define SPF_DESTRUCTION 0x00000002
#define SPF_OBSERVATION 0x00000004
#define SPF_MYSTICISM   0x00000008

/* spell type */
struct Spell
{
	/* name of the spell */
	const char *name;

	int32u flags;

	/* spellpoints required */
	int sp;

	/* spell skill levels required for certain target */
	/* negative means it's not possible */
	int self;
	int other;
	int levitem;
	int invitem;
	int direction;
};

bool spell_zap(
	playerinfo *plr, level_type *level, int spell, int skill, bool scroll);

extern Spell list_spells[];

#endif
