//Legend of Saladir - colors.cpp

//Refactored 5.4.2022 - 14.4.2026 by Paul K. Pekkarinen

#include "input.h"
#include "output.h"

/* this holds the number of colors */
constexpr int MAX_COLORS=8;
int numcolors;
int colortable[MAX_COLORS*4+1];

const int ct[MAX_COLORS+1]=
{
	COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
	COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
};

const char *color_names[]=
{
	"black",
	"red",
	"green",
	"yellow",
	"blue",
	"magenta",
	"cyan",
	"white",
	"gray",
	"light red",
	"light green",
	"light yellow",
	"light blue",
	"light magenta",
	"light cyan",
	"light white",
	"black blinking",
	"red blinking",
	"green blinking",
	"yellow blinking",
	"blue blinking",
	"magenta blinking",
	"cyan blinking",
	"white blinking",
	"gray blinking",
	"light red blinking",
	"light green  blinking",
	"light yellow blinking",
	"light blue blinking",
	"light magenta blinking",
	"light cyan blinking",
	"light white blinking"
};

//Return darker version of color if it's in CH_ -range.
int get_darker_color(int ch_color)
{
	if (ch_color>=CH_DGRAY && ch_color<=CH_WHITE)
		return ch_color-8;

	return ch_color;
}

//Initialize all colors! We use color pairs 1-8 to store default colors,
//this is called at the start of program when has_colors() is tested.
void initcolorpairs()
{
	numcolors=8;

	start_color();

	for (int i=0; i<MAX_COLORS; i++)
	{
		init_pair(i+1, ct[i], COLOR_BLACK);

		colortable[i]=COLOR_PAIR(i+1);
		colortable[i+MAX_COLORS]=COLOR_PAIR(i+1)|A_BOLD;
		colortable[i+MAX_COLORS*2]=COLOR_PAIR(i+1)|A_BLINK;
		colortable[i+MAX_COLORS*3]=COLOR_PAIR(i+1)|A_BLINK|A_BOLD;
	}
}

void my_setcolor(int color)
{
	attrset(colortable[color]);
}

void set_color(int color)
{
	attrset(colortable[color]);
}

void debug_test_colors()
{
	clear_screen();

	attrset(colortable[CH_WHITE]);
	my_printf("This text should be LIGHT WHITE and the rest colors should be:");

	int x=0;
	int y=2;

	for (int i=0; i<MAX_COLORS*4; i++)
	{
		gotoxy(x, y);
		attrset(colortable[i]);
		my_printf("########## ");
		attrset(colortable[i%(MAX_COLORS*2)]);
		my_printf("%s\n", color_names[i]);

		y++;
		if (y>=SCREEN_LINES-3)
		{ 
			y=2;
			x=30;
		}
	}

	gotoxy(0, SCREEN_LINES-3);
	attrset(colortable[CH_WHITE]);
	my_printf(
		"\nIf not, report to \"ernomat@evitech.fi\" to tell what's not correct.");

	showmore(false, false);	
}
