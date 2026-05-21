//Legend of Saladir - crew.h

//Unit crew: List of monsters in level.

#ifndef CREW_H
#define CREW_H

#include <list>
#include "types.h"

class Gameview;

//List of monsters and routines to handle them.
class Crew
{
private:
	std::list<being*> monsters;	// linked list for monsters in this level

	typedef std::list<being*>::iterator mon_iter;

	bool clean_up; //destroy monster instances at the end of turn when they die

	void Cleantargets(being *remove);

public:
	Crew() : clean_up(false) { }

	being *Find_By_Index(int index); //return monster in the list from the index
	being *Find_Monster_At(const Coord &c); //slow version visiting the list
	int Get_Index(being *b); //return index of monster in the list for saving

	void Add_Monster(being *b);
	void handle_monsters(level_type *level, int slots);
	void Refresh_Gameview(Gameview *gv); //put all monsters to gameview
	void removeall();
	void Remove_Dead(level_type *level); //remove at the end of turn
	void Sentenced() { clean_up=true; } //notify that there are dead monsters
	int Target_Nearest(level_type *level, int *cx, int *cy, int lastidx);

	void Debug_List(); //find out amount of monsters and special monsters
	void Full_Debug_List(); //lists all monsters
	void Unalive_Monsters(); //get rid of everyone

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb, level_type *lvl);
};

#endif
