//Legend of Saladir - condit.h

//Unit condit: Conditions for creatures.

#ifndef CONDIT_H
#define CONDIT_H

class Tar_Ball;

/* conditions */
#define CONDIT_BLOATED    0
#define CONDIT_SATIATED   1
#define CONDIT_HUNGRY     2
#define CONDIT_STARVING   3
#define CONDIT_FAINTING   4

#define CONDIT_FAINTED    5
#define CONDIT_BURDENED   6
#define CONDIT_STRAINED   7
#define CONDIT_OVERLOADED 8
#define CONDIT_CONFUSED   9

#define CONDIT_BLESSED    10
#define CONDIT_CURSED     11
#define CONDIT_STUN       12
#define CONDIT_LUCKY      13
#define CONDIT_BLEEDING   14

#define CONDIT_POISONED   15
#define CONDIT_PARALYSED  16
#define CONDIT_BADLEGS    17
#define CONDIT_BADLARM    18
#define CONDIT_BADRARM    19

#define CONDIT_MAX        20

/* condition groups */
#define CONDGRP_NOTSET    0
#define CONDGRP_FOOD      1
#define CONDGRP_PW        2

/* condition flags */
#define CONDF_NOVIS    0x0001 /* don't show on status line */

/* condition data struct */
class Condition
{
private:
	int type;
	int val;
	int time;

public:
	Condition() : type(0), val(0), time(0) { }
	Condition(int t) : type(t), val(0), time(0) { }

	int Get_Group();
	static int Get_Group_Of(int t);
	int Get_Type() const { return type; }
	int Get_Value() const { return val; }

	void Change_Value(int v) { val+=v; }
	bool Spend(int v);

	bool Show();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
