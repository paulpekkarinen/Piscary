//Legend of Saladir - way.h

//Unit way: Directions and routines for them.

#ifndef WAY_H
#define WAY_H

#include "types.h"

// directions of move
#define DIR_UP		8
#define DIR_DOWN	2
#define DIR_RIGHT	6
#define DIR_LEFT	4
#define DIR_UPRIGHT 9
#define DIR_UPLEFT  7
#define DIR_DNRIGHT 3
#define DIR_DNLEFT  1

#define DIR_SELF    5

struct Way
{
	//these match the move_dx, dy matrix data, they also match the defines above
	enum Directions
	{
		Nowhere,
		Southwest,
		South,
		Southeast,
		West,
		Yourself,
		East,
		Northwest,
		North,
		Northeast
	};

	static int Get_Command_Direction(int c);
	static int Get_Diagonal_Direction(int index); //for four diagonal directions
	static int Get_Direction(int index); //from 0-7 for main directions
	static int Get_Dir_To(level_type *level, int x1, int y1, int x2, int y2);
	static int Get_From_Keycode(int k); //get direction from keycode
	static int Get_Main_Direction(int index); //for four main directions
	static int Get_Random_Main_Direction();
};

#endif
