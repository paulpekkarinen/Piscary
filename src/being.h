//Legend of Saladir - being.h

//Unit being: Dynamic creature data.

#ifndef BEING_H
#define BEING_H

#include "actor.h"
#include "creature.h"

// level monster structure
struct being : public Actor
{
	int32u id; // monster generation ID
	int base_hp;

	Actor *target; // who is the monster attacking
	Coord spot; //last known target location

	Area myarea; //monster location limits, shopkeepers
	int roomnum; // shopkeeper room index
	int sindex; // special index, ie for shopkeepers guard route
	int last_room; //last room visited

	being();
	~being();

	bool Is_Spotting() const; //monster has a target location
	bool Is_Peaceful();
	bool Gets_Angry_To(being *other);

	void Checkbody();
	void Check_Room(level_type *level);
	void Checkstat(level_type *level);
	void Checkturn(level_type *level);
	void Damage_Message(int damage, int bodypart) override;
	void Gain_Experience(int gain) override;
	void Getangry(level_type *level, Actor *kohde) override;
	bool Handle_Confusion(Condition *cond, int slots) override;
	void Move_To(int dx, int dy);
	bool Noticestuff();
	bool Pick_Up_Item(level_type *level, invnode *itemptr);
	void Regenerate(level_type *level, int ctime, int slots);
	void Set_Target_Spot(int dx, int dy);
	void Shouldflee(level_type *level) override;
	bool Useitems(level_type *level);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, level_type *lvl);
};

#endif
