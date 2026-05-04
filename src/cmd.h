//Legend of Saladir - cmd.h

//Unit cmd: Game command data and functions.

#ifndef CMD_H
#define CMD_H

#include "types.h"

#define CMD_MAXLEN 4

namespace cmd
{
	//these has to be in the same order as the data in cmdtable
	enum Type
	{
		Move_North,
		Move_Northeast,
		Move_East,
		Move_Southeast,
		Move_South,
		Move_Southwest,
		Move_West,
		Move_Northwest,

		Walk_North,
		Walk_Northeast,
		Walk_East,
		Walk_Southeast,
		Walk_South,
		Walk_Southwest,
		Walk_West,
		Walk_Northwest,

		Enter_Downstairs,
		Enter_Upstairs,

		Equip_Item,
		Show_Inventory,
		Pick_Item,
		Drop_Item,

		Game_Menu,
		Show_Quests,
		Show_Character_Status,
		Show_Journey_Log,
		Show_Weather,

		Change_Tactics,
		Apply_Skill,
		Apply_Quickskill_0,
		Apply_Quickskill_1,
		Apply_Quickskill_2,
		Apply_Quickskill_3,
		Apply_Quickskill_4,
		Apply_Quickskill_5,
		Apply_Quickskill_6,
		Apply_Quickskill_7,
		Apply_Quickskill_8,
		Apply_Quickskill_9,

		Push_Something,
		Search,
		Toggle_Search,
		Look,
		Look_Around,
		Read,
		Open_Door,
		Close_Door,
		Cast_Spell,
		Eat_Food,
		Throw_Or_Shoot,
		Rest,

		Talk,
		Pay_Bill,
		Check_Bill,
	
		Redraw_Screen,
		Previous_Message,
		Show_Message_Buffer,		
		Show_Help_Menu,
		Show_Keys,
		Show_Error_Log,

		Cheat_Light,
		Cheat_Identify_Item,
		Debug_Quit, //quit fast without endgame stuff, but still cleaning up
		Debug_Test,
		Debug_Messages,
		Debug_Menu,

		Amt_Of_Commands
	};

	enum Category
	{
		Navigation,
		Items,
		Adventure,
		Traits,

		Actions,
		Interact,
		View,
		Debugging,

		Edit
	};
};

struct Command_Data
{
	const char *name;
	int keys[CMD_MAXLEN+1];
	int category;
	bool redraw_view; //redraw everything after this command
};

void read_cmd();

#endif
