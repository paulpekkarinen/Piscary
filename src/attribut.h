//Legend of Saladir - attribut.h

//Unit attribut: Attribute, a value with max value.

#ifndef ATTRIBUT_H
#define ATTRIBUT_H

class Tar_Ball;

struct Attribute
{
	int value;
	int max_value;

	Attribute()
		: value(0), max_value(0) { }
	explicit Attribute(int v)
		: value(v), max_value(v) { }

	void Decrease(int v);
	void Initialize(int v);
	void Maximize();
	void Increase_Max(int v) { max_value+=v; }

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
