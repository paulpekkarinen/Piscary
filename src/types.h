//Legend of Saladir - types.h

//Unit types: Type definitions for internal language types.

#ifndef TYPES_H
#define TYPES_H

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!                                                              !
! NOTE: DEFINE LONT64B if your system has 64 bits wide longint !
!                                                              !
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#undef LONG64B

typedef double          real;
typedef unsigned char   int8u;
typedef unsigned short  int16u;

#ifdef LONG64B
typedef unsigned int    int32u;
typedef signed int      int32s;
#else
typedef unsigned long   int32u;
typedef signed long     int32s;
#endif

//some forward declarations here, since we are going to need them anyway
class Actor;
struct being;
struct Coord;
class equipment;
class inventory;
struct invnode;
struct item_def;
struct level_type;
struct monsterdef;
struct Plane;
class Tar_Ball;

#endif
