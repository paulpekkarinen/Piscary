/**************************************************************************
 * skillset.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1888                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 12.6.2022 - 28.8.2026 Paul K. Pekkarinen

#include <format>
#include "avatar.h"
#include "dice.h"
#include "game.h"
#include "input.h"
#include "lexicon.h"
#include "log.h"
#include "magic.h"
#include "message.h"
#include "output.h"
#include "program.h"
#include "skills.h"
#include "skillset.h"
#include "storage.h"

using std::format;
using std::string;

const char *skilllevelnames[]=
{
	"unskilled",       /* 0 */
	"beginner",
	"trainee",
	"promising",
	"apprentice",
	"journeyman",
	"skilled",
	"reliable",
	"expert",
	"master",
	"grandmaster",     /* 10 */
	NULL
};

const char *txt_listinstru=
"\007To browse, use \002UP/DOWN \007or \002A/Z \007keys. Press "
"\002?\007 to get skill description. Pressing "
"\002ENTER \007or \002SPACE \007 selects the highlighted skill.\n";

const char *txt_listinstru2=
"\007Tag/untag quick skills with keys\001 0\007 -\001 9\007. Pressing "
"\001C\007 clears all quick skills. "
"\002x \007or \002Q \007exits without selection.\n";

const char *txt_skillauto=
"\007This is an \003automatic \007skill. It is used automatically in correct "
"situations if you have learned it. This skill can't be selected.\n";

skillset::~skillset()
{
	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
		delete (*ii);
	skills.clear();
}

skill *skillset::find_skill(int group, int type)
{
	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		skill *s=(*ii);
		if (group==s->group && type==s->type)
			return s;
	}

	return 0;
}

skill *skillset::get_skill_handle(int index)
{
	const int sz=(int)skills.size();

	if (index<0) return 0;
	else if (index>=sz) return 0;

	int pos=0;
	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		if (pos==index)
			return (*ii);
		pos++;
	}

	return 0;
}

bool skillset::Has_Skills(int group)
{
	if (skills.empty())
	{
		msg.newmsg("You don't know any special skills.", C_WHITE);
		return false;
	}

	int scount=0;
	if (group!=SKILLGRP_ALL)
	{
		for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
		{
			if ((*ii)->group==group)
				scount++;
		}

		if (scount==0)
		{
			msg.newmsg(C_WHITE, "You don't know any %s skills.",
				skillgroupnames[group]);
			return false;
		}
	}

	return true;
}

void skillset::add_skill(skill *s)
{
	//uncheck, this will be used when creating selected lists
	skills.push_back(s);
}

skill *skillset::add_new_skill(int group, int type, int value)
{
	//don't add the skill if it already exists
	if (find_skill(group, type)!=0)
		return 0;

	skill *s=new skill;
	s->reset(group, type, value);
	skills.push_back(s);

	return s;
}

bool skillset::apply()
{
	//note: the original bool skill_apply(Tskillpointer list) was empty

	return true;
}

int skillset::check(int group, int type)
{
	skill *s=find_skill(group, type);
	if (s!=0) return s->cur;
	return 0;
}

int skillset::build_skillarray(int group, skillset &mylist)
{
	int amt=0;

	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		skill *s=(*ii);
		if (group==s->group || group == SKILLGRP_ALL)
		{
			mylist.add_skill(s);
			amt++;
		}
	}

	mylist.skills.sort();

	mylist.skills.sort(
	[](skill *l, skill *r)
	{
		return std::tie(l->cur) < std::tie(r->cur);
	});

	return amt;
}

int skillset::listselect(int &group, const char *prompt)
{
	bool endreach=false;
	bool topreach=false;
	bool eraseneeded=false;

	int soffs, loffs=0, i, j;

	int box_sy=14;  /* size y */
	int box_sx=24;  /* size x */
	int box_bx=2;
	int box_by=2;

	int sel=group;

	if (sel > SKILLGRP_MAGIC)
	{
		diary.Write("listselect(): Trying to pass illegal group number (%d).", sel);
		return -1;
	}

	skillset ptrlist;
	int num_skills = build_skillarray(sel, ptrlist);

	if (!num_skills)
	{
		msg.newmsg(C_WHITE, "You don't know any %s skills.",
			skillgroupnames[group]);
		return -1;
	}

	clear_screen();
	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	sel = 0;

	hidecursor();
	my_setcolor(C_GREEN);
	makeborder(box_bx-1, box_by-1, box_sx+2, box_sy+2);

	if (prompt)
	{
		my_wordwraptext(prompt, box_by, SCREEN_LINES, (box_bx+box_sx+2),
			SCREEN_COLS);
	}

	my_wordwraptext(txt_listinstru, box_by+box_sy+1, SCREEN_LINES, box_bx,
		SCREEN_COLS);
	my_wordwraptext(txt_listinstru2, get_cursor_y(), SCREEN_LINES, box_bx,
		SCREEN_COLS);

	loffs=soffs=0-(box_sy/2);

	int listcolor=CH_DGRAY;
	skill *curr=0; //currently selected

	while (1)
	{
		endreach=false;

		for (i=0; i<box_sy; i++)
		{
			skill *sptr=ptrlist.get_skill_handle(sel+i);

			/* test for array start boundary */
			if ((loffs+i)<0)
				topreach=true;
			else
				topreach=false;

			string skillname;

			if (!topreach)
			{
				/* test for array end boundary */
				if (sptr->group==SKILLGRP_ENDLIST || sptr->type==SKILL_ENDLIST)
				{
					endreach=true;
				}

				if (!endreach)
				{
					for (j=0; j<NUM_QUICKSKILLS; j++)
					{
						if (player.qskills[j].Is_Selected(sptr))
						{
							skillname=format("({}) ", j);
							break;
						}
					}

					if (sptr->Is_Automatic())
						listcolor=CH_DGRAY;
					else
						listcolor=C_WHITE;

					skillname.append(sptr->Get_Name());

					truncate_string(skillname, box_sx);
					skillname[0]=toupper(skillname[0]);
				}
			}

			if (i==box_sy/2)
				set_color(CH_YELLOW);
			else
				set_color(listcolor);

			const int namelen=(int)skillname.size();
			drawline_limit(box_by+i, box_bx, box_bx+box_sx-1, ' ');
			gotoxy(box_bx+((box_sx/2)-namelen/2), box_by+i);
			my_printf("%s", skillname.c_str());
		}

		/* show some info */
		curr=ptrlist.get_skill_handle(sel);

		gotoxy(box_bx+box_sx+2, box_by+box_sy-3);

		curr->Show_Selected();

		gotoxy(box_bx+box_sx+2, box_by+box_sy-2);
		set_color(C_WHITE);
		my_printf("Knowledge: %d%% of %d%% (%s)",
			curr->cur, curr->max,
			skilllevelnames[curr->cur/10]);
		clrtoeol();
		gotoxy(box_bx+box_sx+2, box_by+box_sy-1);
		my_printf("Needs %d marks to advance!",
			(Needmarks(curr->group, curr->cur) -
				curr->raise));
		clrtoeol();

		const int ch=my_getch();

		/* erase description area first */
		if (eraseneeded)
		{
			for (i=0; i<box_sy; i++)
				drawline_limit(box_by+i, box_bx+box_sx+2, SCREEN_COLS, ' ');
			eraseneeded=false;
		}

		if ((ch=='Z' || ch=='z' || ch==KEY_DOWN) && sel<(num_skills-1))
		{
			loffs++;
			sel++;
		}
		if ((ch=='A' || ch=='a' || ch==KEY_UP) && sel>0)
		{
			loffs--;
			sel--;
		}

		if (ch>='0' && ch<='9')
		{
			if (curr->Is_Automatic())
			{
				eraseneeded=true;
				my_wordwraptext(txt_skillauto,
					box_by, SCREEN_LINES, (box_bx+box_sx+2),
					SCREEN_COLS);
			}
			else
			{
				for (j=0; j<NUM_QUICKSKILLS; j++)
				{
					if (player.qskills[j].group == curr->group &&
						player.qskills[j].type == curr->type &&
						(ch-'0' != j))
					{
						player.qskills[j].select=false;
					}
				}
				j=ch-'0';
				if (j < NUM_QUICKSKILLS)
				{
					player.qskills[j].Toggle(curr);
				}
			}
		}

		if (ch=='c' || ch=='C')
		{
			for (j=0; j<NUM_QUICKSKILLS; j++)
			{
				player.qskills[j].Reset();
			}
		}

		if (is_confirm_key(ch))
		{
			if (curr->Is_Automatic())
			{
				eraseneeded=true;
				my_wordwraptext(txt_skillauto,
					box_by, SCREEN_LINES, (box_bx+box_sx+2),
					SCREEN_COLS);
			}
			else
				break;
		}

		if (ch=='x')
		{
			sel=-1;
			break;
		}

		if (ch=='?')
		{
			eraseneeded=true;

			set_color(C_WHITE);
			gotoxy(box_bx+box_sx+2, box_by);
			const char *skill_desc=curr->Get_Description();

			if (skill_desc)
			{
				my_wordwraptext(skill_desc,
					box_by, SCREEN_LINES, (box_bx+box_sx+2),
					SCREEN_COLS);
			}
			else
				my_printf("No description for the skill.");
		}
	}

	showcursor();

	if (sel<0)
	{
		group=0;
		msg.newmsg("No skill selected.", C_WHITE);
	}
	else
	{
		group=curr->group;
		sel=curr->type;
	}

	/* it should return <0 if no skill selected
	 * if skill is selected, return the skill type number and
	 * place skill group number into *group (parameter given)
	 */

	return sel;
}

bool skillset::list_skills(int group)
{
	if (Has_Skills(group)==false)
		return false;

	clear_screen();
	my_center_puts(1, "%s skills you have knowledge of",
		skillgroupnames[group]);

	my_setcolor(C_YELLOW);
	drawline(2, '-');

	my_setcolor(C_WHITE);
	my_printf("\n");

	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		skill *s=(*ii);

		if (group>0 && group!=s->group)
			continue;

		skilltype *skillptr=s->Get_Data();

		my_printf("%18s - %-11s [%3d%%]\n",
			skillptr->name,
			skilllevelnames[s->cur/10],
			s->cur);
	}
	showmore(false, false);

	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	return true;
}

void skillset::melee_learnskills(item_def *iptr, int amount)
{
	if (!iptr)
	{
		modify_raise(SKILLGRP_WEAPON, SKILL_HAND, amount,
			MIN_SKILLSCORE);
	}
	else
	{
		if (iptr->type==IS_WEAPON1H || iptr->type==IS_WEAPON2H
			|| iptr->type==IS_MISWEAPON)
		{
			modify_raise(SKILLGRP_WEAPON, iptr->group, amount,
				MIN_SKILLSCORE);
		}
	}
}

bool skillset::modify(int group, int type, int value, bool addit)
{
	skill *s=find_skill(group, type);

	//add the skill if it doesn't exist and addition is enabled
	if (s==0)
	{
		if (addit)
		{
			s=add_new_skill(group, type, value);
			if (s==0) return false;
			return true;
		}
		return false;
	}

	s->learn(value);
	return true;
}

/* modify the raise counter of skill */
/* will add the skill if doesn't exist */
/* if addval>0 then skill will be added if there isnt yet */
bool skillset::modify_raise(int group, int type, int amount, int addval, bool plr)
{
	bool skilladd=false;
	skill *sptr=find_skill(group, type);

	if (sptr==0)
	{
		if (addval>0)
		{
			sptr=add_new_skill(group, type, addval);
			if (sptr==0)
				return false;
			skilladd=true;
		}
		else
			return false;
	}

	const char *nameptr=sptr->Get_Name();

	//global_initmode is a global mode for initializing skills and this prevents
	//displaying the message during the initialization process
	if (skilladd && plr==true && !global_initmode)
	{
		msg.vnewmsg(CH_GREEN, "You gained a new %s skill %s!",
			skillgroupnames[sptr->group], nameptr);
	}

	if (sptr->cur>=MAX_SKILLSCORE)
		return true;

	/* increase skill learn counter */
	sptr->raise+=amount;

	/* check for skill advance */
	while (sptr->raise >= Needmarks(group, sptr->cur))
	{
		/* reset mark count */
		sptr->raise-=Needmarks(group, sptr->cur);

		/* uppgrade the skill */
		sptr->cur+=throwdice(sptr->dice_t, sptr->dice_s, 0);

		if (sptr->cur>=MAX_SKILLSCORE)
			sptr->cur=MAX_SKILLSCORE;

		/* if player then show a nice message */
		if (plr==true && !global_initmode)
		{
			msg.vnewmsg(CH_GREEN, "You feel more confident with %s.", nameptr);
		}
	}

	return true;
}

bool skillset::modify_raise(int group, int type, int amount, int addval)
{
	//shortcut for player
	return modify_raise(group, type, amount, addval, true);
}

//returns the needed skillmarks for specific skill group
int skillset::Needmarks(int group, int value)
{
	if (value>skill::Max_Adv-1)
		value=skill::Max_Adv-1;

	int need=SKILL_ADV[value];

	if (group==SKILLGRP_MAGIC)
		need=need / 3;
	else if (group==SKILLGRP_GENERIC)
		need=need / 2;

	return need;
}

void skillset::raiselevel()
{
	//increase skills when gaining a new experience level
	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		skill *s=(*ii);
		modify_raise(s->group, s->type, throwdice(2, 100, 0), false);
	}
}

void skillset::save(Tar_Ball &tb)
{
	//save amount of skills to know how many to restore when loading
	const int amt=(int)skills.size();
	tb.Put(amt);

	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		(*ii)->save(tb);
	}
}

void skillset::load(Tar_Ball &tb)
{
	//how many skills to load
	const int amt=tb.Get_Next_Value();

	for (int t=0; t<amt; t++)
	{
		skill *s=new skill;
		s->load(tb);
		add_skill(s);
	}
}
