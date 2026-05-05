//Legend of Saladir - message.h

//Unit message: Gameplay messages.

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "colors.h"
#include "geometry.h"
#include "messbuff.h"

class Message : public Message_Buffer
{
private:	
	int msgdelay;

	char *mbuffer; //message buffer for one message

public:
	Message();
	~Message();

	void add(const char *, int); //add a message to the queue
	void add_dist(level_type *level,int,int, const char *,int,const char *,int);
	void Add_Dist(int x, int y, const char *mess1, int color1,
		const char *mess2, int color2);
	void addwait(const char *, int);
	void newmsg(const char *message); //uses default color: white
	void newmsg(const char *message, int color);
	void newmsg(std::string &s, int color);
	void newmsg(int color, const char *,...);
	void setdelay(int); //set a delay for printing
	void showall();
	void showlast();
	void vadd(int,const char *,...);
	void vaddwait(int,const char *,...);
	void vnewdist(level_type *level,int x,int y,
		  int color2, int color1,
		  const char *message2,
		  const char *format,...);
	void vnewmsg(int,const char *,...);
};

extern Message msg;

#endif
