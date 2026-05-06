//Legend of Saladir - window.h

//Unit window: Text window.

#ifndef WINDOW_H
#define WINDOW_H

#include "geometry.h"

class Window : public Rectangle
{
private:
	const char *header;
	int color;
	int text_color;

public:
	Window(const char *hdr, int sx, int sy, int w, int h, int c, int tc);

	void Draw();
	void Draw(const char *txt);
};

#endif
