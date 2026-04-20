//Legend of Saladir - log.h

//Unit log: Log of errors or other internal messages.

#ifndef LOG_H
#define LOG_H

class Message_Buffer;

class Log
{
private:
	Message_Buffer *log_messages;

public:
	Log();
	~Log();

	void Read();
	void Write(const char *format, ...);
};

//using 'diary', because VS gets confused with 'log' in math library
extern Log diary;

#endif
