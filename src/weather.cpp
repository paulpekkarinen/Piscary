/*
 * weather.cpp
 *
 * Legend of Saladir (C) 1997/1998 by Erno Tuomainen
 *
 */

//Refactored 2.5.2021 - 1.4.2026 Paul K. Pekkarinen

#include "dice.h"
#include "gametime.h"
#include "message.h"
#include "options.h"
#include "script.h"
#include "storage.h"
#include "weather.h"
#include "world.h"

Weather::TWeather::TWeather()
	: temperature(20), rain(WEATHER_NORAIN), wind(WEATHER_NOWIND), raintype(false),
	duration(0), ticks(0)
{

}

void Weather::TWeather::save(Tar_Ball &tb)
{
	tb.Put(temperature);
	tb.Put(rain);
	tb.Put(wind);

	tb.Put_Bool(raintype);

	tb.Put(duration);
	tb.Put(ticks);
}

void Weather::TWeather::load(Tar_Ball &tb)
{
	temperature=tb.Get_Next_Value();
	rain=tb.Get_Next_Value();
	wind=tb.Get_Next_Value();

	raintype=tb.Get_Next_Bool();

	duration=tb.Get_Next_Value();
	ticks=tb.Get_Next_Value();
}

//====================================================================

void Weather::save(Tar_Ball &tb)
{
	current.save(tb);
	destination.save(tb);
}

void Weather::load(Tar_Ball &tb)
{
	current.load(tb);
	destination.load(tb);
}

void Weather::passtime(int tickspassed, Gametime & mt)
{
	current.ticks+=tickspassed;

	while(current.ticks >= TIME_1MIN)
	{
		current.ticks-=CONFIGVARS.ticksperminute;

		change_weather(mt);
	}
}

/* Air is hot and the wind is blowing calmly */
/* It's raining softly */
/* It's winding mildly */
/* The wind is storming */
/* The temperature is warm */
/* Air is calm. */

/*
 * change weather conditions,
 *  call this every game minute
 *
 */
void Weather::change_weather(Gametime & mt)
{
	bool doreport;

	modify();

	if(current.duration)
	{
		current.duration--;
		return;
	}

	/* get new weather duration */
	current.duration = WEATHER_MINDURA + RANDU(WEATHER_MAXDURA);

	if(world->Get_Dungeon_Index()==0 || world->Get_Level_Type()==DTYPE_TOWN)
		doreport=true;
	else
		doreport=false;

	change_wind(mt, doreport);
	change_temperature(mt, doreport);
	change_rain(mt, doreport);
}

void Weather::modify()
{
	if(current.wind > destination.wind)
		current.wind--;
	else if(current.wind < destination.wind)
		current.wind++;

	if(current.temperature > destination.temperature)
		current.temperature--;
	else if(current.temperature < destination.temperature)
		current.temperature++;

	if(current.rain > destination.rain)
		current.rain--;
	else if(current.rain < destination.rain)
		current.rain++;
}

bool Weather::change_temperature(Gametime & mt, bool report)
{
	int seas;
	int mintemp;

	seas = mt.report_season();

	if(seas==SEASON_WINTER)
	{
		mintemp=WINTER_MINTEMP;
	}
	else if(seas==SEASON_SPRING)
	{
		mintemp=SPRING_MINTEMP;
	}
	else if(seas==SEASON_SUMMER)
	{
		mintemp=SUMMER_MINTEMP;
	}
	else
	{
		mintemp=AUTUMN_MINTEMP;
	}

	if(RANDU(100) < 20)
	{
		destination.temperature=RANDU(WEATHER_TEMPRANGE) - mintemp;
	}
	else
		return false;

	if(!report)
		return true;

	if(destination.temperature > current.temperature)
		msg.newmsg("It's getting warmer.", C_WHITE);
	else if(destination.temperature < current.temperature)
		msg.newmsg("It's getting colder.", C_WHITE);

	return true;
}

bool Weather::change_wind(Gametime & mt, bool report)
{
	if(RANDU(100) < 20)
	{
		destination.wind=RANDU(WEATHER_MAXWIND);
	}
	else
		return false;

	if(!report)
		return true;

	if(destination.wind > current.wind)
		msg.newmsg("The wind is getting stronger.", C_WHITE);
	else if(destination.wind < current.wind)
		msg.newmsg("The wind is calming a bit.", C_WHITE);

	return true;
}

bool Weather::change_rain(Gametime & mt, bool report)
{
	int seas;
	int prob;

	seas = mt.report_season();

	if(seas==SEASON_WINTER)
	{
		prob=WINTER_RAINPROB;
	}
	else if(seas==SEASON_SPRING)
	{
		prob=SPRING_RAINPROB;
	}
	else if(seas==SEASON_SUMMER)
	{
		prob=SUMMER_RAINPROB;
	}
	else
	{
		prob=AUTUMN_RAINPROB;
	}

	/* should it rain */
	if(RANDU(100) < prob)
	{

		/* get stronger or ? */
		if(RANDU(100) < prob)
		{
			if(destination.rain < WEATHER_MAXRAIN)
				destination.rain++;
		}
		else
			if(destination.rain > WEATHER_NORAIN)
				destination.rain--;
	}
	else
		return false;

	if(!report)
		return true;

	if((current.rain==WEATHER_NORAIN) && (destination.rain!=WEATHER_NORAIN))
	{
		if(current.temperature > 0)
			msg.newmsg("It's starting to rain water.", C_WHITE);
		else
			msg.newmsg("It's starting to snow.", C_WHITE);
	}
	else
	{
		if(current.temperature > 0)
		{
			if(destination.rain > current.rain)
				msg.newmsg("The rain is getting stronger.", C_WHITE);
			else if(destination.rain < current.rain)
				msg.newmsg("The rain is slowing down.", C_WHITE);
		}
		else
		{
			if(destination.rain > current.rain)
				msg.newmsg("The snowing gets stronger.", C_WHITE);
			else if(destination.rain < current.rain)
				msg.newmsg("The snowing is slowing down.", C_WHITE);
		}
	}

	return true;
}

int Weather::Get_Wind_Index()
{
	const int cw=current.wind;
	int rv;
	
	if (cw>WEATHER_MAXWIND-5)
		rv=3;
	else if (cw>WEATHER_MAXWIND/2)
		rv=2;
	else if (cw>WEATHER_MAXWIND/4)
		rv=1;
	else
		rv=0;

	return rv;	
}

void Weather::report()
{
	//note: no code was written for weather report, added wind information, more later

	texts->Index_Message(Script::Wind_Speed, Get_Wind_Index());
}
