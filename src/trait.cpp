//trait.cpp

#include "stats.h"
#include "trait.h"

Trait list_traits_neg[]=
{
   { "Ugliness", txt_statnames_short[STAT_CHA], -15,
     0, 0, 0 },
   { "Slow mind", txt_statnames_short[STAT_INT], -5,
     0, 0, 5 },
   { "Slowliness", txt_statnames_short[STAT_DEX], -5,
     0, 0, 0 },

   { 0, 0, 0, 0, 0, 0 } /* end */
};

Trait list_traits_pos[]=
{
   { "Good looks", txt_statnames_short[STAT_CHA], 10,
     0, 0, 0 },
   { "Magic and Power", txt_statnames_short[STAT_WIS], 5,
     0, 15, 0 },
   { "Battle", txt_statnames_short[STAT_STR], 4,
     0, 0, 5 },
   { "Bright mind", txt_statnames_short[STAT_INT], 5,
     0, 5, 0 },
   { "Fast legs", txt_statnames_short[STAT_DEX], 5,
     0, 0, 0 },
   { "Skillfullness", txt_statnames_short[STAT_WIS], 2,
     5, 5, 5 }
};
