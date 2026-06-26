//tactics.cpp

#include "message.h"
#include "tactics.h"

tactics tactics_data[]=
{
	{"Coward", -4, 70, -30, 8, 30},
	{"Very defensive", -2, 80, -20, 6, 20},
	{"Defensive", -1, 90, -10, 4, 10},
	{"Normal", 0, 100, 0, 0, 0},
	{"Aggressive", 1, 100, 5, -4, -10},
	{"Very aggressive", 2, 100, 10, -6, -20},
	{"Berzerk", 4, 100, 20, -8, -30}
};

void tactics::Show()
{
	msg.vadd(C_WHITE,
		"Tactic: %s (Hit:%+d, Dodge:%+d, DAM:%+d, AC:%+d, SPD:%+d%%)!",
		name, hit, dv, dam, pv, 100-spd);
}
