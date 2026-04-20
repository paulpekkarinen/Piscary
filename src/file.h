/*
 * header for file.cc
 *
 * Legend of Saladir (C)1997/1998 by Erno Tuomainen
 *
 */

//Unit file: File routines.

#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include <string>

//File handling class with file name and open state.
class File
{
public:
	enum Error_Codes
	{
		Success,
		Cant_Open,
		Directory_Error
	};

private:
	FILE *file;
	std::string name;
	bool is_open;

public:
	explicit File(const char *fn);
	explicit File(const std::string &fn);
	~File();

	const char *Get_Name();

	bool Open();
	bool Create();
	void Close();

	void Load_Block(char *buffer, long size);
	bool Try_To_Load(char *buffer, long size);
	void Load(char *buffer, long size);

	void Save_Block(const char *buffer, long size);
	long Save(const char *buffer, long size);
};

#endif
