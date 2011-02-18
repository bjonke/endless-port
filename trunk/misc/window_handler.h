//-------------------------------------------------------------------------
// Namn: Rickard Skeppström
// Personnr: 790930-xxxx
// Klass: SP6
// E-mail: rickard_skeppstrom@msn.com alt rickard.skeppstrom@powerhouse.net
// Lab: Labyrint
//-------------------------------------------------------------------------
#ifndef H_WINDOW_HANDLER
#define H_WINDOW_HANDLER

#include <fstream>
using namespace std;

RECT the_window;
RECT corrected_window;

RECT init_window(int left,  int top, int right, int bottom, HWND cw, POINT pt)
{
	GetClientRect(cw,&the_window);
	if(right > the_window.right)
	{
		corrected_window.right = the_window.right;		
	}
	else
	{
		corrected_window.right = pt.x + 400;
	}
	if(bottom > the_window.bottom)
	{
		corrected_window.bottom = the_window.bottom;
	}
	else
	{
		corrected_window.bottom  = pt.y + 400;
	}
	if(top < the_window.top)
	{
		corrected_window.top = the_window.top;
	}
	else
	{
		corrected_window.top = pt.y;
	}
	if(left < the_window.left)
	{
		corrected_window.left = the_window.left;
	}
	else
	{
		corrected_window.left = pt.x;
	}
	return corrected_window;
}

bool set_window(int left, int top, int right, int bottom, HWND cw)
{
	return true;
}

#endif