//Legend of Saladir - target.cpp

#include "actor.h"
#include "invnode.h"
#include "target.h"

void Target::Clear()
{
	olento=0;
	invitem=0;
	pos.Set_Location(-1, -1);
}

bool Target::Is_Active()
{
	if (olento!=0 || invitem!=0 || pos.x!=-1)
		return true;

	return false;
}

void Target::Set(const Coord &c)
{
	pos=c;
	olento=0;
	invitem=0;
}

void Target::Set(Actor *o)
{
	olento=o;
	pos=o->Get_Location();
	invitem=0;
}

void Target::Set(invnode *i)
{
	invitem=i;
	//-1 indicates an inventory item
	pos.Set_Location(-1, -1);
	olento=0;
}

void Target::Set(invnode *i, const Coord &c)
{
	invitem=i;
	pos=i->Get_Location();
	olento=0;
}
