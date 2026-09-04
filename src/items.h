//Legend of Saladir - items.h

//Unit items: Static and dynamic data of items.

#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include "enchant.h"
#include "hitpoint.h"

#define WEIGHT_KILO 1000

/** item categories
 **
 ** If you change these, remember to revise the item category descriptions
 **/
#define IS_WEAPON1H		0     /* weapon groups below */
#define IS_WEAPON2H		1
#define IS_MISWEAPON	2
#define IS_SHIELD		3
#define IS_ARMOR		4     /* armor groups below */
#define IS_TOOL			5
#define IS_AMULET		6
#define IS_WAND			7
#define IS_RING			8
#define IS_BOOK			9
#define IS_SCROLL		10
#define IS_POTION		11
#define IS_INSTRU		12
#define IS_LIGHT		13
#define IS_MISSILE		14
#define IS_BRACELET     15
#define IS_FOOD			16
#define IS_MONEY		17
#define IS_ROCKGEM		18
#define IS_SPECIAL      19
#define IS_CONTAINER    20    /* bags, chests etc */
#define NUMOFITEMGROUPS 21

#define ISMG_MISCITEM	0xff  /* multigroup */

#define SPECIAL_BOULDER 0
#define SPECIAL_ROCK    1
#define SPECIAL_BEEHIVE 2
#define SPECIAL_ALTAR   3

#define LIGHT_MAXNUM	4
#define LIGHT_NOLIGHT	0
#define LIGHT_SMALL		1
#define LIGHT_MEDIUM	2
#define LIGHT_LANTERN	3

// weapon groups SAME as weapon skills!!!
#define WPN_HAND		0	// hand combat */
#define WPN_DAGGER		1
#define WPN_SWORD		2	// swords (not daggers)
#define WPN_AXE			3
#define WPN_BLUNT		4
#define WPN_POLEARM		5
#define WPN_STAFF		6
#define WPN_BOW			7	// item is a bow weapon
#define WPN_CROSSBOW	8
#define WPN_MISSILE_1	9  // missile item for bows
#define WPN_MISSILE_2	10  // missile item for crossbows
#define WPN_MISSILE_3	11
#define WPN_GENMISSILE	12  // missile

#define WEAPONS_DAGGER 0
#define WEAPONS_TANTO 1
#define WEAPONS_STAFF 2
#define WEAPONS_SMALLAXE 3
#define WEAPONS_SHORTBOW 4
#define WEAPONS_SHORTSWORD 5
#define WEAPONS_WAKIZASHI 6
#define WEAPONS_BROADSWORD 7
#define WEAPONS_MACE 8
#define WEAPONS_LONGBOW 9
#define WEAPONS_WARAXE 10
#define WEAPONS_SABER 11
#define WEAPONS_MORNINGSTAR 12
#define WEAPONS_CROSSBOW 13
#define WEAPONS_LONGSWORD 14
#define WEAPONS_BATTLEAXE 15
#define WEAPONS_KATANA 16
#define WEAPONS_CLAYMORE 17
#define WEAPONS_WARHAMMER 18
#define WEAPONS_DAIKATANA 19
#define WEAPONS_ARROW 20
#define WEAPONS_BOLT 21
#define WEAPONS_ROCK 22

#define ARMOR_HELMET 0
#define ARMOR_PAULDRONL 1
#define ARMOR_PAULDRONR 2
#define ARMOR_CUIRASS 3
#define ARMOR_GAUNTLETS 4
#define ARMOR_GREAVES 5
#define ARMOR_BOOTS 6
#define ARMOR_PAULDRONHL 7
#define ARMOR_PAULDRONHR 8
#define ARMOR_CROWN 9
#define ARMOR_CUIRASSH 10
#define ARMOR_GAUNTLESSPIKED 11
#define ARMOR_GREAVESH 12
#define ARMOR_RUNNINGSHOES 13
#define ARMOR_BUCKLER 14
#define ARMOR_SHIELDSM 15
#define ARMOR_SHIELDMD 16
#define ARMOR_SHIELDLG 17
#define ARMOR_CLOAK 18
#define ARMOR_HOODEDCLOAK 19
#define ARMOR_SHIRT 20
#define ARMOR_PANTS 21
#define ARMOR_SANDALS 22

#define MISCI_TORCHSM    0
#define MISCI_TORCHMD    1
#define MISCI_TORCHLG    2
#define MISCI_LANTERN    3
#define MISCI_RATIONSM   4
#define MISCI_MELON      5
#define MISCI_BREAD      6
#define MISCI_PIE        7
#define MISCI_CARROT     8
#define MISCI_RATIONLG   9
#define MISCI_IRONRATION 10
#define MISCI_RING       11
#define MISCI_BAG        12
#define MISCI_POUCH      13
#define MISCI_CHEST      14

/* food types */
#define FOOD_RATION		0x0002
#define FOOD_MELON      0x0001
#define FOOD_CORPSE     0x0000
#define FOOD_RATIONS    0x0003
#define FOOD_RATIONL    0x0004
#define FOOD_RATIONI    0x0005

#define MIS_ARROW		0x00010000 // can be used in bows
#define MIS_BOLT		0x00020000 // can be used in crossbows

/* magic group scroll effects */
/* see magic.h for spells */

/* *************************************************************** */

#define ITEM_CURSED			0x00000008
#define ITEM_BLESSED		0x00000020
#define ITEM_ENCHANTED		0x04000000 /* item is enchanted */
#define ITEM_NOTPASSABLE	0x08000000 /* occupies whole location */
#define ITEM_UNPAID			0x10000000 /* unpaid item */
#define ITEM_GENERATED		0x20000000 /* for artifacts, set if generat */
#define ITEM_ARTIFACT		0x40000000 /* true if artifact */
#define ITEM_IDENTIFIED		0x80000000 /* true if item is known */
#define ITEM_FLAGS 8 //number of item flags

/* conditions */
#define COND_NEW	0
#define COND_GOOD	1
#define COND_USED	2
#define COND_WORN	3
#define COND_BAD	4
#define COND_BROKEN	5
#define COND_SMASHED 6 //unrepairable

/* money indexes to the valuables array */
#define MONEY_COPPER	0
#define MONEY_SILVER	1
#define MONEY_GOLD		2

#define ITEM_NAMEMAX    40

#define EFLG_PERMCURSED 0x00000001 /* permanently cursed */

// item definition
struct item_def
{
	std::string name;	// name when not identified
	std::string rname;	// name when identified
	std::string sname;	// if named item, here is the name
	int icond;		// item damage status
	int type;		// item type, weapon, armor, tool etc..
	int group;	// item group (weapons/armor/food mainly)

	int price;	// base price in copper coins
	int32u status;	// item status (broken, rusty etc.)
	int32u resist;	// resistances
	int32u special;	// other special attributes
	int weight;	// item weight (1000 is 1kg)
	int align;	// item alignment

	int melee_dt;	// dice times
	int melee_ds;	// melee damage dice sides
	int missi_dt;	// melee damage dice
	int missi_ds;
	int meldam_mod;	// melee damage modifier
	int misdam_mod;	// melee damage modifier

	int turnsleft;	// remaining turns until breaks, -1 is infinite
	int pmod1;	// item modifier
	int ac;	// armor class modifier
	int dv;	// defense value
	int material;	// item material

	int pmod2;	// item modifier
	int pmod3;	// item modifier
	int pmod4;	// item modifier

	/* items enchantments */
	enchantments ench;

	/* inventory for containers */
	inventory *inv;

	//the compound initializing of 'staff' can't take any kind of
	//constructor, otherwise it would probably require full constructor

	~item_def();

	bool Decrease_Turns(int luck);

	void Age_Food(int luck);
	int Age_Weapon(int luck, bool player);
	void Clear(); //clears everything to default values
	void Clone(item_def *c); //clone data from 'c'

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

extern item_def weapons[];
extern item_def armor[];
extern item_def artifacts[];
extern item_def miscitems[];
extern item_def templ_scroll;
extern const char *outfits[];
extern item_def SPECIAL_ITEMS[];

#endif
