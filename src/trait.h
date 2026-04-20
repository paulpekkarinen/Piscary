//trait.h

//Unit trait: Traits used in character generation.

#ifndef TRAIT_H
#define TRAIT_H

//note: Traits are currently unused.
struct Trait
{
	const char *name;
	const char *attr;
	int mod;
	int skl_gen;
	int skl_magic;
	int skl_weapon;
};

#endif
