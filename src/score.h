/*
 * Score.h for Legend of Saladir
 *
 * (C)1997/1998 by Erno Tuomainen
 *
 */

//Unit score: Score data for the result of the game.

#ifndef SCORE_H
#define SCORE_H

#include <ctime>
#include "mondef.h"

struct playerinfo;

#define NAMEMAX			20
#define TITLEMAX        30

struct Score
{
	monsterdef monsu;
	std::string deathreason;
	
	int copper; /* how much copper player had */
	int quests; /* number of quests completed */
	int dindex;  /* dungeon index (where died) */
	int places; /* number of places visited */
	int levels; /* number of dungeon levels visited */

	int moves;  /* number of moves player took */
	int kills; //enemies killed
	int final; //final amount of score

	time_t mytime;

	Score();
	explicit Score(const monsterdef &m, int a); //debug constructor
	explicit Score(const playerinfo &plr);

	bool Is_Better_Or_Equal(Score *s);
	bool Is_List_Match(Score *s);
	void Show(int index, int y);

	void Load(Tar_Ball &tb);
	void Save(Tar_Ball &tb);
};

#endif
