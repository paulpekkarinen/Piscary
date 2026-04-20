//Legend of Saladir - colors.h

//Unit colors: Color definitions and functions.

#ifndef COLORS_H
#define COLORS_H

enum color_types
{
	C_BLACK, C_RED, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN,
	C_WHITE,
	CH_DGRAY, CH_RED, CH_GREEN, CH_YELLOW, CH_BLUE, CH_MAGENTA, CH_CYAN,
	CH_WHITE,
	CB_BLACK, CB_RED, CB_GREEN, CB_YELLOW, CB_BLUE, CB_MAGENTA, CB_CYAN,
	CB_WHITE,
	CHB_DGRAY, CHB_RED, CHB_GREEN, CHB_YELLOW, CHB_BLUE, CHB_MAGENTA, CHB_CYAN,
	CHB_WHITE
};

int get_darker_color(int ch_color);
void initcolorpairs();
void my_setcolor(int color); //same as set_color
void set_color(int color);
void debug_test_colors();

#endif
