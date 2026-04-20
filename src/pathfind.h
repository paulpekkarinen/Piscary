/* Legend of Saladir - pathfind.h */

//Unit pathfind: Pathfinder data and functions.

#ifndef PATHFIND_H
#define PATHFIND_H

#include <list>
#include "geometry.h"

struct level_type;

class Pathlist
{
private:
	std::list<Coordist> coords;

	void Show_Debug_Info(char ch, int len);

public:
	void clear();
	void find_route(level_type *level, int x1, int y1, int x2, int y2);
	Coordist getbest(const Coordist &end);
	Coordist *get_first();
	bool has_path();
	void insert(Coordist *data);
	void insert_end(Coordist *data);
	bool movealong(int *tx, int *ty); //makes monster follow the path
	void remove_first();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
