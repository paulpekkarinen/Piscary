/*
 * talk.cpp for Legend of Saladir
 *
 * (C)1997/1998 Erno Tuomainen
 *
 */

//Refactored 14.7.2022 - 21.2.2025 Paul K. Pekkarinen

#include <cstring>
#include "avatar.h"
#include "being.h"
#include "creature.h"
#include "dice.h"
#include "game.h"
#include "gameview.h"
#include "input.h"
#include "lexicon.h"
#include "message.h"
#include "move.h"
#include "names.h"
#include "output.h"
#include "quest.h"
#include "specmon.h"
#include "talk.h"
#include "words.h"

using std::string;

constexpr int INPUT_MAX=70;

/*
 * NPC talk function prototypes
 */
void NPCtalk_sparhawk_name(being *monster);
void NPCtalk_sparhawk_town(being *monster);
void NPCtalk_sparhawk_help(being *monster);

const char *resp_unknown[]=
{
   "Sorry, I know nothing about that.",
   "Sorry, I really can't help you with that.",
   0
};

const char *resp_noname[]=
{
   "I won't tell you my name.",
   "My name is not your business.",
   0
};

constexpr int RESP_GREETKNOWCOUNT=4;
const char *resp_greetknow[]=
{
     "We meet again, %s.",
     "Oh, the world is so small, %s.",
     "Nice to see you again, %s!",
     "Where've you been, %s?",
     0
};

#define RESP_NOTHUMANCOUNT 4
const char *resp_nothumanly[]=
{
     "%s grunts.",
     "%s squawks.",
     "%s yodls.",
     "%s yarns.",
     0
};

#define RESP_HUMANLYCOUNT 4
const char *resp_humanly[]=
{
     "We meet again, %s.",
     "Oh, the world is so small, %s.",
     "Nice to see you again, %s!",
     "Where've you been, %s?",
     0
};

/*
 * Special NPC Wordlists
 */
wordlist NPCwlist_sparhawk[] =
{
   { "help", NPCtalk_sparhawk_help },
   { "name", NPCtalk_sparhawk_name },
   { "town", NPCtalk_sparhawk_town },

   { 0, 0 }
};

/*
 * A table for special NPC characters. Using this table
 * they greet players when a chat mode begins.
 *
 * THERE MUST BE AS MANY STRUCTURES IN THIS ARRAY
 * as there is active NPC characters!!!!!!!!!!!!!!!!!!!!
 *
 */
chatinfo NPCchatinfo[]=
{
     /* greetings text for unknown monsters */
     { "Who're you and what do you want?",
       "Why do you keep pestering me?", 0
     },
     /* billgates */
     { "", "", 0
     },
     /* sparhawk */
     { "( For a while you've have a chilled feeling. It's like this"
       " person can see through your soul)\n\n"
       "(With a booming voice) Greetings traveller, my name is Sparhawk."
       " From the distance I noticed that you could use some help. I am "
       "willing to help you but first, would you tell me who are you? ",
       "Hello again, how can I help you with?",
       NPCwlist_sparhawk
     },
     /* Thomas biskup */
     { "", "", 0
     },
     /* Natasha */
     { "", "", 0
     },
     { 0, 0, 0 }
};

const char *txt_sparhawk_help[] =
{
   "I can provide you with quite a bit of information, just keep asking "
   "for help and I will try to aid you if possible. But first, a word or "
   "two on talking.\n\n "
   "When you're talking with people you can always talk about jobs, quests, "
   "news and such. Some persons can privide you with information on places "
   "and towns.\n\n"
   "You don't need to write whole phrases "
   "it's enough to just type the most important word. Also remember that "
   "it's always polite to greet the person before going into further "
   "discussion.",

   "I wouldn't suggest for you to go fighting without weapons, not "
   "unless you're very skilled in weaponless combat. Check out your "
   "equipment ('q') and equip whatever you might be carrying for now. "
   "If you have some gold with you why not visit some local shops here, "
   "they sell some pretty darn good equipment.",

   "Dungeons and caves are mostly dark. You need some light to see what "
   "is happening in there. I can see that you already have got something "
   "which can help you with that. Just equip it and you can do much better.",

   "Salmorria and all the towns are all using the same financial system. "
   "We have three types of coins: gold, silver and copper. A gold coin "
   "is worth of 10 copper coins and 2 silver coins. A silver coin is "
   "worth of 5 copper coins.",

   "Did you know there's a dungeon nearby to the northwest of this "
   "town. I've been in there myself, the habitants are not too "
   "aggressive and mostly quite easy to kill or evade. When you've "
   "prepared yourself, why not go and visit there.",

   "I've now told you everything I can help you with. When you meet "
   "new people you should try to talk with them. I hope you best with "
   "your travels and maybe we meet again.",

   0
};

void NPCtalk_sparhawk_help(being *monster)
{
   static int hi = 0;

   ww_print(txt_sparhawk_help[hi++]);

   if(txt_sparhawk_help[hi]==0) {
      hi=0;
   }
}

void NPCtalk_sparhawk_town(being *monster)
{
   ww_print("Santhel is a small town located in the western parts of "
	    "Sallmorria. We stay mostly by ourselves but occasionally "
	    "have some visitors from far east and other parts of the "
	    "continent. I suggest that you visit the local stores around "
	    "here before doing anything else, we have a decent supply "
	    "of goods and many merchants are staying here.");
}

void NPCtalk_sparhawk_name(being *monster)
{
	const char *plrname=player.Get_Name();

	/* identify sparhawk */
	if (!(monster->m.status & MST_KNOWN))
	{
		zprintf("Nice to meet you %s! ",
			plrname);
		ww_print("It's been a while since we met a stranger in town.");

		monster->m.status |= MST_KNOWN;

		NPCtalk_sparhawk_help(monster);
	}
	else
	{
		zprintf("Hello again %s, I assume you're doing well.", plrname);
	}
}

void talk_unknown(being *monster)
{
   ww_print(resp_unknown[0]);
}

void talk_greet(being *monster)
{
	if (monster->m.status & MST_KNOWN)
	{
		zprintf(resp_greetknow[RANDU(RESP_GREETKNOWCOUNT)], player.Get_Name());
	}
	else
	{
		monster->m.status|=MST_KNOWN; // monster has been met now
		zprintf("Nice to meet you %s, my name is %s.",
			player.Get_Name(), monster->m.name.c_str());
	}
}

void talk_rumour(being *monster)
{
   ww_print("Nothing new...");
}

void talk_quest(being *monster)
{
	/* one active quest per monster only */
	int qnum=player.quests.unfinished(monster);

	if (qnum >= 0)
	{
		ww_print("You know, I already gave you a quest. Please try to finish "
			"it first.\n");
		string moname=monster_sprintf(monster, true, true);
		my_setcolor(C_CYAN);
		zprintf("%s has given you a quest \"%s\", which is still unfinished.",
			moname.c_str(), quest_showname(qnum));
		return;
	}

	/* find next available quest */
	qnum=player.quests.nextavail(monster);

	if (qnum<0 || qnum>=QUEST_TOTALNUM)
	{
		ww_print("Sorry, I don't have any quests for you. "
			"Maybe you should keep asking!");
		return;
	}

	quest_showdesc(qnum);

	if (!(quest_getflags(qnum) & QFLAG_DONOTASK))
	{

		set_color(CH_WHITE);
		my_printf("\n\nDo you accept this quest (Y/n)?");
		set_color(C_WHITE);
		if (confirm_yn(NULL, true, false))
		{
			my_printf("\n");
			player.quests.add(monster, qnum);
		}
		else
			my_printf("\nOk, as you wish.\n");
	}
	else
		player.quests.add(monster, qnum);
}

void chat(playerinfo *plr)
{
	int dir=dir_askdir("Talk to", true);

	if (dir>9) return;

	Coord nc=move_to_direction(dir, plr);

	if (plr->Is_At(nc))
		msg.newmsg("You chat deeply with yourself!", C_WHITE);
	else
	{
		being *mptr=gameview.Get_Monster(nc);
		if (mptr)
			monster_talk(mptr);
		else
			msg.newmsg("You mumble by yourself...", C_WHITE);
	}
}

void monster_talk(being *monster)
{
	int bbc=0;
	string moname=monster_sprintf(monster, true, true);

	if (npc_races[monster->m.race].behave & BEHV_ANIMAL)
	{
		msg.vnewmsg(C_WHITE, resp_nothumanly[RANDU(RESP_NOTHUMANCOUNT)],
			moname.c_str());
		return;
	}

	/* if it's attacking the player, it wont't talk much */
	if (monster->m.status & MST_ATTACKMODE && monster->target.olento==&player)
	{
		msg.newmsg("No response.", C_WHITE);
		return;
	}

	clear_screen();
	enable_scroll();
	my_printf("%s, you are now talking with %s.\n\n", player.Get_Name(), moname.c_str());

	if (monster->m.status & MST_KNOWN)
		zprintf("%s\n", NPCchatinfo[monster->m.special].knowgreet);
	else
		zprintf("%s\n", NPCchatinfo[monster->m.special].firstgreet);

	while (1)
	{
		my_setcolor(CH_RED);
		my_printf("> ");
		my_setcolor(CH_WHITE);
		string myword=get_string(false, INPUT_MAX);

		if (is_same_string(myword, "bye")==0)
			break;
		if (myword.empty())
		{
			bbc++;
			if (bbc==2)
				break;

			my_setcolor(CH_CYAN);
			my_printf("To end talking, type \"bye\" or press another enter.\n");

		}
		else
		{
			bbc=0;
			my_setcolor(C_WHITE);
			wordtalk(monster, myword.c_str());
			my_printf("\n");
		}
	}

	disable_scroll();
	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	return; //note: returns before the next section is reached

	if (monster->m.special)
		monster_chatspecials(monster);
	else
	{
		if (npc_races[monster->m.race].behave & BEHV_ANIMAL)
		{
			msg.newmsg(C_WHITE,
				resp_nothumanly[RANDU(RESP_NOTHUMANCOUNT)], moname.c_str());
		}
		else
		{
			if (!(monster->m.status & MST_KNOWN))
			{
				if (strlen(monster->Get_Name())>0)
				{
					monster->m.status|=MST_KNOWN; // monster has been met now
					msg.newmsg(C_WHITE, "Hi, my name is %s", monster->Get_Name());
				}
				else
				{
					string s;
					random_language(s, 30+RANDU(40));
					s.append("?");
					msg.newmsg(s, C_WHITE);
				}
			}
			else
			{
				msg.newmsg(C_WHITE, resp_humanly[RANDU(RESP_HUMANLYCOUNT)],
					player.Get_Name());
			}
		}
	}
}
