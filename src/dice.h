//Legend of Saladir - dice.h

//Unit dice: Dice and randon number functions.

#ifndef DICE_H
#define DICE_H

void destroydice();
void initdice();
int throwdice(int times, int sides, int dmod);
int random_number(int rmin, int rmax); //get from minimum-maximum range
float randrealnum(float rmin, float rmax); //get float from min-max range
int RANDU(int limit);
bool sometimes();

#endif
