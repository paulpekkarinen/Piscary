//Legend of Saladir - attribut.cpp

//Written in 10.9.2022 - 21.9.2025 by Paul K. Pekkarinen

#include "attribut.h"
#include "storage.h"

void Attribute::Decrease(int v)
{
	value-=v;
	if (value<0) //keeps at zero
		value=0;
}

void Attribute::Initialize(int v)
{
	value=v;
	max_value=v;
}

void Attribute::Maximize()
{
	value=max_value;
}

void Attribute::Save(Tar_Ball &tb)
{
	tb.Put(value);
	tb.Put(max_value);
}

void Attribute::Load(Tar_Ball &tb)
{
	value=tb.Get_Next_Value();
	max_value=tb.Get_Next_Value();
}
