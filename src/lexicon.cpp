//Legend of Saladir - lexicon.cpp

//Refactored 29.9.2021 - 21.9.2025 Paul K. Pekkarinen

#include <cstring>
#include <sstream>
#include "lexicon.h"

using std::string;

int my_stricmp(const char *str1, const char *str2)
{
#ifdef _WIN32
    return _stricmp(str1, str2);
#else
    return strcasecmp(str1, str2);
#endif
}

int my_strcpy(char *dest, const char *src, int maxlen)
{
	static char error[]="Error! No source string in my_strcpy()!";

	if(dest==0 || maxlen==0)
		return 0;

	/* if no source then copy error string! */
	if(src==0)
		src=error;

	int ccount;
	for (ccount=0; ccount<maxlen-1; ccount++)
	{
		if(src[ccount]==0)
			break;

		dest[ccount]=src[ccount];

	}
	dest[ccount]=0;

	return ccount;
}

void append_string_with(string &s, const char *str, char ch)
{
	s.append(str);
	s.push_back(ch);
}

void append_string_with(string &s, string &src, char ch)
{
	s.append(src);
	s.push_back(ch);
}

int from_string(std::string &str)
{
	std::stringstream s(str);
	int rv = 0;
	s >> rv;
	return rv;
}

//Check if the letter is from range a-z or A-Z.
bool is_alpha(char ch)
{
	if (ch>='A' && ch<='Z')
		return true;
	if (ch>='a' && ch<='z')
		return true;

	return false;
}

bool is_same_string(const std::string &src, const char *dest)
{
	if (src.compare(dest)==0) return true;
	return false;
}

std::string number_of_something(const int n, const char *smthing)
{
	string s=to_string(n);
	s.append(" ");
	s.append(smthing);
	return s;
}

string to_string(int n)
{
	std::stringstream s;
	s << n;
	return s.str();
}

char uppercase(char c)
{
	if (c>='a' && c<='z') return c-32;

	return c;
}

void uppercase_first_letter(std::string &s)
{
	char c=s.at(0);
	c=uppercase(c);
	s.replace(0, 1, 1, c);
}
