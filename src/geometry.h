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

	void Set_Location(int sx, int sy);
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

	void Resize(int sw, int sh);
};

//Rectangle has location and size.
struct Rectangle : public Plane
{
	int x;
	int y;

	Rectangle() : Plane(5, 5), x(0), y(0) { }
	Rectangle(int sx, int sy, int sw, int sh)
		: Plane(sw, sh), x(sx), y(sy) { }	
};

//Area has northwest and southeast points.
struct Area
{
	Coord nw;
	Coord se;

	Area() { }
	Area(int x, int y, int a, int b)
		: nw(x, y), se(a, b) { }

	bool Encloses(const Coord &c);
	void Shrink();

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);		
};

#endif
