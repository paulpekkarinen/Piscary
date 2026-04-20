//Legend of Saladir - fov.h

//Unit fov: Field-of-vision (FOV) routine.

#ifndef FOV_H
#define FOV_H

struct Coord;
struct level_type;

class Fov
{
private:
	static const int fov_multipliers[4][8];
	static const int fov_radius=20; //daylight max fov radius

	level_type *lvl; //level from where fov blocking data is processed
	int current_light; //size of the light source

	void Cast_Fov(int x, int y, int row,
		float start_slope, float end_slope, int xx, int xy, int yx, int yy);

public:
	Fov() : lvl(0) { }

	void Calculate_Fov(const Coord &oc);

	void Set_Level(level_type *l) { lvl=l; }
};

#endif
