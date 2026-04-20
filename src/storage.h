//Legend of Saladir - storage.h - Copyright (C) Paul K. Pekkarinen

//Unit storage: Serialized bytes to save or load.

#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include "types.h"

class File;

//Transforms raw data into character/binary hybrid type with the
//size of the next data block saved as a binary value followed by text data
//(all saved in binary file mode). Also reverses data back to classes.
//This method saves space with integer values requiring 2 bytes for numbers
//less than 10 and 3 bytes for numbers less than 100 etc. rather than using
//32/64 bit binary blocks for each number.
class Tar_Ball
{
private:
	std::string ts;
	int str_pos;
	int current_block_size;
	static const int str_size=64;
	char numstr[str_size]; //for converting numbers to strings

	bool Is_Separator(const char c);
	void Clear() { ts.clear(); }
	int Copy_To_Numstr();	
	void Separate();

public:
	Tar_Ball();

	int Get_Size() const { return current_block_size; }

	void Put(int v);
	void Put_Bool(bool b);
	void Put_Char(char c);
	void Put(int16u v);
	void Put(int32u v);
	void Put_String(std::string &str);
	
	int Get_Next_Value();
	char Get_Next_Char();
	int16u Get_Int16u();
	int32u Get_Next_Unsigned();
	bool Get_Next_Bool();
	std::string Get_Next_String();

	void Save(File *save_file);
	void Load(File *save_file);	
};

#endif
