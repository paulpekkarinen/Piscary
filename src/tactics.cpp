//tactics.cpp

#include "tactics.h"

tactics tacticeffects[]=
{
	{ -4, 70, -30, 8, 30 },
	{ -2, 80, -20, 6, 20 },
	{ -1, 90, -10, 4, 10 },
	{  0, 100, 0, 0, 0 },
	{  1, 100, 5, -4, -10 },
	{  2, 100, 10, -6, -20 },
	{  4, 100, 20, -8, -30 },
};

const char *tacticnames[]=
{
   "Coward",
   "Very defensive",
   "Defensive",
   "Normal",
   "Aggressive",
   "Very aggressive",
   "Berzerk",
   0
};
