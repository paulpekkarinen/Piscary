//Legend of Saladir - script.h

//Unit script: Container for verses - text data pieces.

#ifndef SCRIPT_H
#define SCRIPT_H

#include <array>
#include "verse.h"

//Loads text data from scripts.txt which contains text pieces for each
//enum in Script_List inside [brackets] number. Each tag can have more
//than one text piece which is separated with |. Tag numbers must match
//with enum and there has to be Amt_Of_Scripts amount of tags. If
//anything goes wrong the program will most likely crash.
class Script
{
public:
	enum Script_List
	{
		Door_Sound,
		Eating_Sound,
		Wall_Hit,
		Sparhawk_Comments,
		Keeper_Bill_Message,

		Unpaid_Item,
		Keeper_Nobill_Message,
		Keeper_Unseen,
		Keeper_Throw,
		Bill_Gates_Message,

		Thomas_Message,
		Wind_Speed,
		Rumours,
		Empty_Scroll,
		Shop_Armours,

		Shop_Weapons,
		Shop_General,
		Shop_Food,

		Amt_Of_Scripts	
	};
	
private:
	std::array<Verse, Amt_Of_Scripts> articles;
	bool working; //true if texts loaded and worked
	
public:
	Script();

	const char *Get_At_Index(int id, int index);
	int Get_Items(int id);
	const char *Get_Random(int id);

	void Chain_Message(int id, int color);
	void Index_Message(int id, int index);
	void Message(int id, int color);
	void Random_Message(int id);
	void Random_Message(int id, int color);

	void Debug(int id);
};

extern Script *texts;

#endif

