//Legend of Saladir - window.cpp

//Written in 27.9.2024 by Paul K. Pekkarinen

#include "output.h"
#include "window.h"

Window::Window(const char *hdr, int sx, int sy, int w, int h, int c, int tc)
	: header(hdr), x(sx), y(sy), width(w), height(h), color(c), text_color(tc)
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

//Draw window and display text pieces ending in /0 until $ (end of lines) found.
//note: is this type of text data ever used?
void Window::Draw(const char *txt)
{
	Draw();

	set_color(text_color);

	int dx=x+2;
	int dy=y+2;
	gotoxy(dx, dy);

	int index=0;
	while (txt[index]!='$') //display letters until $ found
	{
		const char ch=txt[index];
		if (ch==0) //if /0 move to new line
		{
			dx=x+2;
			dy++;
			gotoxy(dx, dy);
		}
		else
			addch(ch);

		index++;
	}
}
