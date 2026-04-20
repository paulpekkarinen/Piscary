//randgen.h

//Unit randgen: Random generation routines for terrain features.

#ifndef RANDGEN_H
#define RANDGEN_H

#include "types.h"

void carvepassage(level_type *level, int sx, int sy, int tx, int ty);
void carvepassage2(level_type *level, int sx, int sy, int tx, int ty, bool stopopen, int dir);
bool checkregion(level_type *level, int x1, int y1, int rsizex, int rsizey);
bool checkregion_inv(level_type *level, int x1, int y1, int rsizex, int rsizey);
void create_random_terrain(level_type *level, int type, int count);
void create_terrain_on_passable(level_type *level, int type);
Coord find_random_location(level_type *level, int border);
Coord find_random_location(level_type *level, int border, bool skip_player);
Coord get_random_location(const Plane &p, int border);
void recurse_maze(level_type *maze, int x, int y); //creates a maze
void searchstaircase(level_type *level, int stair, int ldir);
void set_randomcoord(level_type *level, int *tx, int *ty);

#endif
