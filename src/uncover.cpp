//Legend of Saladir - uncover.cpp

//Refactored 15.7.2022 - 26.3.2026 Paul K. Pekkarinen

#include "amount.h"
#include "avatar.h"
#include "creature.h"
#include "gameview.h"
#include "input.h"
#include "invnode.h"
#include "output.h"
#include "purse.h"
#include "uncover.h"

Uncover uncover;

namespace data
{
	int32u item_flags[ITEM_FLAGS]=
		{ITEM_CURSED, ITEM_BLESSED, ITEM_ENCHANTED, ITEM_NOTPASSABLE,
		ITEM_UNPAID, ITEM_GENERATED, ITEM_ARTIFACT, ITEM_IDENTIFIED};

	const char *item_flag_debug_names[ITEM_FLAGS]=
		{"Curs", "Bles", "Ench", "Notp",
		"Unpa", "Gene", "Arti", "Iden"};
}
using namespace data;

void Uncover::Inventory_Item(invnode *iptr, int index, int x, int y)
{
	gotoxy(x, y);

	my_printf("%d) ", index);

	const int ic=iptr->count;

	if (ic>1)
		my_printf("%d ", ic);

	Item_Data(iptr->i);

	my_printf(" (%d, %d)", iptr->x, iptr->y);
}

void Uncover::Item_Data(item_def &item)
{
	my_printf("%s ", item.name.c_str());

	if (item.inv!=0)
		my_printf("[cont] with %d items ", item.inv->Get_Linear_Size());

	bool found=false;
	int a=0;

	for (int t=0; t<ITEM_FLAGS; t++)
	{
		if (item.status & item_flags[t])
		{
			a++;
			if (found==false)
			{
				my_printf("[");
				found=true;
			}
			if (a>1) my_printf(",");

			my_printf("%s", item_flag_debug_names[t]);
		}
	}

	if (found)
		my_printf("] ");
}

void Uncover::Program_Data()
{
	clear_screen();

	my_printf("Terminal size: %d, %d\n", SCREEN_COLS, SCREEN_LINES);
	gameview.Show_Data();
	Coord pc=player.Get_Location();
	my_printf("Player's location: %d, %d\n", pc.x, pc.y);

	mucho.Show_Data();

	wait_key();
}
