//Legend of Saladir - itempack.h

//Unit itempack: Item's generation data.

#ifndef ITEMPACK_H
#define ITEMPACK_H

//Contains data to create an item.
struct Itempack
{
	int type; //item category, IS_ ...
	int subtype;
	int amount;
	int material;

	Itempack(int t, int s, int a, int m)
		: type(t), subtype(s), amount(a), material(m) { }
};

#endif
