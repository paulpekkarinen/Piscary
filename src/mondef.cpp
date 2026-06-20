//Legend of Saladir - mondef.cpp - monsterdef data

//Refactored 15.7.2022 - 27.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "avatar.h"
#include "body.h"
#include "classes.h"
#include "creature.h"
#include "dice.h"
#include "gametime.h"
#include "lexicon.h"
#include "mondef.h"
#include "names.h"
#include "roleplay.h"
#include "specmon.h"
#include "storage.h"

using std::string;

monsterdef npc_list[] =
{
	{"dull looking chairman", "Gill Bates", NPC_BILLGATES, 48000, 4, CHAOTIC, NPC_BILLGATES,
		RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
		{50, 50, 30, 50, 50, 50, 50, 20, BASE_SPEED}, 0},
	{"scar faced knight", "Sparhawk", NPC_SPARHAWK, 121000, 6, LAWFUL, NPC_SPARHAWK,
		RACE_HIGHELF, CLASS_KNIGHT, SEX_MALE, 0, BEHV_FRIENDLY,
		{50, 50, 56, 70, 50, 50, 50, 20, BASE_SPEED+1}, 0},
	{"very corrupted adventurer", "Thomas Biskup", NPC_THOMAS, 77500, 12, LAWFUL, NPC_THOMAS,
		RACE_HUMAN, CLASS_MAGE, SEX_MALE, 0, BEHV_FRIENDLY,
		{50, 60, 53, 50, 50, 50, 50, 20, BASE_SPEED+2}, 0},
	{"beautiful cat lady", "Natasha", NPC_NATASHA, 44500, 8, NEUTRAL, NPC_NATASHA,
		RACE_CATHUMAN, CLASS_MAGE, SEX_FEMALE, 0, BEHV_FRIENDLY,
		{45, 55, 55, 55, 68, 60, 50, 25, BASE_SPEED+3}, 0},
	{ "", "" }
};

monsterdef shopkeeper_list[] =
{
     {"merchant", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
    {67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1}, MST_SHOPKEEPER },
     {"trader", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
    {67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1}, MST_SHOPKEEPER },
     {"dealer", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
    {67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1}, MST_SHOPKEEPER },
     {"shopkeeper", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
    {67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1}, MST_SHOPKEEPER },
     {"storekeeper", "", 0, 48000, 24, NEUTRAL, 0, RACE_HUMAN, CLASS_MERCHANT, SEX_MALE, 0, 0,
    {67, 64, 55, 75, 46, 55, 65, 20, BASE_SPEED+1}, MST_SHOPKEEPER },
	{ "", "" , 0}
};

void monsterdef::playerize()
{
	name="Moonlite";
	desc="the adventurer";
	level=1;
	align=RANDU(LAWFUL_E);
	gender=SEX_MALE;
	race=RACE_HUMAN;
	mclass=CLASS_FIGHTER;
	weight=75000;

	//has to know itself to return player's name as the name,
	//otherwise 'desc' is returned as name
	status|=MST_KNOWN;
}

void monsterdef::randomize(Npcrace *stdmon, int monrace)
{
	desc=stdmon->name;
    name.clear(); //clear just in case, if resetting
    longdesc=0;
	special=0;
    weight=stdmon->weight;

	/* set alignment */
	if (stdmon->align==RANDALIGN)
		align=RANDU(LAWFUL);
	else
		align=stdmon->align;

    race=monrace;

	/* class if monster is not animal */
	if (!(stdmon->behave & BEHV_ANIMAL))
	{
		mclass=1+RANDU(mucho.num_classes-1);

		random_name(name, CNAME_MAX);
		uppercase_first_letter(name);
	}
    else
    {
        mclass=0;
    }

	attitude=stdmon->attitude;
	behave=stdmon->behave;
	status=stdmon->status;

	/* this is now obsolete because of new statpack (see below) */
	/* done just for safety */
	//note: oh, really? Well, that's for that refactoring then...
	stats=stdmon->stats;

	/* generate sex for the creature */
	if (RANDU(100) > 50)
		gender=SEX_MALE;
    else
		gender=SEX_FEMALE;

	/* decide some level for the monster based on player's experience level */
	int lev=RANDU(4) - 2 + player.Get_Experience_Level();
	if (lev<1)
		lev=1;

	level=lev;
}

void monsterdef::steer_alignment(char dir, int amount)
{
	int nal=align;

	if (dir=='c' || dir=='C')
	{
		nal-=amount;
		if (nal < CHAOTIC)
			nal=CHAOTIC;
	}
	else if (dir=='N' || dir=='n')
	{
		if (nal < NEUTRAL)
		{
			nal+=amount;
			if (nal > NEUTRAL)
				nal=NEUTRAL;
		}
		else if (nal > NEUTRAL)
		{
			nal-=amount;
			if (nal < NEUTRAL)
				nal=NEUTRAL;
		}

	}
	else if (dir=='l' || dir=='L')
	{
		nal+=amount;
		if (nal > LAWFUL)
			nal=LAWFUL;
	}

	align=nal;
}

const char *monsterdef::Get_Name()
{
	if (status & MST_KNOWN)
	{
		if (name.size()>0)
			return name.c_str();
	}

	if (desc.size()>0)
		return desc.c_str();

	return npc_races[race].name;
}

void monsterdef::Save(Tar_Ball &tb)
{
	tb.Put_String(desc);
	tb.Put_String(name);
	tb.Put(longdesc);

	tb.Put(weight);
	tb.Put(level);
	tb.Put(align);
	tb.Put(special);

	tb.Put(race);
	tb.Put(mclass);
	tb.Put(gender);
	tb.Put(attitude);
	tb.Put(behave);

	stats.Save(tb);
	tb.Put(status);
}

void monsterdef::Load(Tar_Ball &tb)
{
	desc=tb.Get_Next_String();
	name=tb.Get_Next_String();
	longdesc=tb.Get_Next_Value();

	weight=tb.Get_Next_Value();
	level=tb.Get_Next_Value();
	align=tb.Get_Next_Value();
	special=tb.Get_Next_Value();

	race=tb.Get_Next_Value();
	mclass=tb.Get_Next_Value();
	gender=tb.Get_Next_Value();
	attitude=tb.Get_Next_Value();
	behave=tb.Get_Next_Unsigned();

	stats.Load(tb);
	status=tb.Get_Next_Unsigned();
}
