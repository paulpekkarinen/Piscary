/***************************************************************/
/* SCROLLS                                                     */
/***************************************************************/
/* 
 * (C)1997/1998 Erno Tuomainen
 * Made for Legend of Saladir
 */

//Unit scrolls: Scroll data.

#ifndef SCROLLS_H
#define SCROLLS_H

#include <string>
#include "items.h"

/* scroll groups */
#define SCROLLGROUP_GENERIC 0
#define SCROLLGROUP_MAGIC   1

/* scroll flags */
#define SCFLAG_AUTOIDENTIFY 0x0001 /* autoidentified scroll (by reading) */
#define SCFLAG_IDENTIFIED   0x0002 /* scroll is identified */
#define SCFLAG_NAMED        0x0004 /* scroll is named */
#define SCFLAG_TRIED        0x0008 

/* general group scroll effects */

/* scrolls */
#define SCROLL_SELFWRITTEN    0
#define SCROLL_BLANK    1
#define SCROLL_RUMOUR   2
#define SCROLL_HEALING  3
#define SCROLL_TELEPORT 4 
#define SCROLL_CURSE    5
#define SCROLL_UNCURSE  6
#define SCROLL_BLESS    7
#define SCROLL_CONFUZE  8
#define SCROLL_IDENTIFY 9
#define SCROLL_AMNESIA  10
#define SCROLL_MAGICMAP 11

struct Scroll
{
	/* name of the scroll when identified */
	const char *name;

	/* unidentified name */
	char uname[ITEM_NAMEMAX];

	/* call name */
	char cname[ITEM_NAMEMAX];

	/* scroll group */
	int group;

	/* what spell is contained in the scroll */
	int spell;

	/* recorded skill value, if it's a spell scroll */
	int skill;

	/* scroll flags */
	int32u flags;
};

/* scroll list */
extern Scroll list_scroll[];

void scroll_amnesia(level_type *level);
void scroll_blank();
void scroll_rumour();

#endif
