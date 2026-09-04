//Legend of Saladir - shoppe.cpp

//Refactored 25.9.2021 - 2.4.2026 Paul K. Pekkarinen

#include "dice.h"
#include "invnode.h"
#include "script.h"
#include "shoppe.h"
#include "spawner.h"
#include "storage.h"

struct Shop_Data
{
	int script_id;
	int sellp;
	int buyp;
};

const Shop_Data shop_data[Shoppe::Amt_Of_Shops]=
{
	{-1, 0, 0},
	{Script::Shop_Armours, 110, 70},
	{Script::Shop_Weapons, 100, 75},
	{Script::Shop_General, 100, 50},
	{Script::Shop_Food, 100, 30},

	//note: these shop types are not yet designed
	{Script::Shop_General, 100, 50},
	{Script::Shop_General, 100, 50},
	{Script::Shop_General, 100, 50}
};

Shoppe::Shoppe()
	: type(None), subtype(0), sellp(0), buyp(0)
{

}

const char *Shoppe::Get_Name()
{
	if (type==None)
		return "room";

	return texts->Get_At_Index(shop_data[type].script_id, subtype);
}

real Shoppe::Get_Selling_Price(invnode *item)
{
	real copper=(real)item->i.price * item->count;
	real mod=(real)buyp;
	mod=mod/100;
	copper=copper * mod * item->Get_Material_Mod();

	return copper;
}

bool Shoppe::Is_Open()
{
	if (type<1)
		return false;

	return true;
}

//Determines if this shop accepts a certain type of item.
bool Shoppe::Accept_Item(invnode *item)
{
	bool rv=false;

	switch (type)
	{
		case Armour:
		{
			rv=item->Is_Armor();
		}
		break;
		case Weapon:
		{
			rv=item->Is_Weapon();
		}
		break;
		case Food:
		{
			if (item->i.type == IS_FOOD)
				rv=true;
		}
		break;
		case General:
			rv=true;
		break;
		default: break;
	}

	return rv;
}

//Creates item for this shop type, assumes that location is valid.
void Shoppe::Create_Item(Spawner &spw, const Coord &c)
{
	if (Is_Open()) //check valid shop just in case
		spw.Create_Shop_Item(c, type, sellp);
}

//Set item price after it's sold to the shop.
void Shoppe::Item_Sold(invnode *item)
{
	/* item is no more players */
	item->i.status |= ITEM_UNPAID;

	/* calculate the shop sellout price */
	real mod=(real)sellp;
	mod=mod/100;
	real copper=(real)item->i.price;
	copper=copper * item->Get_Material_Mod() * mod;
	item->i.price=(int32u)copper;
}

void Shoppe::Shopify(int st)
{
	//not a shop type (clear values if this is wanted)
	if (st<1)
	{
		type=None;
		subtype=0;
		return;
	}

	type=st;

	//get number of different shop names from text data
	const int amt=texts->Get_Items(shop_data[st].script_id);

	subtype=random_number(0, amt-1);

	buyp=shop_data[st].buyp;
	sellp=shop_data[st].sellp;
}

void Shoppe::Save(Tar_Ball &tb)
{
	tb.Put(type);
	tb.Put(subtype);
	tb.Put(sellp);
	tb.Put(buyp);	
}

void Shoppe::Load(Tar_Ball &tb)
{
	type=tb.Get_Next_Value();
	subtype=tb.Get_Next_Value();
	sellp=tb.Get_Next_Value();
	buyp=tb.Get_Next_Value();
}
	
