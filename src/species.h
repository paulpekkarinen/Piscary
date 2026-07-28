//Legend of Saladir - species.h

//Unit species: Creature type.

#ifndef SPECIES_H
#define SPECIES_H

class Species
{
public:
	enum Types
	{
		Human,
		Cathuman,
		High_Elf,
		Wood_Elf,
		Gray_Elf,

		Dark_Elf,
		Dwarf,
		Cat,
		Kobold,
		Orc,

		Pit_Snake,
		Cave_Spider,
		Hunter_Spider,
		Giant_Spider,
		Fruit_Bat,

		Gorilla,
		Monkey,
		Deer,
		Rabbit,
		Max_Amount
	};

private:
	int s;

public:
	Species() : s(0) { }
	Species(int spc) : s(spc) { }

	int Get() const { return s; }
	void Mutate(int n) { s=n; }

	void Debug_Info();
	void Draw_Tile();
};

#endif
