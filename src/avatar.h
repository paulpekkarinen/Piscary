//Legend of Saladir - avatar.h

//Unit avatar: The player game object.

#ifndef AVATAR_H
#define AVATAR_H

#include "being.h"
#include "qskills.h"
#include "venture.h"

class Levelnode;
class SelectItemsPack;

struct playerinfo : public being
{
	int color;      // color of player char
	int nutr;       // food status
	int sight;      // distance of sight

	int delta; //note: unused line of sight distance
	int num_places; // number of places visited
	int num_levels; // number of levels visited
	int num_kills;

	bool huntmode;
	bool repeatwalk;
	bool monsterinsight;
	bool searchmode;

	quickskill qskills[NUM_QUICKSKILLS];
	Venture quests;
	SelectItemsPack *backpack; //inventory routines

	playerinfo();
	~playerinfo();

	const char *Get_Title();

	bool Is_Player() override { return true; }

	void Getangry(level_type *level, Actor *kohde, bool always) override;
	void Change_Alignment(char dir, int amount);
	void Changetactics();
	void Check_Bill() const;
	void Checkstat(level_type *level, bool lower, bool showmsg);
	void Damage_Message(Damage &dmg) override;
	void Eat_Addnutr(item_def *item, int weight);
	void Gain_Experience(int gain) override;

	void Go_Hunting(); //wildhunt
	void Handle_Confusion(Condition *cond) override;
	void Handlestatus(level_type *level, int slots);
	void Hitwall();
	void Initialize();
	void Jump_To(const Coord &c);
	void Killedmonster(being *mptr);
	void Lastdir_To_Doorflag(level_type *level);
	void Noticestuff(level_type *level);
	void Shouldflee(level_type *level) override;
	void Show_Inventory();
	void Switch_Searchmode();

	void Display();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, level_type *level);
};

extern playerinfo player;

#endif
