//Legend of Saladir - actor.h

//Unit actor: Base class for player and being classes.

#ifndef ACTOR_H
#define ACTOR_H

#include "ailments.h"
#include "attribut.h"
#include "geometry.h"
#include "hitpoint.h"
#include "mondef.h"
#include "pathfind.h"
#include "purse.h"
#include "rig.h"
#include "skills.h"
#include "stats.h"

//Creature's dynamic data, either player or npc.
class Actor
{
public:
	int attackbonus; //increases damage in some attacks
	int bill; //bill to the shop, how much you owe to the shopkeeper
	Ailments conditions; //states like hungry, cursed, poisoned etc.
	int exp; //experience points
	Attribute health; //hit points
	hpslot hpp[HPSLOT_MAX];	//hitpoints pack for each bodypart
	inventory inv; //inventory - items the actor is carrying
	equipment equips; //equipment wielded
	int lastdir; // direction of last move
	int light; //light radius
	Attribute mana; //spell points
	monsterdef m; //monster's basic data
	int movecount; //movement count on every turn
	Pathlist path; //walking path if any
	int regentime; //hitpoints regeneration time
	skillset skills; //skill list
	statpack stat[STAT_ARRAYSIZE]; //stats: strength, dexterity, wisdom etc.
	int tactic; //tactics mode from coward to berzerk
	int timetaken;  // how much "timeunits" used in current turn
	int x; //x position (Coord not used for backwards compatibility reasons...)
	int y; //y position

	Actor();
	virtual ~Actor() { }

	int Calculate_Meleehit(item_def *iptr, Actor *target, int bodypart);
	int Calculate_Time(int ticks);
	bool Can_Carry(int weight); //check weight when picking up new item

	int Experience_Points_Earned(); //how many points you get when killing this creature

	const char *Get_Description();
	int Get_Experience_Level(); //return current experience level
	int Get_Hit_Points();
	int Gain_Levels(); //how many levels gains based on current experience points
	Coord Get_Location();
	const char *Get_Name();
	int Get_Race();
	bool Get_Rangedskill(int *group, int *skill);
	int Get_Special_Id(); //returns special monster id, or zero if none

	bool Has_Time_For(int ticks); //can the creatue do this action/amount of time

	bool Is_Alive(); //has hit points left
	bool Is_At(int dx, int dy); //is at this location on level
	bool Is_At(const Coord &c);
	virtual bool Is_Player() { return false; }
	bool Is_Shopkeeper();

	void Advance_Level(); //set to next experience level
	int Calc_Carryweight(); //how much the actor can carry items
	void Calculate_Totalhp(); //just sum all hpslots together
	void Damage_Issue(int element, int damage, int bodypart);
	virtual void Damage_Message(int damage, int bodypart) = 0;
	void Drop_Item(invnode *in_src, int count, const Coord &c);
	void Drop_Single_Item(invnode *in_src, const Coord &c);
	void Death(); //set death condition
	virtual void Gain_Experience(int gain) = 0;
	virtual void Getangry(level_type *level, Actor *target, bool always) = 0;
	void Handle_Conditions(int slots);
	virtual bool Handle_Confusion(Condition *cond, int slots) = 0;
	bool Regenerate_Health(int slots, int ctime);
	void Reset(); //resets all data to default values
	void Set_Location(int dx, int dy);
	virtual void Shouldflee(level_type *level) = 0;
	bool Skill_Testsuccess(int group, int type);
	void Spend_Time(int ticks);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
