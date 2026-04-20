/**************************************************************************
 * file.cpp --                                                            *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : sometime during the autumn of 1997                 *
 * Last modified by  : Erno Tuomainen                                     *
 * Date              : 12.04.1999                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************
 *
 * 02.98 - To get Linux/Unix specific file/directory support, 'linux' must
 *         be defined when compiling Saladir.
 */

//above comment no longer relevant, linux 'style' was removed

//Refactored 23.7.2022 - 16.7.2023 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include "codex.h"
#include "file.h"

using std::string;

File::File(const char *fn)
	: file(0), name(fn), is_open(false)
{

}

File::File(const string &fn)
	: file(0), name(fn), is_open(false)
{

}

File::~File()
{
	Close();
}

const char *File::Get_Name()
{
	return name.c_str();
}

bool File::Open()
{
	bool rv;

	//if file is open then Open() and Create() do nothing
	if (is_open==false)
	{
		file=fopen(name.c_str(), "rb");
		if (file==0) rv=false;
		else is_open=rv=true;
	}
	else rv=false;

	return rv; //return true only when the file was opened (the first time)
}

bool File::Create()
{
	if (is_open==false)
	{
		const char *n=name.c_str();
		file=fopen(n, "wb");
		if (file==0) return false;

		is_open=true;
	}

	return true;
}

void File::Close()
{
	if (is_open)
	{
		fclose(file);
		is_open=false;
	}
}

void File::Load_Block(char *buffer, long size)
{
	if (fread(&buffer[0], size, 1, file)!=1)
		panic_exit("Can't load from file");
}

void File::Save_Block(const char *buffer, long size)
{
	fwrite(buffer, size, 1, file);
}

long File::Save(const char *buffer, long size)
{
	Create();
	Save_Block(buffer, size);
	Close();
	return size;
}

bool File::Try_To_Load(char *buffer, long size)
{
	bool rv;

	if (is_open) rv=true;
	else rv=Open();

	if (rv)
	{
		Load_Block(buffer, size);
		Close();
	}

	return rv;
}

void File::Load(char *buffer, long size)
{
	if (Try_To_Load(buffer, size)==false)
		panic_exit("Can't load file");
}
