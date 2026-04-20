//Legend of Saladir - shoppe.h

//Unit shoppe: Shop data.

#ifndef SHOPPE_H
#define SHOPPE_H

#include "types.h"

struct Coord;
struct invnode;
class Spawner;
class Tar_Ball;

class Shoppe
{
public:
	enum Shopperone_Types
	{
		None, //indicates this room is not a shop
		Armour,
		Weapon,
		General,
		Food,
		Book,
		Potion,
		Magic,
		Amt_Of_Shops
	};
	
private:
	int type; //which shopperone this is
	int subtype; //aka name of the shop
	int sellp; // 100 = items are not over or low priced
	int buyp; // 100 = will pay the real item value from items
	
public:
	Shoppe();

	bool Accept_Item(invnode *item);
	const char *Get_Name();
	real Get_Selling_Price(invnode *item);
	bool Is_Open();

	void Create_Item(Spawner &spw, const Coord &c);
	void Item_Sold(invnode *item);
	void Shopify(int st); //make it a shop

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
