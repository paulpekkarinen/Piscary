//Legend of Saladir - quote.h

//Unit quote: Message texts and other misc. text pieces.

#ifndef QUOTE_H
#define QUOTE_H

#include <string>

struct being;
struct Currency;
struct level_type;

std::string do_bay_bill_to(being *mptr, const Currency &c);
std::string get_coppervalue_string(const Currency &c);
std::string get_ordinal_of_time(int n);
void keeper_greet(level_type *level, being *owner, being *creat);
std::string noun_verbs_something(std::string &n, const char *verb, std::string &smt);
std::string shopkeeper_greeting(const char *owner, const char *shopname, bool visited);
std::string someone_says(const char *actor, const char *what);
std::string someone_screams(const char *actor, const char *what);

#endif
