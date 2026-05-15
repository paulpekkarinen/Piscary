//move.h - game object movements

#ifndef MOVE_H
#define MOVE_H

#include "geometry.h"
#include "types.h"

struct playerinfo;

/* result codes for move_item() */
#define BLOCKED_MONSTER 1
#define BLOCKED_WALL    2

int monster_moveAROUNDtarget
	(level_type *level, int *mx, int *my, int tx, int ty);
int monster_moveFROMtarget
	(level_type *level, int *mx, int *my, int tx, int ty);
int monster_moveTOtarget
	(level_type *level, int *mx, int *my, int tx, int ty, bool getpast);
int move_item(level_type *level, invnode *item, int dir, bool checkmonster);
int move_monster(being *, level_type *);
int moveplayer(playerinfo *plr, int dir, level_type *level, bool autowalk);
Coord move_to_direction(int dir, playerinfo *plr); //move from player's place to some direction
Coord move_to_direction(int dir, const Coord &c); //move from c to some direction
bool player_push(playerinfo *plr, level_type *level, int dir);
void repeatmove(playerinfo *plr, int dir, level_type *level);
int shopkeeper_move(level_type *level, being *keeper);
int sur_countall(level_type *level, int x, int y);
int sur_countpass_dia(level_type *level, int x, int y);
bool teleport_item(level_type *level, invnode *iptr);
bool teleport_monster(level_type *level, being *mptr);
void teleport_player(level_type *level, bool inform, bool not_in_room);

extern const int move_dx[];
extern const int move_dy[];

#endif
