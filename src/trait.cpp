//trait.cpp

#include "stats.h"
#include "trait.h"

Trait list_traits_neg[]=
{
   { "Ugliness", STAT_CHA, -15,
     0, 0, 0 },
   { "Slow mind", STAT_INT, -5,
     0, 0, 5 },
   { "Slowliness", STAT_DEX, -5,
     0, 0, 0 },

   { 0, 0, 0, 0, 0, 0 } /* end */
};

Trait list_traits_pos[]=
{
   { "Good looks", STAT_CHA, 10,
     0, 0, 0 },
   { "Magic and Power", STAT_WIS, 5,
     0, 15, 0 },
   { "Battle", STAT_STR, 4,
     0, 0, 5 },
   { "Bright mind", STAT_INT, 5,
     0, 5, 0 },
   { "Fast legs", STAT_DEX, 5,
     0, 0, 0 },
   { "Skillfullness", STAT_WIS, 2,
     5, 5, 5 }
};
