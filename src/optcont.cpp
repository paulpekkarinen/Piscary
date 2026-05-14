//optcont.cpp - game options container

//Refactored 1.10.2021 - 19.4.2026 Paul K. Pekkarinen

#include "display.h"
#include "file.h"
#include "input.h"
#include "optcont.h"
#include "options.h"
#include "output.h"
#include "saldebug.h"
#include "storage.h"

using std::vector;

const char Optcont::confile[]="saladir.cfg";

Optcont::Optcont()
	: changed(false), index(0)
{
	//create list of options and assign global GONFIGVARS variable to
	//an instance, it actually works -->

	opts.push_back(new Bool_Option("ANYKEYMORE",
	"If true, allows you to continue on (more)-prompts by pressing "
	"*any* key instead of just SPACE, ENTER or ESC.", 'a',
	CONFIGVARS.anykeymore)); //<-- like this, using reference or pointer

	opts.push_back(new Bool_Option("AUTODOOR",
	"If true, you can walk to doors and you get a question whether you want "
	"to open the door doors or not. If false, doors are treated as walls "
	"and you'll have to open them manually with open-command.", 'b',
	CONFIGVARS.autodoor));

	opts.push_back(new Bool_Option("AUTOPICKUP",
	"If true, you will automagically pick up any item you walk into. "
	"But if a location contains multiple items, you will still have to pick "
	"then by yourself.", 'c', CONFIGVARS.autopickup));

	opts.push_back(new Bool_Option("AUTOPUSH",
	"If true, you'll automagically push unpassable items (ie. boulders). If "
	"false, you'll have to issue a push command by yourself.", 'd',
	CONFIGVARS.autopush));

	opts.push_back(new Bool_Option("COLORMESSAGES",
	"If true, the game will use colors in game messages as specified "
	"in the message. If false, all messages will show up in a plain "
	"white color.", 'e', CONFIGVARS.colormessages));

	opts.push_back(new Bool_Option("COLORTXT",
	"If true, the game will use colors in the text viewer as specified "
	"in the textfile. If false, all text will show up in a plain "
	"white color.", 'f', CONFIGVARS.colortext));

	opts.push_back(new Bool_Option("COMPACTMESSAGES",
	"If true, compacts repeated messages in the message buffer.", 'g',
	CONFIGVARS.compactmessages));

	opts.push_back(new Bool_Option("DOORDISTURB",
	"If true, stops walk mode if you walk next to a door.", 'h',
	CONFIGVARS.door_disturb));

	opts.push_back(new Bool_Option("DROPPILES",
	"If true, you'll drop item piles at once without any questions. "
	"If false, you'll be prompted for a count of items to get.", 'i',
	CONFIGVARS.droppiles));

	opts.push_back(new Bool_Option("FOODWARN",
	"If true, warns you when you're about to faint.", 'j',
	CONFIGVARS.foodwarn));

	opts.push_back(new Bool_Option("GETALLMONEY",
	"If true, you'll get all coins at once. If false, you'll"
	" be prompted for a coin count to get.", 'k',
	CONFIGVARS.getallmoney));

	opts.push_back(new Integer_Option("HEALTHALARM",
	"If greater than 0, enables the hitpoint alarm. "
	"You'll get an alarm when your hp in any of your bodyparts falls below "
	"the configured percentage of the maximum hitpoints in that bodypart.",
	'l', CONFIGVARS.health_alarm, 0, 100));

	opts.push_back(new Bool_Option("ITEMDISTURB",
	"If true, stops walk mode (repeat) when you stand over any item. "
	"If false you'll ignore items.", 'm',
	CONFIGVARS.item_disturb));

	opts.push_back(new Bool_Option("MONSTERDISTURB",
	"If true, stops walk mode if a monster becomes visible, also disallows"
	" repeat walking if monsters are visible.", 'n',
	CONFIGVARS.monster_disturb));

	//remember to give the correct size of the array
	opts.push_back(new Array_Option("PICKUPTYPES",
	"A list of things you'll automagically pick up.", 'o',
	CONFIGVARS.pickuptypes, 20));

	opts.push_back(new Integer_Option("REPEATCOUNT",
	"Specifies the default count for repeatable commands.", 'p',
	CONFIGVARS.repeatcount, 0, 1000));

	opts.push_back(new Bool_Option("REPEATUPDATE",
	"Show player in repeat mode", 'q',
	CONFIGVARS.repeatupdate));

	opts.push_back(new Bool_Option("STAIRDISTURB",
	"If true, stops walk mode when you stand over a staircase.", 'r',
	CONFIGVARS.stair_disturb));

	opts.push_back(new Bool_Option("TARGETLINE",
	"If true, the game will display a line when you're selecting "
	"a target for spells or missiles.", 's',
	CONFIGVARS.targetline));

	//max value is 2800 = TIME_1MIN
	opts.push_back(new Integer_Option("TICKSMIN",
	"Ticks/minute, how fast time passes", 't',
	CONFIGVARS.ticksperminute, 100, 2800));
}

Optcont::~Optcont()
{
	for (oitr ii = opts.begin() ; ii != opts.end() ; ++ii)
		delete (*ii);	
}

int Optcont::Amount()
{
	return (int)opts.size();
}

void Optcont::Show_List()
{
	display->Header("Options", CH_GREEN);
	display->Footer("Letter = select, Space = change, x = exit", CH_GREEN);

	const int x=10;
	int dx=x;
	int y=2;
	Coord cur;
	int i=0;
	for (oitr ii = opts.begin() ; ii != opts.end() ; ++ii)
	{
		int c;
		if (i==index)
		{
			c=CH_WHITE; //show selected in white
			cur.Set_Location(dx, y);

			//store input point
			inpoint.Set_Location(dx+(*ii)->Get_Input_Offset(), y);
		}
		else
			c=CH_GREEN;

		set_color(c);
		gotoxy(dx, y);
		(*ii)->Show();

		y++;

		//move to next column
		if (y>=16+2)
		{
			y=2;
			dx+=33;
		}
		
		i++;
	}

	//show selected option's description
	set_color(CH_CYAN);
	gotoxy(0, 19);
	opts[index]->Show_Description();

	//put cursor on the selected letter
	gotoxy(cur.x+2, cur.y);
}

void Optcont::Edit()
{
	Show_List();
	
	bool editing=true;

	while (editing)
	{
		const int k=my_getch();
		
		switch (k)
		{
			case ' ':
				gotoxy(inpoint.x, inpoint.y);
				opts[index]->Change();
				//set to changed even if values stay same, because
				//the decision was made to edit an option
				changed=true;
				Show_List();
			break;
			case 'x': editing=false; break;
			default:
			{
				const int i=get_index_from_key(k, Amount());
				if (i!=-1)
				{
					index=i;
					Show_List();
				}
			}
			break;			
		}
	}
}

void Optcont::Load()
{
	File savefile(confile);
	if (savefile.Open()==false)
	{
		debug->Message("Using default options.");
		return;
	}

	Tar_Ball tb;
	tb.Load(&savefile);

	for (oitr ii = opts.begin() ; ii != opts.end() ; ++ii)
		(*ii)->Load(tb);

	debug->Message("Options loaded from file.");
}

bool Optcont::Save()
{
	//options are "saved" if they are not changed
	if (changed==false)
		return true;

	File savefile(confile);

	if (savefile.Create()==false)
		return false;

	Tar_Ball tb;

	for (oitr ii = opts.begin() ; ii != opts.end() ; ++ii)
		(*ii)->Save(tb);

	tb.Save(&savefile);
				
	return true;
}
