/* roleplay.h */

//Unit roleplay: Generic role-playing system routines.

#ifndef ROLEPLAY_H
#define ROLEPLAY_H

#include "types.h"

struct Equipslot;
struct hpslot;
struct playerinfo;

/* alignments */
#define ALIGNMENT_LIMIT	4000
#define CHAOTIC_S		0
#define CHAOTIC_E    ( ALIGNMENT_LIMIT - 1 )
#define NEUTRAL_S		ALIGNMENT_LIMIT
#define NEUTRAL_E		( ALIGNMENT_LIMIT * 2 )
#define LAWFUL_S 		( ( ALIGNMENT_LIMIT * 2 ) + 1)
#define LAWFUL_E 		( ALIGNMENT_LIMIT * 3 )
#define CHAOTIC		0
#define NEUTRAL		( ( ALIGNMENT_LIMIT * 3 ) / 2 )
#define LAWFUL			( ALIGNMENT_LIMIT * 3 )
#define RANDALIGN		-1

class Roleplay
{
private:
	static const int Maxnum_Levels=100;
	static const int Table_Len=101;

	int expneeded[Maxnum_Levels+1];
	int DEX_SPEED[Table_Len]; //How dexterity changes speed (time units consumination)
	int CON_HITP[Table_Len]; //constitution to hit points regeneration

	void Advance_Levels(being *mptr); //advance creature ONE level up
	void Calculate_Raisestats(playerinfo &plr); //advance player one level up

public:
	Roleplay();

	int Get_Con_HP(int index);
	int Get_Dex_Speed(int index);
	int Get_Experience_Levels(int &level, int exp); //new experience levels based on points

	void Advance_To_Level(being *b, int lvl);
	void Calculate_HP(hpslot *hpack, int basehp, int race);
	void Calculate_Itembonus(Actor *actor);
	int Calculate_Slot_AC(int bodypart, Equipslot *eqslots); //note: not used at the moment
	int Calculate_Slot_PV(int bodypart, Equipslot *eqslots); //note: not used at the moment
	void Check_Expneeded(playerinfo &plr);
	bool Check_Levelraise(playerinfo &plr);
	bool Check_Levelraise(being *monster, bool initmode);
};

extern Roleplay roleplay; //global instance initialized in start up

#endif
