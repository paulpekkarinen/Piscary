//Legend of Saladir - verse.h

//Unit verse: Container for text pieces loaded from text data file.

#ifndef VERSE_H
#define VERSE_H

#include <string>
#include <vector>

//Verse can contain one of more text pieces. This is an object of Script
//class which holds a list of verses.
class Verse
{
private:
	std::vector<std::string> fragments;
	int index; //current index if shown in chronological (chained) order

public:
	Verse();

	void Add(std::string &txt);

	const char *Get();
	const char *Get_At(int i);
	const char *Get_Chained();
	const char *Get_Random();
	int Get_Size(); //get number of items

	void Show_Data(); //debug function
};

#endif
