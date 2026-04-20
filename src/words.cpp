/*
 * words.cpp for Legend of Saladir
 *
 * (C)1997/1998 Erno Tuomainen
 *
 */

//Refactored 25.2.2023 Paul K. Pekkarinen

#include "avatar.h"
#include "being.h"
#include "lexicon.h"
#include "output.h"
#include "talk.h"
#include "words.h"

const char *filteroutwords(const char *word);
wordlist *scanwordlist(wordlist *wlist, const char *word);

/*
 * general wordlist, words regognized by most
 * creatures (not animals)
 */
char filt_name[] = "name";
char filt_quest[] = "quest";
char filt_news[] = "news";

wordlist words_general[]=
{
   { filt_name, talk_greet },
   { filt_news, talk_rumour },
   { filt_quest, talk_quest },
   { 0, 0 },
};

/* 
 * This table is to recognize multiple words which actually
 * mean the same thing in the game
 *
 * It contains pairs of words (char pointers),
 * original written word, new word
 *
 * The routine scans this table and compares the original written
 * word to the first word in each pair in this table. If match was
 * found the original word will be replaced with the second word
 * of the pair.
 *
 */
wordfilter filtertable[] =
{
   { "name", filt_name },
   { "greet", filt_name },
   { "hello", filt_name },
   { "hi", filt_name },
   { "yo", filt_name },
   { "work", filt_quest },
   { "quest", filt_quest },
   { "job", filt_quest },
   { "news", filt_news },
   { "rumour", filt_news },
   { 0, 0 }
};

/*
 * Replace "same meaning" words with one replacement
 * See the above table "filterwords"
 *
 * returns a pointer to a word to be used in chat
 *
 */
const char *filteroutwords(const char *word)
{
	wordfilter *ptr;

	ptr = filtertable;

	while (ptr->orig)
	{
		if (my_stricmp(ptr->orig, word)==0)
		{
			return ptr->replace;
		}
		ptr++;
	}

	/* no match - return the original */
	return word;
}

/*
 * Scan the wordlist given in parameter "wlist" search for a word "word".
 * returns pointer to a wordlist entry or NULL if no match found
 *
 */
wordlist *scanwordlist(wordlist *wlist, const char *word)
{
	if (!wlist)
		return 0;

	while (wlist->word!=0)
	{
		if (my_stricmp(word, wlist->word)==0)
		{
			if (wlist->talk==0)
			{
				zprintf("Error: Talk function for word \"%s\" is NULL!\n", word);
				return 0;
			}
			else
				return wlist;
		}

		wlist++;
	}

	return 0;
}

void wordtalk(being *monster, const char *word)
{
	wordlist *wptr;

	if (!word)
	{
		ww_print("Error: Illegal talk word (=NULL).\n");
		return;
	}

	/* filter out multiple greet words */
	if (is_same_string(player.m.name, word))
		word = filt_name;

	word = filteroutwords(word);
	if (!word)
	{
		ww_print("Error in filteroutwords() (returned NULL)\n");
		return;
	}

	/* is there a match in NPC's own wordlist */
	wptr = scanwordlist(NPCchatinfo[monster->m.special].known_words, word);
	if (wptr)
	{
		wptr->talk(monster);
		return;
	}

	/* if not, is there a match in the general wordlist */
	wptr = scanwordlist(words_general, word);
	if (wptr)
	{
		wptr->talk(monster);
		return;
	}

	/* otherwise */
	talk_unknown(monster);
}
