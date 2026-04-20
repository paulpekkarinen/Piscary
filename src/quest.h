//Legend of Saladir - quest.h

//Unit quest: Quest data.

#ifndef QUEST_H
#define QUEST_H

#include <string>
#include "types.h"

/* quest flags */
#define QFLAG_GENERATED  0x0001
#define QFLAG_SOLVED     0x0002
#define QFLAG_UNSOLVABLE 0x0004 
#define QFLAG_REWARDED   0x0008 /* quest has been rewarded */
#define QFLAG_DONOTASK   0x8000 /* add immendiately without asking */

/* quest solve conditions */
#define QNEED_ITEM      1  /* return an item to the giver */
#define QNEED_KILLNPC   2  /* kill a monster of certain race */
#define QNEED_MONSTER   3  /* kill some monster, not NPC */
#define QNEED_MEETNPC   4  /* must meet someone */

/* quest result codes */
#define QSTAT_OK        0
#define QSTAT_PREQUEST  1
#define QSTAT_ALREADYIN 2
#define QSTAT_SOLVED    3
#define QSTAT_ERROR    10

/* quests */
#define QUEST_THOMASBISKUP     0
#define QUEST_KILLGATES        1
#define QUEST_MEETSPARHAWK     2
#define QUEST_TOTALNUM         3

struct Questdata
{
	const char *name;      /* quest description */
	const char *desc;      /* quest talk description */
	const char *solvetxt;  /* shown after quest has been solved */
	int16u flags;
	int owner;    /* quest owner, who shall issue this quest */
	int qnum;     /* if npc has multiple quests, this is the order num*/
	int prequest; /* if quest is required to get this quest */
	int solve;    /* quest solve condition */
	int cond1;    /* for items this is group
			for monsters this is a race */
	int cond2;    /* for items this is item id,
			for monsters this is NPC number */
	int rewardimm;  /* */
	int exp;      /* experience gain for completing the quest */
};

extern Questdata quest_list[];

struct Quest
{
	int id;      /* quest id */
	int32u monid;   /* who gave the quest, id number */
	int dungeon; /* dungeon where the quest was given */
	int level;   /* level of that dungeon */
	int16u flags;   /* conditions concering the quest */
	std::string giver; /* name of the monster who gave this quest */

	Quest();
	Quest(int i, int mid, int dng, int lvl);

	bool Check_Giver_Death(const int32u mid);
	bool Is_Unfinished(const int32u mid);

	int Print_Solved(int index);
	int Print_Unfinished(int index);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

int quest_getflags(int quest);
void quest_showdesc(int);
const char *quest_showname(int);

#endif
