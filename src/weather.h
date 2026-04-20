//Legend of Saladir - weather.h

//Unit weather: Weather data and routines.

#ifndef WEATHER_H
#define WEATHER_H

#include "types.h"

class Gametime;

#define WEATHER_NORAIN 0
#define WEATHER_NOWIND 0

/* minimum and maximum temperatures */
#define WEATHER_TEMPRANGE 50

#define WINTER_MINTEMP -45
#define SPRING_MINTEMP -20
#define SUMMER_MINTEMP -5
#define AUTUMN_MINTEMP -12

#define WINTER_RAINPROB 20
#define SUMMER_RAINPROB 15
#define SPRING_RAINPROB 30
#define AUTUMN_RAINPROB 35

/* min and max duration of current weather conditions */
#define WEATHER_MINDURA 20
#define WEATHER_MAXDURA 60

/* maximum wind (m/s) */
#define WEATHER_MAXWIND 35

/* max rain */
#define WEATHER_MAXRAIN 10

class Weather
{
private:
	struct TWeather
	{
		int temperature;
		int rain;
		int wind;
		bool raintype;  /* true = snow, false = water */

		/* how long (minutes) does the current weather stay */
		int duration;
		int ticks;

		TWeather();

		void save(Tar_Ball &tb);
		void load(Tar_Ball &tb);
	};

	/* weather conditions */
	TWeather current;
	TWeather destination;

	int Get_Wind_Index();

	bool change_temperature(Gametime & mt, bool report);
	bool change_wind(Gametime & mt, bool report);
	bool change_rain(Gametime & mt, bool report);
	void change_weather(Gametime & mt);
	void modify();

public:
	void load(Tar_Ball &tb);
	void save(Tar_Ball &tb);

	/* change randomly according to the season */
	void passtime(int tickspassed, Gametime & mt);
	void report();
};

#endif
