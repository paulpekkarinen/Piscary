//Legend of Saladir - rig.h

//Unit rig: current equipment of a creature

#ifndef RIG_H
#define RIG_H

#include "gearslot.h"

struct playerinfo;

#define EQUIP_HEAD	0
#define EQUIP_NECK	1
#define EQUIP_LRING	2
#define EQUIP_RRING	3
#define EQUIP_HANDS	4	/* duplicate */
#define EQUIP_LHAND	4
#define EQUIP_RHAND	5
#define EQUIP_MISSILE	6
#define EQUIP_TOOL      7

#define EQUIP_CLOAK	8
#define EQUIP_SHIRT	9
#define EQUIP_GLOVES	10
#define EQUIP_PANTS	11
#define EQUIP_BOOTS	12
#define EQUIP_BODY	13
#define EQUIP_LARM	14
#define EQUIP_RARM	15
#define EQUIP_LEGS	16
#define MAX_EQUIP	17

//#define EQUIP_LIGHT             16
//#define EQUIP_MISWEAPON         17

/* armor groups */
#define ARM_CLOAK       EQUIP_CLOAK
#define ARM_SHIRT       EQUIP_SHIRT
#define ARM_PANTS       EQUIP_PANTS
#define ARM_LEGARM    	EQUIP_LEGS
#define ARM_BODY        EQUIP_BODY
#define ARM_LHANDARM    EQUIP_LARM
#define ARM_RHANDARM    EQUIP_RARM
#define ARM_BOOTS       EQUIP_BOOTS
#define ARM_GLOVES      EQUIP_GLOVES
#define ARM_HELMET      EQUIP_HEAD
#define ARM_SHIELD      EQUIP_HANDS

//List of equipment for a creature.
class equipment
{
public:
	enum Equipment_Events
	{
		Equipped,
		Removed,
		No_Items_To_Equip,
		Nothing_Selected,
		Slot_Reserved,

		Does_Not_Fit,
		Cant_Use_Twohanded,
		Item_Unpaid,
		Cursed,
		Already_In_Use,

		Item_Lost,

		Amt_Of_Events
	};

private:
	struct Equipment_Event
	{
		int color;
		const char *message;
	};

	static const char *equip_slotdesc[];
	static const Equipment_Event events[Amt_Of_Events];
	
	int equip_checkfit(int slot, item_def *item);
	void display_result(int what, int slot);
	int get_reserved(int slot);
	bool is_empty(int slot);

	void put_on(invnode *ni, int slot);
	void reserve(int slot, int dest_slot);

public:
	Equipslot equip[MAX_EQUIP];

	equipment();

	bool decide_meleeweapon(bool *useleft, bool *useright);
	
	const char *get_equipment_name(int slot);
	item_def *get_item(int slot);
	invnode *get_inventory_item(int slot);
	bool Get_Ranged_Gear(int &wpntype, int &mistype, int &bonus);
	
	bool is_usable(int slot);

	void clear_slot(int slot);
	static void display_description(int slot);
	int equipitem(int slot, playerinfo &plr);
	void make_usable(int slot);
	bool monster_equip(level_type *level, being *monster, invnode *useitem);
	int monster_unequip(level_type *level, being *monster, invnode *removeitem);
	void player_equip();
	void set_status(int slot, int8u st);
	void show();

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb, inventory &inv);	
};

#endif
