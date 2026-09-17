//Legend of Saladir - being.h

//Unit being: Dynamic creature data.

#ifndef BEING_H
#define BEING_H

#include "actor.h"
#include "creature.h"
#include "target.h"

struct Area;

// level monster structure
class being : public Actor
{
private:
	friend class Display;
	friend class Roleplay;

	int32u id; // monster generation ID
	int base_hp;

	Target target; // who is the monster attacking

	int last_room; //last room visited
	Area myarea; //monster location limits, shopkeepers
	int roomnum; // shopkeeper room index

	void Initialize(int sp);

public:
	being() { }
	being(int sp);
	being(int sp, const monsterdef &mon);
	being(int sp, const monsterdef &mon, const Area &ar);
	~being();

	bool Can_Attack(being *b);
	int In_Room(); //which room the being is now (-1 if none)
	bool Is_Peaceful();
	bool Is_Spotting(); //has active target
	bool Gets_Angry_To(being *other);
	int32u Get_Id() const { return id; }
	int Get_Room() const { return roomnum; }
	Actor *Get_Target(); //returns creature target if any
	Coord Get_Target_Location();

	void Become_Homeowner(level_type *level, int room_id, Area &ar);
	void Checkbody();
	void Check_Room(level_type *level);
	void Checkstat(level_type *level);
	void Checkturn(level_type *level);
	void Clear_Target();
	void Damage_Message(Damage &dmg) override;
	void Gain_Experience(int gain) override;
	void Getangry(level_type *level, Actor *kohde, bool always) override;
	void Handle_Confusion(Condition *cond) override;
	void Move_To(const Coord &c);
	bool Noticestuff();
	bool Pick_Up_Item(level_type *level, invnode *itemptr);
	void Pursue_Creature(being *b);
	void Pursue_Item(invnode *i, const Coord &c);
	void Regenerate(level_type *level, int ctime, int slots);
	void Remove_Target(being *b);
	void Set_Last_Room(int r);
	void Shouldflee(level_type *level) override;
	bool Useitems(level_type *level);

	void Show_Debug_List_Info();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb, level_type *lvl);
};

#endif
