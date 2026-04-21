/*
 * Legend of Saladir roguelike
 * (C) 1997/1998 by Erno Tuomainen
 *
 */

//Unit optcont: Gameplay options container class.

#ifndef OPTCONT_H
#define OPTCONT_H

#include <vector>
#include "geometry.h"

class Option;

//Container class for option data.
class Optcont
{
private:
	static const char confile[]; //name of the options file
	std::vector<Option*> opts; //list of options
	bool changed; //if options are changed
	int index; //points to an option in the list
	Coord inpoint; //where to start input text

	typedef std::vector<Option*>::iterator oitr;

	int Amount();
	void Show_List();
	
public:
	Optcont();
	~Optcont();

	bool Is_Changed() const { return changed; }

	void Edit();
	void Load();
	bool Save();
};

extern Optcont *optcont;

#endif
