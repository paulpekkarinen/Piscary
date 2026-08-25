//Legend of Saladir - theme.h

//Unit theme: Level type.

#ifndef THEME_H
#define THEME_H

struct Plane;

//maximum level sizes x and y
#define MAXSIZEX	160
#define MAXSIZEY	60

class Theme
{
public:
	enum Level_Themes
	{
		Roomy, /* room type level */
		Roomy_Doors, /* room type level with lots of doors */
		Maze, /* maze type level */
		Old_Maze, /* deteriorated maze */
		Town, /* Town level! */
		Outworld, //outworld (mountains)
		Wildhunt /* wilderness hunt */
	};

private:
	int theme;

	static int Get_Random();

public:
	Theme(int t) : theme(t) { }

	int Get_Basetile(); //returns the tile level is cleared with
	Plane Get_Random_Level_Size();
	static int Get_Random_Level_Type(int dung, int depth, int max_depth);
};

#endif
