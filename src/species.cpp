//Legend of Saladir - species.cpp

//Refactored 26.7.2026 Paul K. Pekkarinen

#include "creature.h"
#include "output.h"
#include "species.h"

void Species::Debug_Info()
{
	my_printf("%s ", npc_races[s].name);
	Draw_Tile();
}

void Species::Draw_Tile()
{
	put_char(npc_races[s].out, npc_races[s].color);
}
