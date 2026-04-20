/**************************************************************************
 * filework.cpp --                                                        *
 * Author            : Erno Tuomainen (ernomat@evitech.fi)                *
 * Created on        : 21.04.1998                                         *
 * Last modified by  : Erno Tuomainen                                     *
 * date              : 24.05.1998                                         *
 **************************************************************************
 * (C) 1997, 1998 by Erno Tuomainen. All rights reserved.                 *
 *                                                                        *
 * This source is a part of the Legend of Saladir roguelike game project. *
 * The source and the derived software can only be used in non-profit     *
 * purposes. Modified sources must not be distributed without the         *
 * permission from the author. In any case, the copyright notices you see *
 * here, must be left intact.                                             *
 **************************************************************************/

//Refactored 23.6.2025 - 20.4.2026 Paul K. Pekkarinen

#include <cstdio>
#include <filesystem>
#include <fstream>
#include "filework.h"
#include "input.h"
#include "output.h"

//MS Visual C++ header for _mkdir() and _chdir()
#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif

using std::string;

namespace data
{
	const int Amt_Of_Extensions=2;

	string extensions[Amt_Of_Extensions]=
	{
		{".dat"},
		{".tmp"}
	};

	const char FILE_LOCKPRC[] = "running.sal";
	const char save_dir[]="save";
}
using namespace data;

bool change_directory(const char *dirname)
{
	#ifdef _MSC_VER
		if (_chdir(dirname)!=0) return false;
	#else
		if (chdir(dirname)!=0) return false;
	#endif

	return true;
}

bool create_directory(const char *dirname)
{
	return std::filesystem::create_directory(dirname);
}

bool check_directories()
{
	//note: doesn't check directories here, they are checked when used
	
	//note: lock file removed when testing, because this crashes

	/*
	if (testfile(FILE_LOCKPRC))
	{
		printf("In the 'data' directory there's a file \"%s\",\n",
			FILE_LOCKPRC);

		printf("which indicates that you're already playing the game.\n\n"
			"If you're not playing it then the game might have\n"
			"crashed while you were playing it. In that case you\n"
			"should delete the file \"%s\" by yourself.\n\n",
			FILE_LOCKPRC);
		return false;
	}

	//create an empty lockfile
	File lockfile(FILE_LOCKPRC);
	lockfile.Create(); //destructor of lockfile closes the file
	*/

	return true;
}

void clean_temp_files()
{
	//note: not yet needed, because no temp files other than lockprc to delete
	/*
	int errors=0;

	//delete temp files
	filesys::path dir{"."};
	for (auto &entry : filesys::directory_iterator(dir))
	{
		if (entry.is_regular_file())
		{
			// match only temp files
			if (is_temp_extension(entry.path().extension().string()))
			{
				string s=entry.path().filename().string();
				if (!deletefile(s.c_str()))
				{
					my_printf("Error while deleting \"%s\".\n", s.c_str());
					errors++;
				}
			}
		}
	}*/
}

bool deletefile(const char *filename)
{
	if (!remove(filename))
		return true;

	return false;
}

void delete_lock_file()
{
	/* remove lock file */
	//deletefile(FILE_LOCKPRC); //note: not uset at least not yet
}

bool has_matching_file_extension(const char *src, char *dest)
{
	//since 'fnmatch' is not found from Windows C library,
	//use this unsafe hand made routine
	int src_index=0;
	int dest_index=0;

	//move each index to start from .
	while (src[src_index]!='.') src_index++;
	while (dest[dest_index]!='.') dest_index++;

	//assume that extensios has . and 3 characters
	bool rv=true;
	for (int t=0; t<4; t++)
	{
		if (src[src_index]!=dest[dest_index])
		{
			rv=false;
			break;
		}
		src_index++;
		dest_index++;
	}

	return rv;
}

bool is_temp_extension(const string &source)
{
	for (int t=0; t<Amt_Of_Extensions; t++)
	{
		if (extensions[t]==source)
			return true;
	}

	return false;
}

string load_text_file(const char *filename)
{
	string line, s;
	std::ifstream myfile (filename);

	if (myfile.is_open())
	{
		while (std::getline (myfile, line))
		{
			s.append(line);

			//limit max size for safety reasons
			if (s.size()>80*3000)
			{
				s.append("Max size reached.");
				break;
			}
		}
		myfile.close();
	}

	//returns empty string if error occurs
	return s;
}

//Switch to save directory or if it doesn't exist try to create it.
bool switch_to_savedir()
{
	if (change_directory(save_dir)==false)
	{
		if (create_directory(save_dir)==false)
			return false;

		clear_screen();
		zprintf("Creating 'save' directory... If creating or switching to save "
		"directory fails you can create the save directory manually in the same "
		"(root) directory where the executable is. "
		"Press any key to continue.");
		wait_key();

		if (change_directory(save_dir)==false)
			return false;
	}

	return true;
}

/* test if file exists, return true if exists */
bool testfile(const char *filename)
{
	return std::filesystem::exists(filename);
}
