// itemdata.cpp - misc. item data

//Refactored 24.10.2022 - 21.9.2025 by Paul K. Pekkarinen

#include "colors.h"
#include "itemdata.h"
#include "items.h"
#include "material.h"

/* food */
food_data foodlist[]=
{
   { "ration", 1000 },
   { "melon",  1000 },
   { "corpse", 60 },
   { "small ration", 1000 },
   { "large ration", 1000 },
   { "iron ration", 1000 },
   {0}
};

/*
 * This list if for descriptions of item categories
 *
 * Also, normally the item char to output will be taken from here
 *
 */
item_category gategories[]=
{
   {'(', "One handed weapons"},
   {')', "Two handed weapons"},
   {'}', "Missile weapons"},
   {'[', "Shields"},
   {']', "Armor"},
   {'-', "Tools"},
   {'&', "Amulets"},
   {'\\', "Wands"},
   {'=', "Rings"},
   {'"', "Books"},
   {'?', "Scrolls"},
   {'!', "Potions"},
   {'{', "Instruments"},
   {'+', "Lights"},
   {'/', "Missile ammunition"},
   {'~', "Bracelets"},
   {'%', "Comestibles"},
   {'$', "Valuables"},
   {'*', "Rocks & gems"},
   {'_', "Specials"},
   {'0', "Containers"},
   {0}
};

/* 
 * Faces for special items 
 * if item type==IS_SPECIAL then the face to output will be taken from
 * this list, must cope with special item indexing!
 */

special_item_outface item_outfaces[]=
{
   { 'O', C_WHITE },   /* SPECIAL_BOULDER */
   { '*', CH_DGRAY },  /* SPECIAL_LROCK    */
   { 'O', CH_YELLOW }, /* SPECIAL_BEEHIVE */
   { '_', CH_WHITE },  /* SPECIAL_ALTAR_LAWFUL */
   { '_', C_WHITE },   /* SPECIAL_ALTAR_NEUTRAL */
   { '_', CH_DGRAY },  /* SPECIAL_ALTAR_CHAOTIC */
};

torch_data torches[]=
{
     { 5,5,
       -2,-2,
       "....."
       ".***."	/* no light torch */
       ".***."
       ".***."
       "....."},

     { 7,7,
       -3,-3,
       "......."
       "..***.."	/* small torch */
       ".*****."
       ".*****."
       ".*****."
       "..***.."
       "......."},

     { 9,9,
       -4,-4,
       "........."
       "...***..."	/* medium torch */
       "..*****.."
       ".*******."
       ".*******."
       ".*******."
       "..*****.."
       "...***..."
       "........."},

     { 11, 11,
       -5, -5,
       "..........."
       "....***...."	/* large torch */
       "...*****..."
       "..*******.."
       ".*********."
       ".*********."
       ".*********."
       "..*******.."
       "...*****..."
       "....***...."
       "..........."},

     {0}
};

valuable_data valuables[]=
{
   { "copper coin", IS_MONEY, MAT_COPPER, 10,   1 },
   { "silver coin", IS_MONEY, MAT_SILVER, 5,  5 },
   { "gold coin",   IS_MONEY, MAT_GOLD, 15,  10 },
 
   { "", 0, 0, 0, 0 } /* end of the list */
};
