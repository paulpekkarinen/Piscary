//Legend of Saladir - msginfo.h

//Unit msginfo: Data of a single gameplay message.

#ifndef MSGINFO_H
#define MSGINFO_H

#include <string>

class Msginfo
{
private:
	std::string msg;
	int count;
	int color;
	int delay;

public:
	Msginfo(const char *m, int c, int d)
		: msg(m), count(1), color(c), delay(d)
	{

	}

	int Get_Count() const { return count; }
	int Get_Delay_Time() const { return delay; }
	const char *Get_Raw_String() const { return msg.c_str(); }

	bool Is_Match(const char *src);

	void Set_Color();

	void Show_In_List(int i);
	void Show_Message();
};

#endif
