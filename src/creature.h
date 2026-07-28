//creature.h

//Unit creature: Static data for creatures.

#ifndef CREATURE_H
#define CREATURE_H

#include "ability.h"
#include "hitpoint.h"

#define CNAME_MAX  20

#define RACEGEN_SPECIAL	0x00000001 /* special race, not for random monsters */

/* define bits for monster status flags */
#define MST_PURSUEITEM	0x00000001
#define MST_CANTMOVE	0x00000002 // it's unable to move for some reason
#define MST_FLEEMODE    0x00000004 // monster is fleeing for its life
#define MST_ATTACKMODE  0x00000008 // this bit on monster is in attackmode
#define MST_KEEPERHATES	0x01000000 // monster hated by shopkeeper
#define MST_INSIDESHOP	0x02000000 // (note: unused, now as 'last_room')
#define MST_HATEPLAYER	0x04000000 // for shopkeepers, hates player will attack
#define MST_GUARDDOOR   0x08000000
#define MST_SHOPKEEPER  0x10000000 // it is a shopkeeper
#define MST_KNOWN       0x20000000 // bit set if creature unknown (Chat?)
#define MST_ALIVE       0x40000000 // monster is alive
#define MST_GENERATED	0x80000000 // this is set if monster already generated once
                                   // for special NPC's mainly

#define BEHV_FRIENDLY	0x00000001  // monster is generally friendly
#define BEHV_GETALL 	0x00000002  // get EVERYTHING comes in hand
#define BEHV_SENSEBEST	0x00000004  // sense best item and target it
#define BEHV_CANUSEITEM	0x00000008  // if set with animal, can carry one item
#define BEHV_ANIMAL		0x00000010  // animal monster, no class, no inventory
#define BEHV_FLYING		0x00000020  // flying creature, has hands but
#define BEHV_SWIMMING   0x00000040  /* swimming creature */
                                    // they are called WINGS
/* attack flags */
#define ATTACK_BITE		0x00000001  // does bite attacks
#define ATTACK_HIT		0x00000002  // hits with hands
#define ATTACK_KICK     0x00000004  // mainly kicks (horse?)

#define ANGRY_NORMAL		0
#define ANGRY_TO_MONSTER 	-5
#define ANGRY_TO_PLAYER		-10

/* targetting flags for monsters
   monster can attack specific bodypart when
   a flag has been set in the race description
   structure */
#define TARGET_HEAD	0x01
#define TARGET_BODY	0x02
#define TARGET_LHAND	0x04
#define TARGET_RHAND	0x08
#define TARGET_LEGS	0x10

/*
** npc race definition
*/
struct Npcrace
{
	const char *name;	// name of this race
	const char *desc;	// race description (if any)
	char out;			// char to output
	int color;			// color of output

	int32u attacktypes;	// which kind of attack does the race do
	int32u behave;		// behaviour & status flags
	int weight;			// 1000 is 1kg
	int32u status;		// status flags (is he alive, confuzed etc)

	int align;			// alignment of the monster
	int hp_plev_dt;		// hp gain (dice count) per level
	int hp_plev_ds;		// hp gain (dice sides) per level
	int sp_plev_dt;
	int sp_plev_ds;
	int hp_base;		// starting hp (level 1)
	int sp_base;		// starting sp	(level 1)
	int ac;      		// armor class
	int attitude;		// general attitude (pissing factor :-)
	int exp;

	int dam_dt;			// hand damage dice
	int dam_ds;			// hand damage dice
	int dam_mod;

	int8u targetflags;	// where this monster can target (legs, head etc.)

	// bodyparts, -1 doesn't exist
	// 0 or positive is the hit probability for player
	int bodyparts[HPSLOT_MAX];

	int32u GENFLAGS;	// generation flags

	resistpack res; // race resistances
	Ability stats; // class starting values
};

/* public tables and variables */
extern const int eqslot_from_hpslot[];
extern char *desclist[];
extern Npcrace npc_races[];

#endif
