//Legend of Saladir - being.h

//Unit being: Dynamic creature data.

#ifndef BEING_H
#define BEING_H

#include "actor.h"
#include "creature.h"
#include "target.h"

// level monster structure
struct being : public Actor
{
	int32u id; // monster generation ID
	int base_hp;

	Target target; // who is the monster attacking

	Area myarea; //monster location limits, shopkeepers
	int roomnum; // shopkeeper room index
	int sindex; // special index, ie for shopkeepers guard route
	int last_room; //last room visited

	being();
	~being();

	int In_Room(); //which room the being is now (-1 if none)
	bool Is_Peaceful();
	bool Is_Spotting(); //has active target
	bool Gets_Angry_To(being *other);

	void Checkbody();
	void Check_Room(level_type *level);
	void Checkstat(level_type *level);
	void Checkturn(level_type *level);
	void Damage_Message(int damage, int bodypart) override;
	void Gain_Experience(int gain) override;
	void Getangry(level_type *level, Actor *kohde, bool always) override;
	bool Handle_Confusion(Condition *cond, int slots) override;
	void Move_To(const Coord &c);
	bool Noticestuff();
	bool Pick_Up_Item(level_type *level, invnode *itemptr);
	void Regenerate(level_type *level, int ctime, int slots);
	void Shouldflee(level_type *level) override;
	bool Useitems(level_type *level);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, level_type *lvl);
};

#endif
