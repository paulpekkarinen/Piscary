//Legend of Saladir - lexicon.h

//Unit lexicon: Generic text routines.

#ifndef LEXICON_H
#define LEXICON_H

#include <string>

/* compare incasesensitively two strings */
int my_stricmp(const char *, const char *);

/* copy a string into another, with length check */
int my_strcpy(char *dest, const char *src, int maxlen);

void append_string_with(std::string &s, const char *str, char ch);
void append_string_with(std::string &s, std::string &src, char ch);
int from_string(std::string &str); //transforms string to number
bool is_alpha(char ch);
bool is_same_string(const std::string &src, const char *dest);
std::string number_of_something(const int n, const char *smthing);
std::string to_string(int n);
char uppercase(char c);
void uppercase_first_letter(std::string &s);

#endif
