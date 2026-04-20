//Legend of Saladir - saldebug.h

//Unit saldebug: Debug routines.

#ifndef SALDEBUG_H
#define SALDEBUG_H

//if this is defined then debug routines can be used
#define saladir_debug

class Message_Buffer;

//Debug routines.
class Debug
{
private:
	Message_Buffer *debug_messages;
	bool debug_mode; //shows extra information etc.

	void Create_Level(); //re-create current level
	void List_Objects(); //list game objects in current level
	void Overview(); //shows data about game world, current level etc.
	void View_Level(); //view revealed level map
	void View_Level_Data(); //show level statistics

public:
	Debug(bool m);
	~Debug();

	bool Cheat() const { return debug_mode; }
	
	void Identify(); //identify item in backpack
	void Menu(); //use debug commands that don't have a direct keybind
	void Message(const char *format, ...); //write a debug message
	void Set() { debug_mode=true; }
	void Toggle_Light(); //toggle debug light area of the player
	void View_Messages(); //show stored debug messages
};

extern Debug *debug;

#endif
