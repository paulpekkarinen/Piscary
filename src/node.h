//Legend of Saladir - node.h

//Unit node: Nodes (locations) in the game world.

#ifndef NODE_H
#define NODE_H

#include <vector>
#include "geometry.h"
#include "types.h"

struct Dungeon;
struct Level;
class Tar_Ball;

/* level flags */
#define LEVEL_VISITED    0x80

/* dungeon structure definitions & flags */
#define DUNGEON_KNOWN    0x01
#define DUNGEON_VISITED  0x80 

//Level location node.
class Levelnode
{
private:
	level_type *level; //level of this node, zero if not visited (created)
	Level *leveldata; //points to static level data
	int32u flags; /* flags for the level */

	int Get_Basetile(int type); //returns the tile level is cleared with
	Plane Get_Random_Level_Size(int type);
	void Display_Portal_Pair(Level *from, Level *to);

public:
	explicit Levelnode(Level *lvl) : level(0), leveldata(lvl), flags(0) { }
	~Levelnode();

	Level *Get_Level_Data() { return leveldata; }
	level_type *Get_Level() { return level; }

	bool Remake_Level(); //debug function to remake the current level
	bool Visit();

	void Display_Data(int dung);
	
	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

//Dungeon location node.
class Dungnode
{
private:
	std::vector<Levelnode*> levels; //levels of this dungeon
	Dungeon *dung; //points to static dungeon data in the 'dungeonlist'
	int16u flags; /* dungeon status flags, is it found etc... */
	int dungindex; //index of the dungeon

	typedef std::vector<Levelnode*>::iterator levitr;

public:
	Dungnode(Dungeon *d, int i);
	~Dungnode();

	int Get_Amount_Of_Levels();
	Dungeon *Get_Dungeon() { return dung; }
	Levelnode *Get_Node(int index);
	Levelnode *Get_Node_By_Level(const Level *dest);

	bool Visit(int index);

	void Display_Data(bool currdung);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

#endif
