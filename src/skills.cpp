/**************************************************************************
 * skill.cpp --                                                           *
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

//Refactored 12.6.2022 - 8.5.2023 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cctype>
#include <cstring>
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
#include "storage.h"

using std::format;
using std::list;
using std::string;

int SKILL_ADV[skillset::Max_Skill_Adv+1];

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

const char *skillgroupnames[]=
{
	"all",
	"weapon",
	"generic",
	"magic",
	NULL
};

skilltype skills_weapon[]=
{
	{"weaponless combat",
	"Weaponless combat skill affects your fighting ability when you fight "
	"without weapons, using your hands and feet only.",
	SKILLAUTO},

	{"daggers",
	"Daggers are small but deadly blades. Apart from their small size, "
	"they can be lethal when used properly. This skill tells how good "
	"you're in handling daggers.",
	SKILLAUTO},

	{"swords",
	"Swords are blades bigger and heavier than daggers. Swords are slower "
	"to use but they issue superior damage to daggers. This skill is for "
	"both one handed and two handed swords.",
	SKILLAUTO},

	{"axes",
	"Axes and battle axes.",
	SKILLAUTO},

	{"blunt weapons",
	"Blunt weapons are weapons of force. Hammers and clubs.",
	SKILLAUTO},

	{"polearms",
	"Polearms are long, sharp poles. Proper use requires good handling.",
	SKILLAUTO},

	{"staffs",
	"Staffs are often used by mages.",
	SKILLAUTO},

	{"bows",
	"Bows and crossbows. These weapons are not melee weapons, they are used "
	"to attack targets from a distance.",
	SKILLAUTO},

	{"crossbows", 0, SKILLAUTO},

	{"throwing",
	"Not only bows can be used to attack distant foes, you can also throw "
	"items with your bare hands.",
	SKILLAUTO},

	{"two handed weapons",
	"If you possess this skill, you gain bonuses when using two handed "
	"weapons. These bonuses depend on the level of this skill.",
	SKILLAUTO},

	{"one handed weapons",
	"If you possess this skill, you gain bonuses when using one handed "
	"weapons. These bonuses depend on the level of this skill.",
	SKILLAUTO},

	{"two weapon combat",
	"Fighting successfully with two weapons, one in both of your hands, "
	"requires good handling of the weapon you're using and also a skill "
	"of using two weapons simultaneously.",
	SKILLAUTO},

	{"shield",
	"A shield or a buckler is a powerful tool in the hands of a skilled "
	"warrior. This skill directly affects your attempts to use shield "
	"items as protective equipment. Train it well and survive.",
	SKILLAUTO},

	{0, 0, 0}
};

skilltype skills_general[]=
{
	{"food gathering",
	"Food gathering skill allows you to get more food out of corpses and "
	"several other sources. The better your knowledge is in this area, "
	"the more you are able to gather food.",
	SKILLAUTO},

	/* magic group skills */
	{"observation",
	"Observation skill is learned at the Magic School of Observation. "
	"The school of Observation teaches ways to examine the real nature "
	"of things. The skill itself affects directly to your Observation "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Observation. It also can affect your ability "
	"to learn new Observation spells.",
	SKILLAUTO},

	{"alteration",
	"Alteration skill is learned at the Magic School of Alteration. "
	"The school teaches magical methods and ways to alter the nature "
	"of things. The skill itself affects directly to your Alteration "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Alteration. It also can affect your ability "
	"to learn new Alteration spells.",
	SKILLAUTO},

	{"mysticism",
	"Mysticism skill is learned at the Magic School of Mysticism. "
	"The school teaches methods to create illusions and temporary "
	"alterations of nature. The skill itself affects directly to "
	"your Mysticism spells. Owning a high skill in this area "
	"produces more successes when casting spells of Mysticism. "
	"It also can affect your ability to learn new Mysticism spells.",
	SKILLAUTO},

	{"destruction",
	"Destruction skill is learned at the Magic School of Destruction. "
	"The school teaches methods to permanently harm or destruct things "
	"of nature. The skill itself affects directly to your Destruction "
	"spells. Owning a high skill in this area produces more successes "
	"when casting spells of Destruction. It also can "
	"affect your ability to learn new Destruction spells.",
	SKILLAUTO},

	{"concentration",
	"Concentration is an important skill for mages and other "
	"spellcasters. It makes possible to cast hard spells more easily. "
	"As your concentration skill increases, it's more likely to get "
	"successes when casting spells.",
	SKILLAUTO},

	{"mana flow",
	"The Mana flow skill tells how closely your mind and the forces of the "
	"Universe are tied together. You regenerate magic points faster on high "
	"levels of this skill.",
	SKILLAUTO},

	{"healing",
	"The skill of healing tells how well you can take care your wounds. "
	"Every being has it's own healing rate, but this skill will improve it if "
	"the skill is in your knowledge.",
	SKILLAUTO},

	{"find weakness",
	"With this skill you can find weak spots from your enemies. High skill "
	"causes more critical hits.",
	SKILLAUTO},

	{"searching",
	"\007The searching skill helps you to find hidden items, traps and "
	"other special locations. The skill will be used when you issue the "
	"search command.\n\n"
	"You can also move with care and be more observant. To do this, turn "
	"the searching mode on. This will slow you down, but increases the "
	"chances for you to find something hidden. "
	"(Use the command \001:s\007).",
	0},

	{"disarm trap",
	"The skill makes it possible to disarm traps whose location is "
	"known to you. If you fail to disarm a trap, it will most probably "
	"activate and cause much unhappiness for you and your body. "
	"You've been warned.",
	0},

	/* skills not used yet below */
	{"Survival", NULL, SKILLAUTO},
	{"Haggling", NULL, SKILLAUTO},
	{"Farming", NULL, SKILLAUTO},
	{"Hunting", NULL, SKILLAUTO},
	{"Preservation", NULL, SKILLAUTO},
	{"Mining", NULL, SKILLAUTO},
	{"Observation", NULL, SKILLAUTO},
	{"Swimming", NULL, SKILLAUTO},
	{"Climbing", NULL, SKILLAUTO},
	{"Hiding", NULL, SKILLAUTO},
	{"Sneaking", NULL, SKILLAUTO},
	{"Concentration", NULL, SKILLAUTO},
	{"Dodging", NULL, SKILLAUTO},
	{0, 0, 0}
};

skilltype skills_magic[]=
{
	{list_spells[SPELL_HEALING].name,
	"Healing spell makes it possible to heal wounded people, creatures "
	"and yourself.",
	0},

	{list_spells[SPELL_TELEPORT].name,
	"Teleporting spell creates a temporary dimension door allowing items "
	"and/or creatures to move between different planes of existence "
	"(locations).\n\n"
	"The power of the teleporting effect depends on your skill level, "
	"higher levels allow you to use teleporting on selected targets as "
	"well as on yourself.",
	0},

	{list_spells[SPELL_IDENTIFY].name,
	"Identify is a very useful and powerfull spell. It allows you to "
	"reveal the real identity of unknown items. It goes without saying "
	"that it can be used to reveal cursed items before using them.",
	0},

	{list_spells[SPELL_BLESS].name,
	"Bless spell allows you to bless items. Blessed item can be more "
	"powerful in some situations. Higher levels of the bless spell allows "
	"you to bless creatures as well.",
	0},

	{list_spells[SPELL_CURSE].name,
	"Curse spell is the exact and violent opposite of bless spell. "
	"It allows you to curse items and/or creatures depending on your "
	"skill level.",
	0},

	{list_spells[SPELL_UNCURSE].name,
	"The spell of remove curse is often a spell of relief. It makes "
	"possible to uncurse items and/or creatures.",
	0},

	{list_spells[SPELL_CONFUZE].name,
	"This spell allows you to temporarily confuze other creatures, "
	"thus making them a better target for your attacks. This cloud of "
	"confuzion can also be used for fleeing the battle area, if so "
	"desired.\n\n"
	"Higher skill level makes it possible to select your target more "
	"freely.",
	0},

	{0, 0, 0}
};

skilltype skills_illegal[]=
{
	{"Illegal skill group!", 0, 0},
	{0, 0, 0}
};

const char *txt_listinstru=
"\007To browse, use \002UP/DOWN \007or \002A/Z \007keys. Press "
"\002?\007 to get skill description. Pressing "
"\002ENTER \007or \002SPACE \007 selects the highlighted skill.\n";

const char *txt_listinstru2=
"\007Tag/untag quick skills with keys\001 0\007 -\001 9\007. Pressing "
"\001C\007 clears all quick skills. "
"\002ESC \007or \002Q \007exits without selection.\n";

const char *txt_skillauto=
"\007This is an \003automatic \007skill. It is used automatically in correct "
"situations if you have learned it. This skill can't be selected.\n";

//*** skill ***

void skill::clamp(int &v)
{
	//test min and max values
	if (v < min)
		v=min;
	if (v > max)
		v=max;
}

void skill::reset(int g, int t, int v)
{
	max=MAX_SKILLSCORE;
	min=MIN_SKILLSCORE;
	group=g;
	type=t;
	cur=v;
	ini=v;
	level=0;
	raise=0;
	dice_t=1;     /* 1d6 */
	dice_s=6;

	clamp(cur);
	clamp(ini);
}

void skill::learn(int v)
{
	cur+=v;
	clamp(cur);
}

void skill::save(Tar_Ball &tb)
{
	tb.Put(group);
	tb.Put(type);

	tb.Put(ini);
	tb.Put(cur);
	tb.Put(min);
	tb.Put(max);

	tb.Put(raise);
	tb.Put(dice_t);
	tb.Put(dice_s);
	tb.Put(level);
}

void skill::load(Tar_Ball &tb)
{
	group=tb.Get_Next_Value();
	type=tb.Get_Next_Value();

	ini=tb.Get_Next_Value();
	cur=tb.Get_Next_Value();
	min=tb.Get_Next_Value();
	max=tb.Get_Next_Value();

	raise=tb.Get_Next_Value();
	dice_t=tb.Get_Next_Value();
	dice_s=tb.Get_Next_Value();
	level=tb.Get_Next_Value();
}

//*** skillset ***

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

void skillset::add_skill(skill *s)
{
	//uncheck, this will be used when creating selected lists
	skills.push_back(s);
}

skill *skillset::add_new_skill(int group, int type, int value)
{
	//don't add the skill if it already exists
	if (find_skill(group, type)!=0)
	{
		diary.Write("Can't add an existing skill!");
		return 0;
	}
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

bool skillset::list_skills(int group)
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
			msg.newmsg(C_WHITE, "You don't know any %s skills.", skillgroupnames[group]);
			return false;
		}
	}

	clear_screen();
	my_center_puts(1, "%s skills you've knowledge of", skillgroupnames[group]);

	my_setcolor(C_YELLOW);
	drawline(2, '-');

	my_setcolor(C_WHITE);
	my_printf("\n");

	for (sitr ii = skills.begin() ; ii != skills.end() ; ++ii)
	{
		skill *s=(*ii);
		int grp=s->group;
		int skl=s->type;

		if (group>0 && group!=grp)
			continue;

		skilltype *skillptr;

		if (grp==SKILLGRP_WEAPON)
			skillptr=&skills_weapon[skl];
		else if (grp==SKILLGRP_MAGIC)
			skillptr=&skills_magic[skl];
		else if (grp==SKILLGRP_GENERIC)
			skillptr=&skills_general[skl];
		else
			skillptr=&skills_illegal[0];

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

	const char *nameptr;

	if (sptr->group==SKILLGRP_WEAPON)
		nameptr=skills_weapon[sptr->type].name;
	else if (sptr->group==SKILLGRP_MAGIC)
		nameptr=skills_magic[sptr->type].name;
	else if (sptr->group==SKILLGRP_GENERIC)
		nameptr=skills_general[sptr->type].name;
	else
		nameptr=skills_illegal[0].name;

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
	return modify_raise(group, type, amount, addval, true); //shortcut for player
}

//returns the needed skillmarks for specific skill group
int skillset::Needmarks(int group, int value)
{
	if (value>Max_Skill_Adv-1)
		value=Max_Skill_Adv-1;

	int need=SKILL_ADV[value];

	if (group==SKILLGRP_MAGIC)
		need=need / 3;
	else if (group==SKILLGRP_GENERIC)
		need=need / 2;

	return need;
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
	int box_sy, box_sx;
	int box_by, box_bx;

	bool endreach=false;
	bool topreach=false;
	bool endoflist=false;
	bool eraseneeded=false;

	int soffs, loffs=0, i, j;
	int ch, stype=0;

	int listcolor;

	box_sy=14;  /* size y */
	box_sx=24;  /* size x */

	box_bx=2;
	box_by=2;

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
	int eoffs=box_sy;

	listcolor=CH_DGRAY;
	skilltype *origlist=0;
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
					if (!endoflist)
						eoffs=loffs+box_sy/2 - 1;
					endoflist=true;
				}

				if (!endreach)
				{
					stype=sptr->type;
					if (sptr->group==SKILLGRP_WEAPON)
						origlist=skills_weapon;
					else if (sptr->group==SKILLGRP_MAGIC)
						origlist=skills_magic;
					else if (sptr->group==SKILLGRP_GENERIC)
						origlist=skills_general;
					else
					{
						origlist=skills_illegal;
						stype=0;
					}

					for (j=0; j<NUM_QUICKSKILLS; j++)
					{
						if (player.qskills[j].group == sptr->group &&
							player.qskills[j].type == sptr->type &&
							player.qskills[j].select != 0)
							skillname=format("({}) ", j);
					}

					if (origlist[stype].flags & SKILLAUTO)
						listcolor=CH_DGRAY;
					else
						listcolor=C_WHITE;

					skillname.append(origlist[stype].name);

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

		stype=curr->type;
		if (curr->group==SKILLGRP_WEAPON)
			origlist=skills_weapon;
		else if (curr->group==SKILLGRP_MAGIC)
			origlist=skills_magic;
		else if (curr->group==SKILLGRP_GENERIC)
			origlist=skills_general;
		else
		{
			origlist=skills_illegal;
			stype=0;
		}

		gotoxy(box_bx+box_sx+2, box_by+box_sy-3);

		set_color(CH_GREEN);
		my_printf("%s skill ", skillgroupnames[curr->group]);

		if (origlist[stype].flags & SPF_ALTERATION)
			my_printf("of alteration");
		if (origlist[stype].flags & SPF_DESTRUCTION)
			my_printf("of destruction");
		if (origlist[stype].flags & SPF_MYSTICISM)
			my_printf("of mysticism");
		if (origlist[stype].flags & SPF_OBSERVATION)
			my_printf("of observation");

		if (origlist[stype].flags & SKILLAUTO)
		{
			my_setcolor(CH_RED);
			my_printf("*Automatic*");
		}
		clrtoeol();

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

		ch=my_getch();

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
			if (origlist[stype].flags & SKILLAUTO)
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
						player.qskills[j].select=0;
					}
				}
				j=ch-'0';
				if (j < NUM_QUICKSKILLS)
				{
					if (player.qskills[j].group == curr->group &&
						player.qskills[j].type == curr->type &&
						player.qskills[j].select != 0)
					{
						player.qskills[j].select = 0;
					}
					else
					{
						player.qskills[j].select = 1;
						player.qskills[j].group = curr->group;
						player.qskills[j].type = curr->type;
					}
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
			if (origlist[stype].flags & SKILLAUTO)
			{
				eraseneeded=true;
				my_wordwraptext(txt_skillauto,
					box_by, SCREEN_LINES, (box_bx+box_sx+2),
					SCREEN_COLS);
			}
			else
				break;
		}

		if (ch==KEY_ESC || ch=='q' || ch=='Q')
		{
			sel=-1;
			break;
		}

		if (ch=='?')
		{
			eraseneeded=true;

			set_color(C_WHITE);
			gotoxy(box_bx+box_sx+2, box_by);

			if (origlist[stype].desc)
			{
				my_wordwraptext(origlist[stype].desc,
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
