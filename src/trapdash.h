//Legend of Saladir - trapdash.h

//Unit trapdash: Trigger traps and trap actions.

#ifndef TRAPDASH_H
#define TRAPDASH_H

class Actor;
struct Coord;
struct level_type;

void entrap_location(level_type *level);
bool handletrap(level_type *level, const Coord &d, Actor *monster);

#endif
