//Legend of Saladir - log.cpp

//Refactored 8.5.2023 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include "colors.h"
#include "log.h"
#include "messbuff.h"

Log diary;

Log::Log()
{
	log_messages=new Message_Buffer(60);
}

Log::~Log()
{
	delete log_messages;
}

void Log::Read()
{
	log_messages->Show_All("Errors");
}

void Log::Write(const char *format, ...)
{
	static char buffer[80];
	va_list ap;

	va_start(ap, format);
#ifdef _HAS_VSNPRINTF_
	vsnprintf(buffer, sizeof(mbuffer)-1, format, ap);
#else
	vsprintf(buffer, format, ap);
#endif
	va_end(ap);

	log_messages->Add(buffer, C_BLUE, 0);
}
