//Legend of Saladir - window.h

//Unit window: Text window.

#ifndef WINDOW_H
#define WINDOW_H

class Window
{
private:
	const char *header;
	int x;
	int y;
	int width;
	int height;
	int color;
	int text_color;

public:
	Window(const char *hdr, int sx, int sy, int w, int h, int c, int tc);

	void Draw();
	void Draw(const char *txt); //show with content text
};

#endif
