//Legend of Saladir - use.h

//Unit use: Use of objects like doors, traps etc.

#ifndef USE_H
#define USE_H

class Actor;
struct Coord;
struct level_type;
struct playerinfo;

/* door "error" codes */
#define DOORSTAT_SUCCESS        0 /* operation succesful */
#define DOORSTAT_ALREADY_OPEN   1 /* is already open */
#define DOORSTAT_NODOOR	        2 /* is no door */
#define DOORSTAT_LOCKED         3 /* door is locked */
#define DOORSTAT_STUCK	        4 /* door is locked */
#define DOORSTAT_ALREADY_CLOSED 5 /* is already closed */

void close_door(playerinfo *plr, level_type *level);
int close_door(Actor *tonttu, level_type *level, const Coord &c);
bool disarm_trap(playerinfo *plr, level_type *level);
void open_door(playerinfo *plr, level_type *level);
int open_door(Actor *tonttu, level_type *level, const Coord &c);

#endif
