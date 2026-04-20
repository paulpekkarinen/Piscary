//Legend of Saladir - pocket.h

//Unit pocket: Temporary list of items, can be filtered by type.

#ifndef POCKET_H
#define POCKET_H

#include <list>
#include "types.h"

struct Currency;

//List of items when picking up items for example.
class Pocket
{
private:
	//A node of pocket list.
	struct itemlistptr
	{
		invnode *ptr;
		bool sel; //this item is selected to pick up etc.

		explicit itemlistptr(invnode *i)
			: ptr(i), sel(0) { }
	};

	std::list<itemlistptr> items;

	typedef std::list<itemlistptr> poclist;
	typedef poclist::iterator pitr;

	void Add_Coins(Currency &c, inventory &to_inv);

public:
	bool collectmoneyptr(inventory &inv);

	void add_item(invnode *ptr);
	void Clear_Items();
	void Show(int starting_index, int &lasttype, bool darklevel);

	int Get_Selected(); //amount of items currently selected
	int get_weight_of_items(); //returns weight of the current items in the pocket
	bool is_empty();
	void money_transaction
		(inventory &from_inv, inventory &to_inv, equipment &gear,
			int copperneed, int copperamt);

	void Transfer(inventory &from_inv, inventory &to_inv); //get selected items
};

#endif
