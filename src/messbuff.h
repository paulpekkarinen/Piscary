//Legend of Saladir - message.h

//Unit message: Gameplay message list.

#ifndef MESSBUFF_H
#define MESSBUFF_H

#include <deque>
#include "types.h"

class Msginfo;

class Message_Buffer
{
private:
	friend class Message;

	std::deque<Msginfo*> messages;

	typedef std::deque<Msginfo*>::iterator mitr;

	const size_t Max_Messages;
	int messages_per_turn;

	static const char *Nullmsg;
	static const char *Shortmsg;

protected:
	Msginfo *Get_Last_Message();

	void Clear(); //clears all messages waiting

public:
	explicit Message_Buffer(int maxmess);
	~Message_Buffer();

	bool Is_Empty();

	void Add(const char *message, int color, int delay);

	void Show_All(const char *header);
};

#endif
