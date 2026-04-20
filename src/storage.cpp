//Legend of Saladir - storage.cpp

//Written in 8.4.2008 - 11.4.2026 by Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstdlib>
#include <cstring>
#include "file.h"
#include "storage.h"

using std::string;

Tar_Ball::Tar_Ball()
	: str_pos(0), current_block_size(0), numstr{0}
{

}

bool Tar_Ball::Is_Separator(const char c)
{
	if (c==',') return true;
	return false;
}

void Tar_Ball::Separate()
{
	ts.append(",");
}

void Tar_Ball::Put(int v)
{
	//save number data as string
	sprintf(numstr, "%d", v);

	ts.append(numstr);
	Separate();
}

void Tar_Ball::Put(int16u v)
{
	//save unsigned number data as string
	sprintf(numstr, "%u", v);

	ts.append(numstr);
	Separate();
}

void Tar_Ball::Put(int32u v)
{
	//save long unsigned number data as string
	sprintf(numstr, "%lu", v);

	ts.append(numstr);
	Separate();
}

int Tar_Ball::Copy_To_Numstr()
{
	int p=0;

	//restore number value from string, copy it to null terminated raw array
	for (int t=0; t<str_size; t++)
	{
		const char c=ts.at(str_pos++);
		if (Is_Separator(c))
		{
			numstr[p]=0;
			break;
		}
		numstr[p++]=c;
	}

	//returns zero if nothing copied
	return p;
}

int Tar_Ball::Get_Next_Value()
{
	const int p=Copy_To_Numstr();

	//change text to integer
	int rv;
	if (p>0)
		rv=atoi(numstr);
	else
		rv=0;

	return rv;
}

int16u Tar_Ball::Get_Int16u()
{
	return (int16u)Get_Next_Unsigned();
}

int32u Tar_Ball::Get_Next_Unsigned()
{
	return (int32u)Get_Next_Value();
	
	const int p=Copy_To_Numstr();

	//change text to unsigned long
	unsigned long rv;
	if (p>0)
		rv=strtoul(numstr, NULL, 0);
	else
		rv=0;

	return rv;
}

void Tar_Ball::Put_Bool(bool b)
{
	if (b==true) Put_Char('1');
	else Put_Char('0');
}

bool Tar_Ball::Get_Next_Bool()
{
	const char v=Get_Next_Char();
	if (v=='1') return true;
	return false;
}

void Tar_Ball::Put_Char(char c)
{
	ts.push_back(c);
}

char Tar_Ball::Get_Next_Char()
{
	return ts.at(str_pos++);
}

void Tar_Ball::Put_String(string &str)
{
	const int sz=(int)str.size();
	Put(sz); //saves zero if empty string

	for (int t=0; t<sz; t++)
		Put_Char(str[t]);
}

string Tar_Ball::Get_Next_String()
{
	const int sz=Get_Next_Value(); //get stored length
	if (sz<=0)
		return {}; //apparently best way to return empty string

	string s;

	for (int t=0; t<sz; t++)
		s.push_back(Get_Next_Char());

	return s;
}

void Tar_Ball::Save(File *save_file)
{
	//copy the size of save block to char array
	unsigned char fs[4];
	unsigned int sz=(long)ts.size();
	memcpy(fs, &sz, 4);

	//create temp string and copy the bytes of int there to create
	//a const buffer for save routine
	string temp_str;
	temp_str.push_back(fs[0]);
	temp_str.push_back(fs[1]);
	temp_str.push_back(fs[2]);
	temp_str.push_back(fs[3]);

	save_file->Save_Block(temp_str.c_str(), 4);
	save_file->Save_Block(ts.c_str(), (long)sz);

	Clear();
}

void Tar_Ball::Load(File *save_file)
{
	str_pos=0;
	unsigned int block_size=0;

	save_file->Load_Block((char*)&block_size, 4);
	if (block_size==0) return;

	current_block_size=block_size;

	//allocate a block for load routine and load it
	char *nb=new char[block_size];
	save_file->Load_Block(nb, block_size);

	//copy the content to string
	Clear();
	for (unsigned int t=0; t<block_size; t++) ts.push_back(nb[t]);

	delete[] nb;
}
