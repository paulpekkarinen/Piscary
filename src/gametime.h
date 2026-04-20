//gametime.h

//Unit gametime: Time datatype and definitions.

#ifndef GAMETIME_H
#define GAMETIME_H

#include "types.h"

class Tar_Ball;

#define TIME_DAYS    29
#define TIME_MONTHS  12
#define TIME_MINUTES 60
#define TIME_HOURS   24

/* how many ticks in minute */
#define TIME_1MIN 2800

/* season numbers */
#define SEASON_WINTER 0
#define SEASON_SPRING 1
#define SEASON_SUMMER 2
#define SEASON_AUTUMN 3

#define BASE_SPEED 100	/* monster base speed */
#define BASE_TIMENEED 1000

//time needed by certain actions, this is the 'ticks' parameter in Actor class
#define TIME_BADLEGS		800
#define TIME_MOVEAROUND		800
#define TIME_OPENDOOR		600
#define TIME_CLOSEDOOR		600
#define TIME_PICKUP			600
#define TIME_SEARCH			900
#define TIME_AUTOSEARCH		400
#define TIME_MELEEATTACK	600	/* per hand */
#define TIME_EATKILO		400 /* per kilo */
#define TIME_DROPITEM		400
#define TIME_MISSILEATTACK	600
#define TIME_USESKILL		800

class Gametime
{
private:
	struct Tgametime
	{
		int min;
		int hour;
		int day;
		int month;
		int year;
		int ticks;

		void save(Tar_Ball &tb);
		void load(Tar_Ball &tb);
	};

	friend class Weather;

	Tgametime tm;
	bool eventmessages;

	void add_minute();
	void add_hour();
	void add_day();
	void add_month();
	void add_year();

	void sub_minute();
	void sub_hour();
	void sub_day();
	void sub_month();
	void sub_year();

public:
	Gametime();

	bool is_night();

	static int Calculate(int speed, int time, int tactic);
	void set_events(bool state);
	void setrandom(int data);
	void set(int ryear, int rmonth, int rday, int rhour, int rmin);
	void reset();
	int compare_time(int rhour, int rmin);
	int compare_date(int ryear, int rmonth, int rday);
	void add_minutes(int minutes);
	void tick(int tickspassed);
	void tickback(int tickspassed);
	void show_passed();
	void show_worldtime();
	void show_timer(char *msg);
	void print_worldtime();
	int report_season();

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
