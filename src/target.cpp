//Legend of Saladir - target.cpp

#include "being.h"
#include "caves.h"
#include "invnode.h"
#include "output.h"
#include "storage.h"
#include "target.h"

void Target::Clear()
{
	type=None;
	olento=0;
	invitem=0;
	pos.Set_Location(-1, -1);
}

Coord Target::Get_Location()
{
	Coord rv;
	switch (type)
	{
		case Creature: rv=olento->Get_Location(); break;
		case Level_Item: rv=invitem->Get_Location(); break;
		default: rv=pos; break;
	}
	return rv;
}

bool Target::Is_Active()
{
	if (type!=None)
		return true;

	return false;
}

void Target::Set(const Coord &c)
{
	type=Terrain;
	pos=c;
}

void Target::Set(Actor *o)
{
	type=Creature;
	olento=o;
}

//note: when item is removed from level or placed to inventory the target
//pointer is not lost which has to be solved later somehow
void Target::Set(invnode *i)
{
	type=Inventory_Item;
	invitem=i;
}

void Target::Set(invnode *i, const Coord &c)
{
	type=Level_Item;
	invitem=i;
}

void Target::Show_Data()
{
	if (Is_Active()==false)
	{
		my_printf("No target\n");
		return;
	}

	if (olento!=0)
		my_printf("Target (%d): %s ", type, olento->Get_Name());

	if (invitem!=0)
		my_printf("Target (%d): %s ", type, invitem->Get_Name());

	if (pos.x!=0)
		my_printf("Target location (%d): %d, %d\n", type, pos.x, pos.y);
	else
		my_printf("(No target pos.)\n");
}

//When saving and loading use type to indicate something was saved.
void Target::Save(Tar_Ball &tb)
{
	Coord c;
	tb.Put(type);

	//this assumes that pointers exist...
	switch (type)
	{
		case Creature:
			c=olento->Get_Location();
			c.Save(tb);
		break;
		case Level_Item:
		case Inventory_Item:
			c=invitem->Get_Location();
			c.Save(tb);
		break;
		case Terrain:
			pos.Save(tb);
		default: break;
	}
}

void Target::Load(Tar_Ball &tb, level_type *level)
{
	Clear();

	type=tb.Get_Next_Value();

	switch (type)
	{
		case Creature:
			pos.Load(tb);
			olento=level->crew.Find_Monster_At(pos);
		break;
		case Level_Item:
			pos.Load(tb);
			//note: this sets the top item as target even if it could be
			//something else, because target only knows the location of
			//the item
			invitem=level->inv.Top_Item(pos);
		break;
		case Inventory_Item:
			//note: yes.. hmm.. how to load inventory item?
		break;
		case Terrain:
			pos.Load(tb);
		break;
		default: break;
	}
}
