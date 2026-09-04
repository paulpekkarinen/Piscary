//items.cpp (C) Erno Tuomainen 1997

//Refactored 15.7.2022 - 21.9.2025 by Paul K. Pekkarinen

#include "dice.h"
#include "items.h"
#include "material.h"
#include "message.h"
#include "purse.h"
#include "rig.h"
#include "roleplay.h"
#include "storage.h"

item_def::~item_def()
{
	if (inv != 0)
	{
		delete inv;
		inv=0;
	}
}

void item_def::Age_Food(int luck)
{
	if (type!=IS_FOOD)
		return;

	Decrease_Turns(luck);
}

/****************************
	age (make worse) weapons

   luck is holders luck value

   first a rand value 1-100 is generated, this value is then compared
   to the materials BREAK PROPABILITY value, if rand is lower than
   the material value then item condition might be lowered...

   A second throw against item holders luck is then issued
   if throw (1..100) <= luck, item remains the same

   when COND_BROKEN is reached, function returns COND_BROKEN
      if item breaks completely function returns COND_SMASHED (beyond repair)

   materials with BREAK PROBABILITY of 0 WILL *NOT* get worse*/
int item_def::Age_Weapon(int luck, bool player)
{
	/* if broken weapons breaks once more */
	if (icond==COND_BROKEN)
		return COND_SMASHED;

	/* if prob is smaller than material->breakprob (1-100)
	   then make item condition worse */

	if (Decrease_Turns(luck))
	{
		const char *itemname=name.c_str();
		int shard=0;

		/* substract damage modifiers */
		meldam_mod--;
		misdam_mod--;

		/* if MATSTAT_CANSHARD is set there is
	   a chance that it breaks immendiately */
		if (materials[material].status & MATSTAT_CANSHARD)
		{
			const int prob=50 + (luck - 10);

			if ((1+RANDU(100)) > prob)
			{
				icond=COND_BROKEN;
				shard++;
			}
		}

		const char *matname=materials[material].name;

		if (player && icond<COND_BROKEN && !shard)
			msg.newmsg(CH_BLUE, "Your %s %s seems worse.",
				matname, itemname);

		/* if stat reaches BROKEN condition then it must be removed
	   by the caller of this routine */
		if (icond>=COND_BROKEN)
		{
			if (player)
			{
				if (shard)
					msg.newmsg(CH_RED, "Your %s %s breaks into million pieces.",
						matname, itemname);
				else
					msg.newmsg(CH_RED, "Your %s %s breaks up.",
						matname, itemname);
			}
			melee_ds=0;
			melee_dt=0;
			missi_ds=0;
			missi_dt=0;
			meldam_mod=0;
			misdam_mod=0;

			icond=COND_BROKEN;
			if (shard)
				return COND_SMASHED;
			return COND_BROKEN;
		}
	}

	return 0;
}

void item_def::Clear()
{
	icond=0;
	type=0;
	group=0;

	price=0;
	status=0;
	resist=0;
	special=0;
	weight=1;
	align=NEUTRAL;

	melee_dt=melee_ds=0;
	missi_dt=missi_ds=0;
	meldam_mod=misdam_mod=0;

	turnsleft=-1;
	pmod1=pmod2=pmod3=pmod4=0;
	ac=0;
	dv=0;
	inv=0;
	material=-1;

	ench.Clear();

	inv=0;
}

void item_def::Clone(item_def *c)
{
	name=c->name;
	rname=c->rname;
	sname=c->sname;
	icond=c->icond;
	type=c->type;
	group=c->group;

	price=c->price;
	status=c->status;
	resist=c->resist;
	special=c->special;
	weight=c->weight;
	align=c->align;

	melee_dt=c->melee_dt;
	melee_ds=c->melee_ds;
	missi_dt=c->missi_dt;
	missi_ds=c->missi_ds;
	meldam_mod=c->meldam_mod;
	misdam_mod=c->misdam_mod;

	turnsleft=c->turnsleft;
	pmod1=c->pmod1;
	ac=c->ac;
	dv=c->dv;
	material=c->material;

	pmod2=c->pmod2;
	pmod3=c->pmod3;
	pmod4=c->pmod4;

	ench=c->ench;

	//note: inventory is not cloned, it's set to zero. if the item is
	//inventory this should be checked and created for inventory
	inv=0;
}

bool item_def::Decrease_Turns(int luck)
{
	//some items don't get older or they are already broken
	if (turnsleft<0 || icond>=COND_BROKEN)
		return false;

	if (luck < 10 && luck>=0)
		turnsleft-=1+RANDU(10-luck);
	else
		turnsleft--;

	if (turnsleft<=0)
	{
		icond++;
		turnsleft=materials[material].durability; //new round of turns
		return true; //changed to worse
	}

	return false;
}

void item_def::Save(Tar_Ball &tb)
{
	tb.Put_String(name);
	tb.Put_String(rname);
	tb.Put_String(sname);
	tb.Put(icond);
	tb.Put(type);
	tb.Put(group);

	tb.Put(price);
	tb.Put(status);
	tb.Put(resist);
	tb.Put(special);
	tb.Put(weight);
	tb.Put(align);

	tb.Put(melee_dt);
	tb.Put(melee_ds);
	tb.Put(missi_dt);
	tb.Put(missi_ds);
	tb.Put(meldam_mod);
	tb.Put(misdam_mod);

	tb.Put(turnsleft);
	tb.Put(pmod1);
	tb.Put(ac);
	tb.Put(dv);
	tb.Put(material);

	tb.Put(pmod2);
	tb.Put(pmod3);
	tb.Put(pmod4);

	ench.Save(tb);

	//if this item has no inventory, save zero
	if (inv==0) tb.Put(0);
	else
	{
		//save 1 and the inventory
		tb.Put(1);
		inv->save(tb);
	}
}

void item_def::Load(Tar_Ball &tb)
{
	name=tb.Get_Next_String();
	rname=tb.Get_Next_String();
	sname=tb.Get_Next_String();
	icond=tb.Get_Next_Value();
	type=tb.Get_Next_Value();
	group=tb.Get_Next_Value();

	price=tb.Get_Next_Value();
	status=tb.Get_Next_Unsigned();
	resist=tb.Get_Next_Unsigned();
	special=tb.Get_Next_Unsigned();
	weight=tb.Get_Next_Value();
	align=tb.Get_Next_Value();

	melee_dt=tb.Get_Next_Value();
	melee_ds=tb.Get_Next_Value();
	missi_dt=tb.Get_Next_Value();
	missi_ds=tb.Get_Next_Value();
	meldam_mod=tb.Get_Next_Value();
	misdam_mod=tb.Get_Next_Value();

	turnsleft=tb.Get_Next_Value();
	pmod1=tb.Get_Next_Value();
	ac=tb.Get_Next_Value();
	dv=tb.Get_Next_Value();
	material=tb.Get_Next_Value();

	pmod2=tb.Get_Next_Value();
	pmod3=tb.Get_Next_Value();
	pmod4=tb.Get_Next_Value();

	ench.Load(tb);

	const int i=tb.Get_Next_Value();

	//if no inventory, set pointer to zero
	if (i==0) inv=0;
	else
	{
		//or load the inventory
		inv=new inventory;
		inv->load(tb);
	}
}

/* Weapon list for Saladir
**
** Note that these are only item TEMPLATES. When item is generated to
** level or monster it is randomized in several ways
*/

item_def weapons[]=
{
     { "dagger", "", "", 0, IS_WEAPON1H, WPN_DAGGER,
       1000, 0, 0, 0, 200, NEUTRAL, 1, 4, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "tanto", "", "", 0, IS_WEAPON1H, WPN_DAGGER,
       1000, 0, 0, 0, 500, NEUTRAL, 2, 4, 2, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "staff", "", "", 0, IS_WEAPON2H, WPN_STAFF,
       1500, ITEM_ENCHANTED, 0, 0, 500, NEUTRAL, 1, 8, 1, 2, 0, 0, 
       -1, 0, 0, 0, MAT_IRON, 0, 0, 0,
       {
			0, 0,
			{ 20, 0, 0, 0, 0, 0, 0 }, /* resistances */
            {4, 15, 0, 0, 0, 0, 0, 0, 5},
			0, 0, 0, 0
       }
     },
     { "small axe", "", "", 0, IS_WEAPON1H, WPN_AXE,
       2000, 0, 0, 0, 2000, NEUTRAL, 2, 3, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "short bow", "", "", 0, IS_MISWEAPON, WPN_BOW,
       1000, 0, 0, 0, 500, NEUTRAL, 1, 2, 1, 2, 0, 2, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "short sword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       1700, 0, 0, 0, 2000, NEUTRAL, 2, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "wakizashi", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       1700, 0, 0, 0, 1000, NEUTRAL, 3, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "broadsword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       2000, 0, 0, 0, 1000, NEUTRAL, 4, 3, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "mace", "", "", 0, IS_WEAPON1H, WPN_BLUNT,
       1600, 0, 0, 0, 1500, NEUTRAL, 3, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "long bow", "", "", 0, IS_MISWEAPON, WPN_BOW,
       2000, 0, 0, 0, 1000, NEUTRAL, 1, 2, 1, 2, 0, 4, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "war axe", "", "", 0, IS_WEAPON2H, WPN_AXE,
       3000, 0, 0, 0, 3000, NEUTRAL, 3, 4, 2, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "saber", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       2700, 0, 0, 0, 3000, NEUTRAL, 4, 4, 2, 2, 0, 2, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "morningstar", "", "", 0, IS_WEAPON2H, WPN_BLUNT,
       3500, 0, 0, 0, 3000, NEUTRAL, 4, 4, 3, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "crossbow", "", "", 0, IS_MISWEAPON, WPN_CROSSBOW,
       3500, 0, 0, 0, 1000, NEUTRAL, 1, 2, 1, 2, 0, 6, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "long sword", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       4000, 0, 0, 0, 3000, NEUTRAL, 3, 6, 2, 1, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "battle axe", "", "", 0, IS_WEAPON2H, WPN_AXE,
       4000, 0, 0, 0, 3000, NEUTRAL, 3, 6, 2, 2, 1, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "katana", "", "", 0, IS_WEAPON1H, WPN_SWORD,
       4200, 0, 0, 0, 3000, NEUTRAL, 3, 6, 1, 2, 2, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "claymore", "", "", 0, IS_WEAPON2H, WPN_SWORD,
       4500, 0, 0, 0, 3000, NEUTRAL, 5, 4, 1, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "warhammer", "", "", 0, IS_WEAPON2H, WPN_BLUNT,
       5000, 0, 0, 0, 3000, NEUTRAL, 5, 4, 3, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "dai-katana", "", "", 0, IS_WEAPON2H, WPN_SWORD,
       8000, 0, 0, 0, 3000, NEUTRAL, 4, 6, 2, 2, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },

     /* missiles */
     { "arrow", "", "", 0, IS_MISSILE, WPN_MISSILE_1,
       8, 0, 0, 0, 20, NEUTRAL, 1, 2, 2, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "bolt", "", "", 0, IS_MISSILE, WPN_MISSILE_2,
       16, 0, 0, 0, 40, NEUTRAL, 1, 2, 3, 3, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "rock", "", "", 0, IS_MISSILE, WPN_MISSILE_3,
       4, 0, 0, 0, 250, NEUTRAL, 1, 2, 2, 2, 0, 0, -1, 0, 0, 0, MAT_STONE, 0, 0, 0 },

    {"", "", "", 0} //end of the list
};

/* armor templates */
item_def armor[]=
{
     { "helmet", "", "", 0, IS_ARMOR, ARM_HELMET,
       1000, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0},
     { "left pauldron", "", "", 0, IS_ARMOR, ARM_LHANDARM,
       1500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "right pauldron", "", "", 0, IS_ARMOR, ARM_RHANDARM,
       1500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "cuirass", "", "", 0, IS_ARMOR, ARM_BODY,
       2000, 0, 0, 0, 2000, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "gauntlets", "", "", 0, IS_ARMOR, ARM_GLOVES,
       500, 0, 0, 0, 200, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "greaves", "", "", 0, IS_ARMOR, ARM_LEGARM,
       1000, 0, 0, 0, 1000, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "boots", "", "", 0, IS_ARMOR, ARM_BOOTS,
       500, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "left heavy pauldron", "", "", 0, IS_ARMOR, ARM_LHANDARM,
       2000, 0, 0, 0, 1400, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "right heavy pauldron", "", "", 0, IS_ARMOR, ARM_RHANDARM,
       2000, 0, 0, 0, 1400, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "crown", "", "", 0, IS_ARMOR, ARM_HELMET,
       600, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "heavy cuirass", "", "", 0, IS_ARMOR, ARM_BODY,
       3000, 0, 0, 0, 4000, NEUTRAL,
       1, 2, 1, 2, -2, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "spiked gauntlets", "", "", 0, IS_ARMOR, ARM_GLOVES,
       800, 0, 0, 0, 200, NEUTRAL,
       1, 2, 3, 2, 0, 0,
       -1, 0, 2, 0, MAT_IRON, 0, 0, 0 },
     { "heavy greaves", "", "", 0, IS_ARMOR, ARM_LEGARM,
       1500, 0, 0, 0, 2400, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "running shoes", "Shoes of haste", "", 0, IS_ARMOR, ARM_BOOTS,
       1000, 0, 0, 0, 500, NEUTRAL,
       1, 2, 1, 2, 0, 0,
       -1, 2, 0, 0, MAT_IRON, 0, 0, 0 },
     { "buckler", "", "", 0, IS_SHIELD, ARM_SHIELD,
       200, 0, 0, 0, 500, NEUTRAL,
       1, 2, 2, 2, 0, 0,
       -1, 0, 1, 0, MAT_IRON, 0, 0, 0 },
     { "small shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       500, 0, 0, 0, 800, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 2, 0, MAT_IRON, 0, 0, 0 },
     { "medium shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       1000, 0, 0, 0, 1200, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 3, 0, MAT_IRON, 0, 0, 0 },
     { "large shield", "", "", 0, IS_SHIELD, ARM_SHIELD,
       1500, 0, 0, 0, 2000, NEUTRAL,
       1, 1, 2, 2, 0, 0,
       -1, 0, 4, 0, MAT_IRON, 0, 0, 0 },

     { "cloak", "", "", 0, IS_ARMOR, ARM_CLOAK,
       200, 0, 0, 0, 400, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, C_WHITE },
     { "hooded cloak", "", "", 0, IS_ARMOR, ARM_CLOAK,
       220, 0, 0, 0, 500, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_DGRAY },
     { "shirt", "", "", 0, IS_ARMOR, ARM_SHIRT,
       200, 0, 0, 0, 400, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_RED },
     { "pants", "", "", 0, IS_ARMOR, ARM_PANTS,
       200, 0, 0, 0, 700, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_BLUE },
     { "sandals", "", "", 0, IS_ARMOR, ARM_BOOTS,
       200, 0, 0, 0, 500, NEUTRAL,
       1, 1, 1, 1, 0, 0,
       -1, 0, 1, 0, MAT_FABRIC, 0, 0, CH_BLUE },

    {"", "", "", 0} //end of the list
};

/* 
 * template for scroll item, when a scroll is created
 * the data is first copied from here to initialize the item 
 *
 * Most important "fields" for scroll item are:
 * 'type'  = indicates that the item is a scroll
 * 'group' = indicates the group of scroll (ie. SCROLLGROUP_MAGIC)
 * 'pmod1' = scroll "effect" or spell 
 * 'pmod2' = the recorded skill value for a spell
 *
 */
item_def templ_scroll=
{
   "scroll", "", "", 0, IS_SCROLL, 0,
   800, 0, 0, 0, 100, NEUTRAL, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, MAT_PAPYRUS,
   0, 0, CH_WHITE
};

item_def SPECIAL_ITEMS[]=
{
   /* rocks */
   { "large stone boulder", "", "", 0, IS_SPECIAL, SPECIAL_BOULDER,
     10, ITEM_NOTPASSABLE, 0, 0, 800000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 0, 0, 0, MAT_STONE, 0, 0, 0 },
   { "large rock", "", "", 0, IS_SPECIAL, SPECIAL_ROCK,
     10, ITEM_NOTPASSABLE, 0, 0, 450000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 0, 0, 0, MAT_STONE, 0, 0, 0 },

/*
   { "large beehive", "", "", 0, IS_SPECIAL, SPEC_BEEHIVE,
     100, 0, 0, 0, 1000000, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     -1, 0, 0, 0, MAT_FABRIC, 0, 'O', CH_CYAN },
*/
    {"", "", "", 0} //end of the list
};

item_def miscitems[]=
{
   /* lanterns */
   { "small torch", "", "", 0, IS_LIGHT, LIGHT_SMALL,
     200, 0, 0, 0, 1200, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     400000, 3, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "torch", "", "", 0, IS_LIGHT, LIGHT_MEDIUM,
     300, 0, 0, 0, 1800, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     600000, 5, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "large torch", "", "", 0, IS_LIGHT, LIGHT_LANTERN,
     400, 0, 0, 0, 2500, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     600000, 7, 0, 0, MAT_WOOD, 0, 0, 0 },
   { "lantern", "", "", 0, IS_LIGHT, LIGHT_LANTERN,
     1000, 0, 0, 0, 1200, NEUTRAL,
     1, 4, 2, 2, 0, 0,
     800000, 10, 0, 0, MAT_WOOD, 0, 0, 0 },

   /* food items here
    *  ! food items have pmod1 as a percentage multiplier for
    *    food nutrition. 100 is normal
    */
   { "small ration", "", "", 0, IS_FOOD, FOOD_RATION,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "melon", "", "", 0, IS_FOOD, FOOD_MELON,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 110, 0, 0, MAT_ORCISH, 0, 0, 0 }, 
   { "large bread", "", "", 0, IS_FOOD, FOOD_RATION,
     800, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 120, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "cream pie", "", "", 0, IS_FOOD, FOOD_RATION,
     600, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_SILVER, 0, 0, 0 },
   { "carrot", "", "", 0, IS_FOOD, FOOD_RATION,
     500, 0, 0, 0, 500, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 150, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "large ration", "", "", 0, IS_FOOD, FOOD_RATION,
     1000, 0, 0, 0, 5000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 100, 0, 0, MAT_GOLD, 0, 0, 0 },
   { "iron ration", "", "", 0, IS_FOOD, FOOD_RATION, /* 2 x nutr */
     1400, 0, 0, 0, 2000, NEUTRAL,
     0, 0, 1, 1, 0, 0,
     0, 200, 0, 0, MAT_GOLD, 0, 0, 0 },

   { "ring", "", "", 0, IS_RING, 0,
     2000, 0, 0, 0, 50, NEUTRAL,
     0, 0, 0, 0, 0, 0,
     -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },   

   /* containers */
   { "bag", "", "", 0, IS_CONTAINER, 0,
     200, 0, 0, 0, 1500, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_LEATHER, 0, 0, 0 },
   { "pouch", "", "", 0, IS_CONTAINER, 0,
     100, 0, 0, 0, 800, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_LEATHER, 0, 0, 0 },
   { "chest", "", "", 0, IS_CONTAINER, 0,
     250, 0, 0, 0, 8500, NEUTRAL,
     1, 2, 1, 2, 0, 0,
     -1, 0, 0, 0, MAT_WOOD, 0, 0, 0 },
    
    {"", "", "", 0} //end of the list
};

item_def missiles[]=
{
     { "arrow", "", "", 0, IS_MISSILE, WPN_MISSILE_1,
       8, 0, 0, 0, 20, NEUTRAL, 1, 2, 1, 4, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
     { "bolt", "", "", 0, IS_MISSILE, WPN_MISSILE_2,
       16, 0, 0, 0, 40, NEUTRAL, 1, 2, 1, 6, 0, 0, -1, 0, 0, 0, MAT_IRON, 0, 0, 0 }
};

item_def artifacts[]=
{
     { "huge battle axe", "Great battle axe", "Thoron", 0, IS_WEAPON1H, WPN_AXE,
       500000, ITEM_ARTIFACT,
       0, 0, 300, NEUTRAL, 6, 6, 3, 4, 4, 0, -1, 4, 0, 0, MAT_ORCISH, 0, 0, 0 },
/*
     { "shiny katana", "Katana of resistance", "Em'ladir", 0, 
     IS_WEAPON2H, WPN_SWORD,
       70000, ITEM_ARTIFACT | ITEM_MODDEX,
       RES_FIRE | RES_ACID | RES_POISON | RES_COLD, 0, 300, NEUTRAL,
       5, 6, 1, 2, 4, 0, -1, 4, 0, 0, MAT_PLATINIUM },
     { "lantern", "Lantern", "Tha'nthol's light", 0, IS_LIGHT, LIGHT_LANTERN,
       20000, ITEM_ARTIFACT, 0, 0, 1200, NEUTRAL,
       1, 4, 2, 2, 0, 0,
       -1, 0, 0, 0, MAT_IRON, 0, 0, 0 },
*/
    {"", "", "", 0} //end of the list
};

/*
struct item_def potions[]=
{
{ "cure blindness", "", POTION, MISSILES,
  ITEM_OK, 0, CURE_BLINDNESS, 5, NEUTRAL, 1, 1, 0, 0, -1, 0 },
{ "blindness", "", POTION, MISSILES,
       ITEM_OK, BLINDNESS, 0, 5, NEUTRAL, 1, 1, 0, 0, -1, 0 },

{ "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }	// end of list
};
*/

const char *outfits[]=
{
     "heavy",
     "light",
     "nice",
     "rusty",
     "clear",
     "blazing",
     "blinking",
     "hazy",
     "smoky",
     "wobbly",
     "dusty",
     "synthy",
     "ancient",
     "watery",
     "shady",
     NULL
};
