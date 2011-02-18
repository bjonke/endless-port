#ifndef GS_GENERAL_STORE_H
#define GS_GENERAL_STORE_H

#include <iostream>
#include <sstream>

class gs_general_store
{
public:
	std::vector<RECT> button;
	std::vector<RECT> window;
	gs_general_store(){}

	void add_gui_item(int left,int top,int right,int bottom, std::string type, std::string name)
	{
		RECT dimensions = {left,top,right,bottom};
		if(type == "window")
			window.push_back(dimensions);
		else
			button.push_back(dimensions);
	}

};

#endif