//Legend of Saladir - output.h

//Unit output: Output routines.

#ifndef OUTPUT_H
#define OUTPUT_H

//possibly need to set this when using pdcurses.dll on Windows,
//has to be defined before including curses.h
//#define PDC_DLL_BUILD

#include <curses.h>
#include "colors.h"
#include "types.h"

//Override some keyboard codes for curses, which by nature are chaotic.
//Note that sometimes 'backspace' in some terminals wont be 8.
#define KEY_ESC			27
#undef KEY_ENTER
#define KEY_ENTER		10
//#undef KEY_BACKSPACE
//#define KEY_BACKSPACE	8

#ifdef __linux__
#define PADENTER           KEY_ENTER
#define PADMINUS           '-'
#define PADPLUS            '+'
#endif

#ifdef __APPLE__
#define PADENTER        KEY_ENTER
#endif

/* curses direct codes (with keypad set TRUE) for Function keys */
#define MYKEY_CURSESF1       KEY_F(1)
#define MYKEY_CURSESF20      KEY_F(20)

#define MYKEY_F1           KEY_F(1)
#define MYKEY_F2           KEY_F(2)
#define MYKEY_F3           KEY_F(3)
#define MYKEY_F4           KEY_F(4)
#define MYKEY_F5           KEY_F(5)
#define MYKEY_F6           KEY_F(6)
#define MYKEY_F7           KEY_F(7)
#define MYKEY_F8           KEY_F(8)
#define MYKEY_F9           KEY_F(9)
#define MYKEY_F10          KEY_F(10)

/* since curses doesn't provide symbolic defines for
 * CTRL-keys, I've defined my own
 */
#define MYKEY_CTRLA        1
#define MYKEY_CTRLB        2
#define MYKEY_CTRLC        3     /* NOT USABLE, BREAK */
#define MYKEY_CTRLD        4
#define MYKEY_CTRLE        5
#define MYKEY_CTRLF        6
#define MYKEY_CTRLG        7
#define MYKEY_CTRLH        8
#define MYKEY_CTRLI        9
#define MYKEY_CTRLJ        10
#define MYKEY_CTRLK        11
#define MYKEY_CTRLL        12
#define MYKEY_CTRLM        13
#define MYKEY_CTRLN        14
#define MYKEY_CTRLO        15
#define MYKEY_CTRLP        16
#define MYKEY_CTRLQ        17
#define MYKEY_CTRLR        18
#define MYKEY_CTRLS        19
#define MYKEY_CTRLT        20
#define MYKEY_CTRLU        21
#define MYKEY_CTRLV        22
#define MYKEY_CTRLW        23
#define MYKEY_CTRLX        24
#define MYKEY_CTRLY        25
#define MYKEY_CTRLZ        26

//size of the screen in characters
#define SCREEN_COLS 80
#define SCREEN_LINES 24

//map window size
#define MAPWIN_SIZEX (SCREEN_COLS-20)
#define MAPWIN_SIZEY (SCREEN_LINES-5)

//minimum size of a level is the size of map window
#define MINSIZEX MAPWIN_SIZEX
#define MINSIZEY MAPWIN_SIZEY

/* relative position of MAP window */
#define MAPWIN_RELX	(SCREEN_COLS-MAPWIN_SIZEX)
#define MAPWIN_RELY	2

//was +1 for both
#define MAPWIN_X MAPWIN_RELX
#define MAPWIN_Y MAPWIN_RELY

// y location of status row
#define STATUSROW	(MAPWIN_RELY+MAPWIN_SIZEY)
#define MSGLINE		0
#define MSGLINES	2

char decidelinechar(int ox, int oy, int nx, int ny);
Coord get_cursor();
int get_cursor_x();
int get_cursor_y();
bool is_over_border(int word_length);
void clearline(int y);
void clear_lines(int from_y, int to_y);
void clear_screen();
void disable_scroll();
void drawborder(int x1, int y1, int x2, int y2);
void drawline(int y, char chr);
void drawline_limit(int y, int x1, int x2, char chr);
void draw_rectangle(int x, int y, int w, int h, char ch);
void enable_scroll();
void gotoxy(int x, int y);
void goto_content();
void hidecursor();
void makeborder(int x1, int y1, int wd, int hg);
void my_center_puts(int y, const char *fmt, ...);
void my_cputs_init(int color, bool cond, const char *str);
void my_printf(const char *fmt, ...);
void my_wordwraptext(const char *dptr, int top, int bottom, int left, int right);
void print_centered(int y, const char *txt);
void print_text(const char *txt);
void print_text_to(int x, int y, const char *txt);
void print_toc(int x, int y, const char *txt, int color);
void put_char(char ch, int color);
void put_char_to(char ch, const Coord &c);
void showcursor();
void ww_print(const char *txt);
void zprintf(const char *format, ...);

#endif
