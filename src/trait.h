//trait.h

//Unit trait: Traits used in character generation.

#ifndef TRAIT_H
#define TRAIT_H

//note: Traits are currently unused.
struct Trait
{
	const char *name;
	int affected_stat;
	int mod;
	int skl_gen;
	int skl_magic;
	int skl_weapon;
};

#endif
