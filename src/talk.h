//Legend of Saladir - talk.h

//Unit talk: Conversation routines.

#ifndef TALK_H
#define TALK_H

struct being;
struct playerinfo;
struct wordlist;

struct chatinfo
{
	const char *firstgreet; /* when met for the first time */
	const char *knowgreet; /* when met again */
	wordlist *known_words; /* pointer to a list of known words */
};

void chat(playerinfo *plr); //chat with a creature
void monster_talk(being *monster);
void talk_greet(being *monster);
void talk_quest(being *monster);
void talk_rumour(being *monster);
void talk_unknown(being *monster);

extern chatinfo NPCchatinfo[];

#endif
