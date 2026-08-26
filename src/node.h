//Legend of Saladir - node.h

//Unit node: Nodes (locations) in the game world.

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include "geometry.h"
#include "types.h"

class Feature_Level;
class Tar_Ball;

/* level flags */
#define LEVEL_VISITED    0x80

/* dungeon structure definitions & flags */
#define DUNGEON_KNOWN    0x01
#define DUNGEON_VISITED  0x80

//Stairs or border exit.
struct Portal
{
	int id;
	int dest_id;
	int terrain_type; //TYPE_DARK is border exit, others are stairs etc. type

	Portal(int i, int did, int tt)
		: id(i), dest_id(did), terrain_type(tt) { }
};

//Static data for a level.
struct Level
{
	int theme; //what kind of level this is
	int dungeon; //dungeon type of the level
	int depth; //level depth for each level in a set
	int where; //where the level is in the set

	Level() :
		theme(0), dungeon(0), depth(0), where(0) { }
	Level(int th, int du, int de, int wh)
		: theme(th), dungeon(du), depth(de), where(wh) { }

	std::string Get_Name();
	std::string Get_Data();
};

//Level location node.
class Levelnode
{
private:
	friend class World;

	level_type *level; //level of this node, zero if not visited (created)
	Level site; //basic level data
	int32u flags; //flags for the level

	//list of exits from this level to another one
	std::vector<Portal> portals;
	typedef std::vector<Portal>::iterator pitr;

	void Create_Portals(Feature_Level *f);
	Coord New_Dungeon_Location(Feature_Level *f, std::vector<Coord> &vc);

public:
	Levelnode() : level(0), flags(0) { }
	explicit Levelnode(Level lvl) : level(0), site(lvl), flags(0) { }
	~Levelnode();

	level_type *Get_Level() { return level; }
	Portal &Get_Portal(int index);
	int Get_Reverse_Portal(int terratype);
	int8u Get_Stairs_Number(int id);
	int Get_Theme() { return site.theme; }
	bool Has_Portal(int id);
	bool Is_First_Level_Of_Town();
	bool Is_Visited();

	void Add_Portal(Portal &p);
	bool Remake_Level(); //debug function to remake the current level
	bool Visit();

	void Display_Data(int i);
	void Display_Compact_Data(int i);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};

//Dungeon location node.
/*class Dungnode
{
private:
	std::vector<Levelnode*> levels; //levels of this dungeon
	int type; //type of the dungeon (also the index)
	int16u flags; // dungeon status flags, is it found etc...

	typedef std::vector<Levelnode*>::iterator levitr;

public:
	Dungnode(int dt);
	~Dungnode();

	int Get_Amount_Of_Levels();
	Levelnode *Get_Node(int index);
	Levelnode *Get_Node_By_Level(const Level *dest);

	bool Visit(int index);

	void Display_Data(bool currdung);

	void Save(Tar_Ball &tb);
	void Load(Tar_Ball &tb);
};*/

#endif
