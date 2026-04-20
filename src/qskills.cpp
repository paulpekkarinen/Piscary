//qskills.cpp

#include "qskills.h"
#include "storage.h"

void quickskill::Reset()
{
	group=0;
	type=0;
	select=false;
}

void quickskill::Save(Tar_Ball &tb)
{
	tb.Put(group);
	tb.Put(type);
	tb.Put_Bool(select);
}

void quickskill::Load(Tar_Ball &tb)
{
	group=tb.Get_Next_Value();
	type=tb.Get_Next_Value();
	select=tb.Get_Next_Bool();
}
