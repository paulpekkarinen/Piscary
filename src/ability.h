//Legend of Saladir - ability.h

//Unit ability: Basic stats.

#ifndef ABILITY_H
#define ABILITY_H

class Tar_Ball;

struct Ability
{
	int STR; //strength
	int WIS; //wisdom
	int DEX; //dexterity
	int CON; //constitution
	int CHA; //charisma

	int INT; //intelligence
	int TGH; //toughness
	int LUC; //luck
	int SPD; //speed

	Ability& operator=(const Ability& right)
	{
		if(this == &right) return *this;
		STR=right.STR;
		WIS=right.WIS;
		DEX=right.DEX;
		CON=right.CON;
		CHA=right.CHA;

		INT=right.INT;
		TGH=right.TGH;
		LUC=right.LUC;
		SPD=right.SPD;
		return *this;
	}

	int Get_Statpack_Value(int slot);

	void Clear();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
