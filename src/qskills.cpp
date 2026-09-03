//Legend of Saladir - qskills.cpp

#include "qskills.h"
#include "skills.h"
#include "storage.h"

bool quickskill::Is_Selected(skill *src)
{
	if (group == src->group && type == src->type && select == true)
		return true;

	return false;
}

void quickskill::Reset()
{
	group=0;
	type=0;
	select=false;
}

void quickskill::Toggle(skill *src)
{
	if (Is_Selected(src))
		select=false;
	else
	{
		select=true;
		group=src->group;
		type=src->type;
	}
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
