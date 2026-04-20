//Legend of Saladir - codex.h

//Unit codex: Misc. functions.

#ifndef CODEX_H
#define CODEX_H

#include "types.h"

void clear_flag_bit(int16u &var, int16u lippu);
void delay(int delaycount);
int inv_selectsource(int s2_x, int s2_y, int type);
void panic_exit(const char *error_text);
void set_flag_bit(int16u &var, int16u lippu);

#endif
