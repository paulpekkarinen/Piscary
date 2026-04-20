//Legend of Saladir - names.h

//Unit names: Name generation and construction.

#ifndef NAMES_H
#define NAMES_H

#include <string>
#include "types.h"

char *generate_scrollname();
const char *get_keyname(int key);
const char *get_player_killer();
std::string item_name(invnode *iptr);
std::string monster_sprintf(Actor *mptr, bool upcase, bool nameonly);
void player_killer(const char *txt); //what killed the player
bool random_language(std::string &str, int maxlen);
bool random_name(std::string &str, int maxlen);
bool random_name(char *buffer, int maxlen); //for char arrays
bool random_scrollname(char *str, int maxlen);

#endif
