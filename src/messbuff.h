//Legend of Saladir - message.h

//Unit message: Gameplay message list.

#ifndef MESSBUFF_H
#define MESSBUFF_H

#include <deque>
#include "geometry.h"
#include "types.h"

class Msginfo;

class Message_Buffer
{
private:
	std::deque<Msginfo*> messages;

	typedef std::deque<Msginfo*>::iterator mitr;

	const size_t Max_Messages;
	char countstr[20];
	Coord last; //last location of a message output string
	mitr origin; //current start of displayed message list

	static const char *Nullmsg;
	static const char *Shortmsg;

	void Set_Origin(Msginfo *here);
	
public:
	explicit Message_Buffer(int maxmess);
	~Message_Buffer();

	Msginfo *Get_Last_Message();
	bool Is_Empty();

	void Add(const char *message, int color, int delay);
	void Clear(); //clears all messages waiting
	void notice();
	void Show_All(const char *header);
	void update();
};

#endif
