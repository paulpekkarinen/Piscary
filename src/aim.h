//Legend of Saladir - aim.h

//Unit aim: Target something on gameview.

#ifndef AIM_H
#define AIM_H

class Actor;
struct Coord;
struct level_type;

class Aim
{
public:
	enum Aim_Modes
	{
		Look_Around	
	};
	
private:
	level_type *level;
	Actor *olento;

	void Show_Info();
	void Show_Spot_Info(const Coord &c);
	
public:
	Aim(level_type *lvl);

	bool Select();
};

#endif
