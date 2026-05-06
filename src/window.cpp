//Legend of Saladir - window.cpp

//Written in 27.9.2024 by Paul K. Pekkarinen

#include "output.h"
#include "window.h"

Window::Window(const char *hdr, int sx, int sy, int w, int h, int c, int tc)
	: Rectangle(sx, sy, w, h), header(hdr), color(c), text_color(tc)
{

}

//Draw empty window.
void Window::Draw()
{
	set_color(color);
	makeborder(x, y, width, height);
	draw_rectangle(x+1, y+1, width-2, height-2, ' ');
	
	print_toc(x+2, y, header, color);
}

//Draw window and display text in it.
void Window::Draw(const char *txt)
{
	Draw();

	set_color(text_color);

	my_wordwraptext(txt, y+1, y+height-1, x+1, x+width-2);
}
