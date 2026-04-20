/* venture.h */

//Unit venture: List of quests.

#ifndef VENTURE_H
#define VENTURE_H

#include <list>
#include "types.h"

struct Quest;

class Venture
{
private:
	std::list<Quest*> quests;

	typedef std::list<Quest*>::iterator qitr;

	Quest *find(int quest);
	bool remove(int quest);

public:
	~Venture();

	int add(being *monster, int quest);
	void check_kill(being *mptr);
	void complete(int qnum);
	int isavailable(int quest);
	int nextavail(being *monster);
	bool ownerdied(being *mptr);
	void removeall();
	void reward(int qnum);
	void showall();
	int unfinished(being *monster);

	void save(Tar_Ball &tb);
	void load(Tar_Ball &tb);
};

#endif
