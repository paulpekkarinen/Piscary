//Legend of Saladir - dice.cpp

//Refactored 22.7.2022 - 6.5.2023 by Paul K. Pekkarinen

#include <chrono>
#include <random>
#include "dice.h"
#include "log.h"

//Random number generator.
struct rng
{
	std::mt19937 mersenne;

	explicit rng(size_t myseed)
		: mersenne(myseed) { }

	int get_min_max(int dmin, int dmax);
};

int rng::get_min_max(int dmin, int dmax)
{
	if (dmin==dmax) return dmin;
	if (dmin>dmax)
	{
		diary.Write("Min value %d was bigger than max %d in RNG.", dmin, dmax);
		return dmin;
	}

	std::uniform_int_distribution<int> dist(dmin, dmax);
	return dist(mersenne);
}

rng *dice;

void destroydice()
{
	delete dice;
}

void initdice()
{
	std::random_device rand_dev;

	//note: using 'entropy' should be tested, because it doesn't seem to work properly
	//in some (most?) implementations
	std::size_t seed;
    if (rand_dev.entropy())
        seed = rand_dev();
    else
	{
        seed = (size_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}

	dice = new rng(seed);
}

int throwdice(int times, int sides, int dmod)
{
	int dvalue=0;

	if(sides==0)
		return dmod;

	dvalue=0;
	for(int i=0; i<times; i++)
		dvalue+=1+RANDU(sides);

	dvalue+=dmod;

	if(dvalue<0)
		dvalue=0;

	return dvalue;
}

int random_number(int rmin, int rmax)
{
	return dice->get_min_max(rmin, rmax);
}

float randrealnum(float rmin, float rmax)
{
	std::uniform_real_distribution<float> dist(rmin, rmax);
	return dist(dice->mersenne);
}

/*
** Return a random number from 0 to limit-1
*/
int RANDU(int limit)
{
	return dice->get_min_max(0, limit-1);
}

bool sometimes()
{
	if(RANDU(100) < 20) return true;
	return false;
}
