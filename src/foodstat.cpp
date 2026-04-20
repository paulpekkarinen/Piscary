//foodstat.cpp

#include "foodstat.h"

foodstat food_status[]=
{
     { "Unknown", 0, 0 },
     { "Fainted!", 0, 0 },
     { "Fainting", 0, 1 },
     { "Starving", 1, 0 },
     { "Hungry", 2, 30 },
     { "", 6, 0 },
     { "Satiated", 10, 0 },
     { "Bloated", 12, 0 },
     { "Unknown", 100, 0 },
     { 0, 0 }
};
