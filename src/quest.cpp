/**************************************************************************
 * quest.cpp --                                                           *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 12.05.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 12.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 25.9.2021 - 1.8.2023 Paul K. Pekkarinen

#include "being.h"
#include "message.h"
#include "output.h"
#include "quest.h"
#include "scrolls.h"
#include "specmon.h"
#include "storage.h"
#include "world.h"

/* list of special quests */
Questdata quest_list[]=
{
   { "Return Thomas Biskup to his own realm",
     "Hey, wonderful that you asked. You probably are wondering why I am "
     "wandering around in this world. And so am I.\nOne day when I was "
     "installing the latest design of my corruption trap in one of the "
     "dungeons of Drakalor Chain, the trap became really wild...\n"
     "...the next thing I noticed was that I was stuck in this place. I "
     "don't even know what place this is, but it surely looks boring when "
     "compared to my own realm. Dungeons are so primitive and huge, the "
     "creatures are stupid and can't do nothing but fight.\n"
     "Now, I want out! I want a way to get back to my own world. Please, "
     "if you could just help me, I can do you a great favour in exchange.",
     "You helped Thomas Biskup to get back to his own realm.",
     0, NPC_THOMAS, 0, -1, 
     QNEED_ITEM, IS_SCROLL, SCROLL_TELEPORT, 0, 2000 },
   { "Ruthlessly terminate Bill Gates",
     "<weep> You surely have heard that everyone is after me. My company is "
     "being "
     "hacked into pieces and nothing goes well with Windows2000. The life "
     "is pure misery for me, thus I am asking if you could do a big favor "
     "for me and to the whole world. Would you please kill me?",
     "You have ruthlessly killed Bill Gates.",
     0, NPC_BILLGATES, 0, -1, 
     QNEED_KILLNPC, 0, NPC_BILLGATES, 1, 1000
   },
   { "Meet the burly adventurer called Sparhawk",
     "Sparhawk is looking for you you should go and seek him out.",
     "You met Sparhawk.",
     QFLAG_DONOTASK, NPC_NATASHA, 0, -1, 
     QNEED_MEETNPC, 0, NPC_SPARHAWK, 0, 500 },

   { NULL, 0, 0, 0, 0, 0, 0 }
};

const char qname_illegal[]="Illegal quest name!";

Quest::Quest()
	: id(0), monid(0), dungeon(0), level(0), flags(0)
{

}

Quest::Quest(int i, int mid, int dng, int lvl)
	: id(i), monid(mid), dungeon(dng), level(lvl), flags(quest_list[i].flags)
{

}

bool Quest::Check_Giver_Death(const int32u mid)
{
	if(!(flags & QFLAG_SOLVED) && monid == mid)
	{
		msg.vnewmsg(CH_RED, "Quest \"%s\" cannot be completed since "
			"the owner has just died.\n", quest_list[id].name);

		flags |= QFLAG_UNSOLVABLE;

		return true;
	}

	return false;
}

bool Quest::Is_Unfinished(const int32u mid)
{
	if (monid==mid && !(flags & QFLAG_SOLVED))
		return true;

	return false;
}

int Quest::Print_Solved(int index)
{
	/* write all finished quests */
	if (flags & QFLAG_SOLVED)
	{
		zprintf("%2d. \007%s.\n", index,
			quest_list[id].solvetxt);
		zprintf("    It was given by %s at %s level %d.\n",
			giver.c_str(),
			dungeonlist[dungeon].name,
			level);
		return 1;
	}

	return 0;
}

int Quest::Print_Unfinished(int index)
{
	if (!(flags & QFLAG_SOLVED))
	{
		zprintf("%2d. \007%s.\n", index, quest_list[id].name);
		zprintf("    Given at %s level %d by %s\n",
			dungeonlist[dungeon].name,
			level, giver.c_str());
		return 1;
	}

	return 0;
}

void Quest::Save(Tar_Ball &tb)
{
	tb.Put(id);
	tb.Put(monid);
	tb.Put(dungeon);
	tb.Put(level);
	tb.Put(flags);

	tb.Put_String(giver);
}

void Quest::Load(Tar_Ball &tb)
{
	id=tb.Get_Next_Value();
	monid=tb.Get_Next_Unsigned();
	dungeon=tb.Get_Next_Value();
	level=tb.Get_Next_Value();
	flags=tb.Get_Int16u();

	giver=tb.Get_Next_String();
}

int quest_getflags(int quest)
{
	if(quest < QUEST_TOTALNUM)
		return quest_list[quest].flags;
	else
		return 0;
}

void quest_showdesc(int qnum)
{
	if (qnum>=QUEST_TOTALNUM)
	{
		my_printf("Error: Trying to query quest %d, which is not available (Max=%d)!\n",
			qnum,QUEST_TOTALNUM-1);
		return;
	}

	if (quest_list[qnum].desc!=0)
		ww_print(quest_list[qnum].desc);
	else
		my_printf("Error: quest %d has no description (=NULL)!\n",qnum);
}

const char *quest_showname(int qnum)
{
	if (qnum>=QUEST_TOTALNUM)
	{
		my_printf("Error: Trying to query quest %d, which is not available (Max=%d)!\n",
			qnum,QUEST_TOTALNUM-1);
		return qname_illegal;
	}

	if (quest_list[qnum].name!=0)
		return quest_list[qnum].name;
	else
		return qname_illegal;
}
