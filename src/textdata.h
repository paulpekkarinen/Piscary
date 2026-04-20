//Legend of Saladir - textdata.h

//Unit textdata: Pieces of text.

#ifndef TEXTDATA_H
#define TEXTDATA_H

#include <array>
#include <string>

//Loads and stores text files.
class Text_Data
{
public:
	enum Text_Pieces
	{
		Inventory_Help,
		Manual,
		Missile_Help,
		Amt_Of_Pieces
	};

private:
	static const char *filenames[Amt_Of_Pieces];
	std::array<std::string, Amt_Of_Pieces> data;

public:
	Text_Data();

	void View(int piece);
};

extern Text_Data *text_data;

#endif
