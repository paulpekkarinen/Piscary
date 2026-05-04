//Legend of Saladir - way.cpp

//Refactored 3.12.2021 - 2.4.2023 Paul K. Pekkarinen

#include "caves.h"
#include "cmd.h"
#include "dice.h"
#include "move.h"
#include "output.h"
#include "way.h"

int Way::Get_Command_Direction(int c)
{
	int rv;

	switch (c)
	{
		case cmd::Walk_North:
		case cmd::Move_North: rv=North; break;
		case cmd::Walk_Northeast:
		case cmd::Move_Northeast: rv=Northeast; break;
		case cmd::Walk_East:
		case cmd::Move_East: rv=East; break;
		case cmd::Walk_Southeast:
		case cmd::Move_Southeast: rv=Southeast; break;
		case cmd::Walk_South:
		case cmd::Move_South: rv=South; break;
		case cmd::Walk_Southwest:
		case cmd::Move_Southwest: rv=Southwest; break;
		case cmd::Walk_West:
		case cmd::Move_West: rv=West; break;
		case cmd::Walk_Northwest:
		case cmd::Move_Northwest: rv=Northwest; break;
		default: rv=Nowhere; break;
	}

	return rv;
}

int Way::Get_Diagonal_Direction(int index)
{
	static const int dirs[4]={Northeast, Southeast, Southwest, Northwest};

	if (index<0 || index>3) return Nowhere;

	return dirs[index];
}

int Way::Get_Direction(int index)
{
	static const int dirs[8]={North, Northeast, East, Southeast, South, Southwest, West, Northwest};

	if (index<0 || index>7) return Nowhere;

	return dirs[index];
}

int Way::Get_From_Keycode(int k)
{
	int dir;

	switch (k)
	{
		case KEY_UP: dir=North; break;
		case KEY_DOWN: dir=South; break;
		case KEY_LEFT: dir=West; break;
		case KEY_RIGHT: dir=East; break;
		default:
			if (k>='1' && k<='9')
			{
				dir=k-'0';
			}		
			else dir=-1;
		break;
	}

	return dir;
}

int Way::Get_Main_Direction(int index)
{
	static const int dirs[4]={North, East, South, West};

	if (index<0 || index>3) return Nowhere;

	return dirs[index];
}

/*
 * get a horizontal or vertical direction from x1,y1 to x2,y2
 * if "onlymajor" is set, then get only one of these directions,
 * "LEFT" "RIGHT" "UP" or "DOWN".
 */
int Way::Get_Dir_To(level_type *level, int x1, int y1, int x2, int y2)
{
	int i;

	x2-=x1;
	y2-=y1;

	for (i=0; i<10; i++)
	{
		if (x2==move_dx[i] && y2==move_dy[i])
			break;
	}

	if (i==DIR_UPLEFT)
	{
		if (level->Is_Passable(x1-1, y1))
			return DIR_LEFT;
		else if (level->Is_Passable(x1, y1-1))
			return DIR_UP;
	}
	else if (i==DIR_UPRIGHT)
	{
		if (level->Is_Passable(x1+1, y1))
			return DIR_RIGHT;
		else if (level->Is_Passable(x1, y1-1))
			return DIR_UP;
	}
	else if (i==DIR_DNRIGHT)
	{
		if (level->Is_Passable(x1+1, y1))
			return DIR_RIGHT;
		else if (level->Is_Passable(x1, y1+1))
			return DIR_DOWN;
	}
	else if (i==DIR_DNLEFT)
	{
		if (level->Is_Passable(x1-1, y1))
			return DIR_LEFT;
		else if (level->Is_Passable(x1, y1+1))
			return DIR_DOWN;
	}

	return i;  /* 5 */
}

int Way::Get_Random_Main_Direction()
{
	const int i=random_number(0, 7);
	return Get_Direction(i);	
}
