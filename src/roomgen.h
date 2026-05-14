//roomgen.h

//Unit roomgen: Room generation data and routines.

#ifndef ROOMGEN_H
#define ROOMGEN_H

class Genlevel;

struct Roomtemplate
{
	int sx;
	int sy;
	int doordir;
	int roomtype;
	const char *room;
};

bool createroom(Genlevel *level, int *rx1, int *ry1, int *sizex, int *sizey);
bool generatelair(Genlevel *level);
bool generate_townroom(Genlevel *level, int roomtype);
void makehall(Genlevel *level, int x1, int y1, int dir, int recd);
void makehall_only(Genlevel *level, int x1, int y1, int dir);
void makeroom(Genlevel *level, int x1, int y1, int dir, int recd);

extern Roomtemplate towntemplates[];
extern Roomtemplate lairtemplates[];

#endif
