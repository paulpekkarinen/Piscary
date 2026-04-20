//Legend of Saladir - words.h

//Unit words: Words in parser.

#ifndef WORDS_H
#define WORDS_H

struct being;

struct wordfilter
{
	const char *orig;
	const char *replace;
};

struct wordlist
{
	const char *word;
	void (*talk)(being *monster);
};

void wordtalk(being *monster, const char *word);

#endif
