//Legend of Saladir - program.h

//Unit program: Program-wide data and functions.

#ifndef PROGRAM_H
#define PROGRAM_H

#include "types.h"

class Scoreboard;

#define PROC_VERSION "V0.0.28"
#define PROC_DATE "(Wed Mar 6 23:04:23 2019)"
#ifdef linux
#define PROC_PLATFORM "-Linux"
#else
#define PROC_PLATFORM "-DOS"
#endif

//Program handles the first initialization of the program, running game
//instances and on exit cleaning up global resources.
class Program
{
public:
	Scoreboard *scoreboard;

private:
	static char SALADIR_VERSTRING[];

public:
	Program();
	~Program();

	const char *Get_Version() { return SALADIR_VERSTRING; }
};

extern Program *program;
extern bool global_initmode;

#endif
