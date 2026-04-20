//Legend of Saladir - inquire.h

//Unit inquire: Learn about things.

#ifndef INQUIRE_H
#define INQUIRE_H

class Actor;
struct level_type;
struct playerinfo;

//search results
#define FOUND_NOTHING        0
#define FOUND_SECRETDOOR     1
#define FOUND_SECRETCORRIDOR 2
#define FOUND_DOORTRAP       3
#define FOUND_CAVETRAP       4

void lookdir(playerinfo *plr, level_type *level);
void read_scroll(playerinfo *plr, level_type *level);
bool search_surroundings(Actor *tonttu, level_type *level, bool automatic);

#endif
