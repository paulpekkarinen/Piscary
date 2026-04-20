/**************************************************************************
 * venture.cpp --                                                         *
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

//Refactored 24.8.2021 - 2.8.2023 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "message.h"
#include "names.h"
#include "output.h"
#include "quest.h"
#include "storage.h"
#include "venture.h"
#include "world.h"

using std::list;

Venture::~Venture()
{
	removeall();
}

int Venture::add(being *monster, int quest)
{
	if (!monster)
	{
		msg.addwait("Error: quest_add() monster pointer is NULL!",CH_RED);
		return QSTAT_ERROR;
	}

	/* test if the quest is already in the list (so it has been
	 * either generated or completed since all generated quests
	 * are in the players quest list
	 */
	if (find(quest)!=NULL)
		return QSTAT_ALREADYIN;

	/* test if the quest is available, ie. all prequisities are taken
	 * care of
	 */
	const int qstat=isavailable(quest);

	if (qstat!=QSTAT_OK)
		return qstat;

	/* create a node */
	Quest *nq=new Quest(quest, monster->id, world->Get_Dungeon_Index(),
		world->Get_Level_Index());

	if (!nq)
	{
		msg.addwait("Error: Can't add quest. No memory!",CH_RED);
		return QSTAT_ERROR;
	}

	/* init quest */
	nq->flags |= QFLAG_GENERATED;

	nq->giver=monster_sprintf(monster, false, true);

	my_setcolor(C_CYAN);
	msg.vnewmsg(C_CYAN,"You are now involved in a quest: %s!",
		quest_list[quest].name);
	zprintf("You're now involved in a quest: %s!\n",
		quest_list[quest].name);

	quests.push_back(nq);

	return QSTAT_OK;
}

/*
 * Check if the player has any quests which involve killing
 * a monster. Again compare mptr if that monster is on the
 * kill list.
 */
void Venture::check_kill(being *mptr)
{
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		Quest *qp=(*ii);
		if (quest_list[qp->id].solve == QNEED_KILLNPC)
		{
			const int npc=mptr->Get_Special_Id();

			if (quest_list[qp->id].cond2 == npc && npc>0)
			{
				/* yes there was a NPC to be killed */

				complete(qp->id);

				if (quest_list[qp->id].rewardimm)
					reward(qp->id);
			}
		}
	}
}

void Venture::complete(int qnum)
{
   Quest *qp = find(qnum);
   if(qp!=0)
      qp->flags |= QFLAG_SOLVED;
}

Quest *Venture::find(int quest)
{
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		Quest *qptr=(*ii);
		if (qptr->id==quest) return qptr;
	}
	return 0;
}

int Venture::isavailable(int quest)
{
	Quest *qp = find(quest);

	/* the quest was in the list,
	 * check if it is completed
	 */
	if (qp!=0)
	{
		if (qp->flags & QFLAG_SOLVED)
			return QSTAT_SOLVED;
		else
			return QSTAT_ALREADYIN;
	}

	if (quest_list[quest].prequest >= 0)
	{
		qp = find(quest_list[quest].prequest);
		if (qp!=0)
		{
			if (!(qp->flags & QFLAG_SOLVED))
				return QSTAT_PREQUEST;
		}
	}

	/* it's ok to add */
	return QSTAT_OK;
}

int Venture::nextavail(being *monster)
{
	int q=0, sq=-1;
	int lindx=0xffff;

	/* monster needs to be a special npc */
	if (monster->m.special == 0)
		return -1;

	my_printf("%d\n",monster->m.special);

	while (quest_list[q].name!=NULL)
	{
		if (monster->m.special == quest_list[q].owner)
		{
			if (isavailable(q)==QSTAT_OK)
			{
				if (lindx>quest_list[q].qnum)
				{
					lindx=quest_list[q].qnum;
					sq=q;
				}
			}
		}
		q++;
	}

	return sq;
}

/* this is called every time a monster dies
 * removes all quests whose owner dies.
 */
bool Venture::ownerdied(being *mptr)
{
	bool res=false;

	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		if ((*ii)->Check_Giver_Death(mptr->id))
			res=true;
	}

	return res;
}

bool Venture::remove(int quest)
{
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		Quest *qptr=(*ii);
		if (qptr->id==quest)
		{
			quests.erase(ii);
			delete qptr;
			return true;
		}
	}
	return false;
}

void Venture::removeall()
{
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
		delete (*ii);
	quests.clear();
}

void Venture::reward(int qnum)
{
	Quest *qp = find(qnum);

	if (!qp) return;

	if (qp->flags & QFLAG_REWARDED)
		return;

	/* ensure that the quest will not be rewarded again */
	qp->flags |= QFLAG_REWARDED;

	/* reward the player */
	if (quest_list[qnum].exp > 0)
	{
		msg.vnewmsg(CH_GREEN,"You gain %d exp for completing a quest.",
			quest_list[qnum].exp);
		player.Gain_Experience(quest_list[qnum].exp);
	}
}

void Venture::showall()
{
	if (quests.empty())
	{
		msg.newmsg("You are not involved in any quest.", C_WHITE);
		return;
	}

	zprintf("You're involved in these quests:\n\n");

	int i=1;
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		i+=(*ii)->Print_Unfinished(i);
	}
	if (i==1)
		zprintf("None.\n");

	zprintf("\nThe quests you have solved by now:\n\n");

	i=1;
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		i+=(*ii)->Print_Solved(i);
	}
	if (i==1)
		zprintf("None.\n");

#ifdef QUESTUNSOLVABLESTUFF
	fprintf(fh,"\n\001Quests which are not solvable but you're involved!\n\n");

	qptr=*list;
	i=1;
	while (qptr)
	{
		/* first write all unfinished quests */
		if (qptr->quest.flags & QFLAG_UNSOLVABLE)
		{
			fprintf(fh,"\002%2d. \007%s.\n",i,quest_list[qptr->quest.id].name);
			fprintf(fh,"    Given at %s level %d by %s\n",
				dungeonlist[qptr->quest.dungeon].name,
				qptr->quest.level,qptr->quest.giver);
		}
		qptr=qptr->next;
		i++;
	}
	if (i==1)
		fprintf(fh,"\002None.\n");
#endif
}

int Venture::unfinished(being *monster)
{
	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		Quest *qptr=(*ii);
		if (qptr->Is_Unfinished(monster->id))
			return qptr->id;
	}

	return -1;
}

void Venture::save(Tar_Ball &tb)
{
	const int amt=(int)quests.size();
	tb.Put(amt);

	for (qitr ii = quests.begin() ; ii != quests.end() ; ++ii)
	{
		(*ii)->Save(tb);
	}
}

void Venture::load(Tar_Ball &tb)
{
	const int amt=tb.Get_Next_Value();

	for (int t=0; t<amt; t++)
	{
		Quest *q=new Quest;
		q->Load(tb);
		quests.push_back(q);
	}
}
