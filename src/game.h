//Legend of Saladir - game.h

//Unit game: Data of a gameplay.

#ifndef GAME_H
#define GAME_H

#include "gametime.h"

#define GAME_DO_REDRAW     0x80000000 // true if screen needs redraw
#define GAME_SHOWALLSTATS  0x0fffffff

#define GAME_EXPERCHG      0x00000001
//#define GAME_STATUSCHG     0x00000002  // true if status line needs redraw
#define GAME_ATTRIBCHG     0x00000004
#define GAME_MONEYCHG      0x00000008
#define GAME_HPSPCHG       0x00000010
#define GAME_ALIGNCHG      0x00000020
#define GAME_LEVELCHG      0x00000040
#define GAME_EDITORCHG     0x00000080
#define GAME_CONDCHG       0x00000100

class gamedata
{
public:
	enum Runstates
	{
		Running,
		Debug_Quit,
		Quit_And_Save,
		End_Of_Game
	};

private:
	static const int TIMEMOD_OUTSIDE=10;

	int state; //game running state
	int32u monid; /* monster generation id, for next monster */
	Gametime passedtime;

	void advance_time(int ticks);
	void clean_up();
	void initialize(bool fast);
	void quit();
	void Save();

public:
	gamedata() : state(Running), monid(0) { }

	int32u get_new_monster_id();

	void Eat_Passturns(level_type *level, item_def *item); //pass turns while eating
	void Menu(); //game menu: options, save, exit, etc.
	bool noticeevents(level_type *level);
	void Passturn(bool playervis, bool foodsub);
	void run(bool fast); //run one game
	void Set_State(int st) { state=st; }
	void show_time();

	int Load(const char *plrname);
};

extern gamedata Game;
extern int32u GAME_NOTIFYFLAGS;

#endif
