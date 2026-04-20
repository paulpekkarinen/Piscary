//Legend of Saladir - geometry.h

//Unit geometry: Geometry types, like coordinates and plane.

#ifndef GEOMETRY_H
#define GEOMETRY_H

class Tar_Ball;

//coordinate type
struct Coord
{
	int x;
	int y;

	Coord() : x(0), y(0) { }
	Coord(int sx, int sy) : x(sx), y(sy) { }

	Coord& operator=(const Coord& right)
	{
		if(this == &right) return *this;
		x=right.x;
		y=right.y;
		return *this;
	}
	
	bool operator==(const Coord &r) const
	{
		if (x==r.x && y==r.y) return true;
		return false;
	}

	void Reset();
	void Set(int sx, int sy);
	void Move_Direction(int dir);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

/* distance & coordinate type */
struct Coordist : public Coord
{
	int dist;  /* distance, needed only while path evaluation */

	Coordist() : Coord(0, 0), dist(0) { }
	Coordist(int sx, int sy) : Coord(sx, sy), dist(0) { }
	explicit Coordist(Coordist *src);

	Coordist& operator=(const Coordist& right)
	{
		if(this == &right) return *this;
		x=right.x;
		y=right.y;
		dist=right.dist;
		return *this;
	}

	int Distance(const Coordist &dest);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

//Plane has size data.
struct Plane
{
	int width;
	int height;

	Plane() : width(0), height(0) { }
	Plane(int w, int h) : width(w), height(h) { }

	void Set(int sw, int sh);
};

//Rectangle has location and size.
struct Rectangle
{
	int x;
	int y;
	int width;
	int height;

	Rectangle() : x(0), y(0), width(5), height(5) { }
	Rectangle(int sx, int sy, int sw, int sh)
		: x(sx), y(sy), width(sw), height(sh) { }	
};

#endif
