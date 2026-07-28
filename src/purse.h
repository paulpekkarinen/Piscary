//Legend of Saladir - purse.h

//Unit purse: List of items, either in creature inventory or on ground.

#ifndef PURSE_H
#define PURSE_H

#include <list>
#include "types.h"

class Gameview;
struct Itempack;
class Pocket;
struct Target;

/* carrying weight burnen levels */
#define WGH_OVERLOAD    98 /* values out of 100 which is the capasity */
#define WGH_STRAIN      90
#define WGH_BURDEN      75

//Inventory holds a linked list of items.
class inventory
{
public:
	enum Inventory_Type {Ground, Backpack, Nothing};

private:
	friend class Display;

	int capasity;
	int copper; //money amount in copper coins (10 copper = 1 gold)
	int weight; //total weight (1000 is 1kg)

	void Calc_Recursive(); //calculate weight and copper values
	invnode *Find_Item(invnode *src); //recursive
	void recursive_task(int op);

protected:
	std::list<invnode*> items;
	typedef std::list<invnode*>::iterator oitr;

public:
	inventory();
	~inventory();

	int Count_Items(int type, int x, int y, bool recursive);

	invnode *Find_Best_Item(int type, int group);
	invnode *Find_By_Slot_Id(int id); //recursive

	invnode *Get_Last_Item();
	int Get_Linear_Size() const; //get the size without inner container items
	int Get_Wealth() const { return copper; }
	int Get_Weight() const { return weight; }

	void Add_Copper(int amount); //add copper coins to inventory
	void Add_Gold(int amount); //add gold coins to inventory
	void Add_Item(invnode *new_item);
	void Add_Silver(int amount); //add silver coins to inventory
	void age_food_items();

	bool builditemarray(Pocket &ptrlist, int filter, int x, int y);
	void clean_shop_items(level_type *level, being *owner);
	void Clear(); //clear everything from inventory
	void collectmoneypointers_recurse(Pocket *ptrlist);
	invnode *Create_Item(const Itempack &ip); //create new item
	int Destroy_Item(equipment &gear, invnode *remthis, int count);
	void destroy_item(equipment &gear, invnode *src);
	void destroy_one_stacked_item(equipment &gear, invnode *src);
	invnode *detach(invnode *src);

	void mark_normal();
	void mark_unpaid();
	bool ready_newmissile(equipment &e, int type, int group);
	void recalculate();
	invnode *remove_n_items(invnode *src, int count);
	invnode *remove_one_item(invnode *src);
	invnode *Remove_An_Unpaid_Item(); //get one unpaid item

	void List_Items();

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
