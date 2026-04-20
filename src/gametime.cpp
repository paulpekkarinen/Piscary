/**************************************************************************
 * gametime.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 22.04.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored in 2.5.2021 - 30.3.2023 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstring>
#include "dice.h"
#include "game.h"
#include "gametime.h"
#include "lexicon.h"
#include "message.h"
#include "options.h"
#include "output.h"
#include "quote.h"
#include "storage.h"
#include "tactics.h"

using std::string;

const char *time_hourtxt[]=
{
	"It's midnight.",
	"",
	"",
	"",
	"",
	"",
	"It's morning.",
	"",
	"",
	"",
	"",
	"",
	"It's midday.",
	"",
	"",
	"",
	"",
	"",
	"It's evening.",
	"",
	"",
	"",
	"",
	"",
	0,
};

const char *time_months[]=
{
	"New year's dawn",
	"White glimmer",
	"Snowcone",
	"Rapid water",
	"Seed",
	"Sundance",
	"Festival",
	"Harvest",
	"Thunder",
	"Snow",
	"Ice",
	"Old Year's eve",
	0
};

/* seasons */
const char *time_seasons[]=
{
	"winter",
	"spring",
	"summer",
	"autumn",
	0
};

void Gametime::Tgametime::save(Tar_Ball &tb)
{
	tb.Put(min);
	tb.Put(hour);
	tb.Put(day);
	tb.Put(month);
	tb.Put(year);
	tb.Put(ticks);
}

void Gametime::Tgametime::load(Tar_Ball &tb)
{
	min=tb.Get_Next_Value();
	hour=tb.Get_Next_Value();
	day=tb.Get_Next_Value();
	month=tb.Get_Next_Value();
	year=tb.Get_Next_Value();
	ticks=tb.Get_Next_Value();
}

Gametime::Gametime()
{
	eventmessages=false;
	reset();
}

int Gametime::Calculate(int speed, int time, int tactic)
{
	real tm;

	tm=(real)tacticeffects[tactic].spd;
	tm=(tm*time)/100;

	time=(int)tm;

	if (speed<100)
	{
		tm=time;
		tm+=time-(time/100 * speed);

		return (int)tm;
	}
	if (speed>100)
	{
		tm=time;
		tm-=time/100 * (speed-100);

		return (int)tm;
	}

	return time;
}

void Gametime::set_events(bool state)
{
	eventmessages=state;
}

void Gametime::setrandom(int syear)
{
	tm.min=RANDU(TIME_MINUTES);
	tm.hour=RANDU(TIME_HOURS);
	tm.day=RANDU(TIME_DAYS);
	tm.month=RANDU(TIME_MONTHS);
	tm.year=syear;
	tm.ticks=0;
}

void Gametime::reset()
{
	tm.min=0;
	tm.hour=0;
	tm.day=0;
	tm.month=0;
	tm.year=0;
	tm.ticks=0;
}

void Gametime::set(int ryear, int rmonth, int rday, int rhour, int rmin)
{
	tm.year=ryear;
	tm.month=rmonth;
	tm.day=rday;
	tm.hour=rhour;
	tm.min=rmin;
	tm.ticks=0;
}

int Gametime::compare_date(int ryear, int rmonth, int rday)
{
	if (tm.year>=ryear && tm.month>=rmonth && tm.day>=rday)
		return 1;

	return 0;
}

int Gametime::compare_time(int rhour, int rmin)
{
	rhour-=tm.hour;
	rmin-=tm.min;
	if (rhour==0)
		return rmin;
	return rhour;
}

void Gametime::add_minute()
{
   tm.min++;

   if(tm.min>= TIME_MINUTES)
      add_hour();
}

void Gametime::sub_minute()
{
	if (tm.hour>0 || tm.day>0 || tm.month>0 || tm.year>0 || tm.min>0)
		tm.min--;

	if (tm.min < 0)
		sub_hour();
}

void Gametime::add_hour()
{
	tm.min=0;
	bool prec=is_night();

	tm.hour++;

	if (tm.hour >= TIME_HOURS)
		add_day();

	if (eventmessages)
	{
		if (strlen(time_hourtxt[tm.hour])>0)
			msg.newmsg(time_hourtxt[tm.hour]);

		if (prec!=is_night())
		{
			GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

			if (is_night())
				msg.newmsg("The sun goes down.");
			else
				msg.newmsg("The sun rises.");
		}
	}
}

void Gametime::sub_hour()
{
	if (tm.hour>0 || tm.day>0 || tm.month>0 || tm.year>0)
	{
		tm.min=TIME_MINUTES-1;
		tm.hour--;
	}
	else
		tm.hour=0;

	if (tm.hour < 0)
		sub_day();
}

void Gametime::add_day()
{
	tm.hour=0;
	tm.day++;
	if (tm.day >= TIME_DAYS)
		add_month();
}

void Gametime::sub_day()
{
	if (tm.day>0 || tm.month>0 || tm.year>0)
	{
		tm.hour=TIME_HOURS-1;
		tm.day--;
	}
	else tm.day=0;

	if (tm.day < 0)
		sub_month();
}

void Gametime::add_month()
{
	tm.day=0;
	tm.month++;

	if (tm.month >= TIME_MONTHS)
		add_year();
}

void Gametime::sub_month()
{
	if (tm.month>0 || tm.year>0)
	{
		tm.day=TIME_DAYS-1;
		tm.month--;
	}
	else tm.month=0;

	if (tm.month < 0)
		sub_year();
}

void Gametime::add_year()
{
	tm.month=0;
	tm.year++;
}

void Gametime::sub_year()
{
	if (tm.year>0)
	{
		tm.month=TIME_MONTHS-1;
		tm.year--;
	}
	else tm.year=0;
}

void Gametime::add_minutes(int minutes)
{
	while (minutes!=0)
	{
		if (minutes>0)
		{
			add_minute();
			minutes--;
		}
		if (minutes<0)
		{
			sub_minute();
			minutes++;
		}
	}
}

void Gametime::tick(int tickspassed)
{
	tm.ticks+=tickspassed;

	while (tm.ticks >= CONFIGVARS.ticksperminute)
	{
		tm.ticks-=CONFIGVARS.ticksperminute;
		add_minute();
	}
}

void Gametime::tickback(int tickspassed)
{
	tm.ticks+=tickspassed;

	while (tm.ticks >= CONFIGVARS.ticksperminute)
	{
		tm.ticks-=CONFIGVARS.ticksperminute;
		sub_minute();
	}
}

void Gametime::save(Tar_Ball &tb)
{
	tm.save(tb);
}

void Gametime::load(Tar_Ball &tb)
{
	tm.load(tb);
}

bool Gametime::is_night()
{
	if (tm.hour >= 0 && tm.hour < 5)
		return true;

	if (tm.hour >= 20)
		return true;

	return false;
}

int Gametime::report_season()
{
	if (tm.month >= 0 && tm.month < 3)
		return SEASON_WINTER;
	else if (tm.month >= 3 && tm.month < 5)
		return SEASON_SPRING;
	else if (tm.month >= 5 && tm.month < 8)
		return SEASON_SUMMER;
	else if (tm.month >= 8 && tm.month < 11)
		return SEASON_AUTUMN;
	else return SEASON_WINTER;
}

/* Several time display routines */
void Gametime::show_passed()
{
	string s("You've been adventuring ");

	if (tm.year>0)
		s.append(number_of_something(tm.year, "years, "));

	if (tm.month>0)
		s.append(number_of_something(tm.month, "months, "));

	if (tm.day>0)
		s.append(number_of_something(tm.day, "days, "));

	if (tm.hour>0)
		s.append(number_of_something(tm.hour, "hours, "));

	s.append(number_of_something(tm.min, "minutes."));

	my_printf(s.c_str());
}

void Gametime::show_worldtime()
{
	char timestr[100]={0};
	string mtmp(get_ordinal_of_time(tm.day));

	sprintf(timestr, "%02d:%02d, %02d%s of %s in the year of %04d (%s).\n",
		tm.hour, tm.min, tm.day+1, mtmp.c_str(), time_months[tm.month], tm.year,
		time_seasons[report_season()]);

	my_printf(timestr);
}

void Gametime::show_timer(char *msgtxt)
{
	static char timestr[80];

	sprintf(timestr, "%d:%d of %d/%d/%d.", tm.hour, tm.min,
		tm.day, tm.month, tm.year);

	string s;

	if (msgtxt)
		s.append(msgtxt);
	else
		s.append("Timer is now ");

	s.append(timestr);

	msg.newmsg(s.c_str());
}

void Gametime::print_worldtime()
{
	string mtmp(get_ordinal_of_time(tm.day));

	my_printf("%02d%s of %s in the year of %04d at %02d:%02d\n",
		tm.day+1, mtmp.c_str(), time_months[tm.month],
		tm.year, tm.hour, tm.min);
}
