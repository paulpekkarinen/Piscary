//Legend of Saladir - rawdata.h

//Unit rawdata: Raw text data.

#ifndef RAWDATA_H
#define RAWDATA_H

#include <string>

class Rawdata
{
private:
	bool working; //false if the database didn't load or setup correctly
	std::string text; //char data as string
	int bytes; //size of the 'text' data buffer
	const int Number_Of_Tags; //number of text ids in the data
	int *tag_offsets; //offsets of each data piece
	int index; //current index location to data

	bool Find_Offsets();

public:
	Rawdata(const char *fn, const int numtags);
	~Rawdata();

	char Get_Next_Char();

	bool Is_Working() const { return working; }

	void Set_Index(int tag);
};

#endif
