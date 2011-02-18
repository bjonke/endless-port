#ifndef GS_BLITTER_H
#define GS_BLITTER_H

#include <vector>
#include <algorithm>

using namespace std;

class blit_object
{
public:
	HDC hdc;
	int x;
	int y;
	int cx;
	int cy;
	HDC hdcsrc;
	int x1;
	int y1;
	DWORD rop;
	int Z;
};


bool MyDataSortPredicate(const blit_object& d1, const blit_object& d2)
{
	return d1.Z < d2.Z;
}


class gs_BLITTER
{
private:
	std::vector<blit_object> render_obj; 
public:


	gs_BLITTER()
	{
	};

	void queue_render(HDC i_hdc,std::vector<GUIObject> i_obj, UINT i_edge, UINT i_flags,std::string i_type, std::map<string,string> i_map)
	{
		for(unsigned int obj_index = 0; obj_index < i_obj.size(); obj_index++)
		{
			if( i_type == i_obj.at(obj_index).location )
			{
				if( i_obj.at(obj_index).text == "DESCRIPTION")
				{
					SIZE textsize    = {0, 0};
					GetTextExtentPoint(BackBuffer,i_obj.at(obj_index).text.c_str(),strlen(i_obj.at(obj_index).text.c_str()),&textsize);

					
										SelectObject(i_hdc, GetStockObject(WHITE_BRUSH));
					RoundRect(i_hdc, i_obj.at(obj_index).area.left, i_obj.at(obj_index).area.top, 
						i_obj.at(obj_index).area.right, i_obj.at(obj_index).area.bottom, 
                    20, 20);

					i_obj.at(obj_index).area.left += 5;
					i_obj.at(obj_index).area.top += 5;
					i_obj.at(obj_index).area.right -= 5;
					i_obj.at(obj_index).area.bottom -= 5;

							DrawText(i_hdc,
								i_map[i_obj.at(obj_index).text].c_str(),
								strlen(i_map[i_obj.at(obj_index).text].c_str()),
								&i_obj.at(obj_index).area,
								DT_WORDBREAK);
					return;
				}

				SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
				TransparentBlt(BackBuffer, 
					GUIObjects.at(obj_index).area.left, 
					GUIObjects.at(obj_index).area.top, 
					260/2, 
					50/2, 
					Tiles_DC, 
					528, 
					(52*5)/2,
					260/2,
					52/2,
					RGB(255,0,255));

				// KEY =STATUSBAR, VALUE = 1000
				if( i_map.end() == i_map.find(i_obj.at(obj_index).text) )
				{
					if( i_obj.at(obj_index).text != "X")
					{
						SIZE textsize    = {0, 0};
						GetTextExtentPoint(BackBuffer,i_obj.at(obj_index).text.c_str(),strlen(i_obj.at(obj_index).text.c_str()),&textsize);

											i_obj.at(obj_index).area.left += 5;
					i_obj.at(obj_index).area.top += 5;

						DrawText(i_hdc,
							i_obj.at(obj_index).text.c_str(),
							strlen(i_obj.at(obj_index).text.c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
						//if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }
					}
				}
				else
				{
					i_obj.at(obj_index).area.left += 5;
					i_obj.at(obj_index).area.top += 5;

					DrawText(i_hdc,
							i_map[i_obj.at(obj_index).text].c_str(),
							strlen(i_map[i_obj.at(obj_index).text].c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
				}

				//if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }
			}
		}
	}

	void queue_edge(HDC i_hdc,std::vector<GUIObject> i_obj, UINT i_edge, UINT i_flags,std::string i_type, std::map<string,string> i_map)
	{
		for(unsigned int obj_index = 0; obj_index < i_obj.size(); obj_index++)
		{
			if( i_type == i_obj.at(obj_index).location )
			{
				// KEY =STATUSBAR, VALUE = 1000
				if( i_map.end() == i_map.find(i_obj.at(obj_index).text) )
				{
					if( i_obj.at(obj_index).text != "X")
					{
						SIZE textsize    = {0, 0};
						GetTextExtentPoint(BackBuffer,i_obj.at(obj_index).text.c_str(),strlen(i_obj.at(obj_index).text.c_str()),&textsize);

										SelectObject(i_hdc, GetStockObject(WHITE_BRUSH));
					RoundRect(i_hdc, i_obj.at(obj_index).area.left, i_obj.at(obj_index).area.top, 
						i_obj.at(obj_index).area.right, i_obj.at(obj_index).area.bottom, 
                    20, 20);

										i_obj.at(obj_index).area.left += 5;
					i_obj.at(obj_index).area.top += 5;
	
						DrawText(i_hdc,
							i_obj.at(obj_index).text.c_str(),
							strlen(i_obj.at(obj_index).text.c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
						if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }
					}
				}
				else
				{
										SelectObject(i_hdc, GetStockObject(WHITE_BRUSH));
					RoundRect(i_hdc, i_obj.at(obj_index).area.left, i_obj.at(obj_index).area.top, 
						i_obj.at(obj_index).area.right, i_obj.at(obj_index).area.bottom, 
                    20, 20);

										i_obj.at(obj_index).area.left += 5;
					i_obj.at(obj_index).area.top += 5;

						DrawText(i_hdc,
							i_map[i_obj.at(obj_index).text].c_str(),
							strlen(i_map[i_obj.at(obj_index).text].c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
				}

				if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }

				if( i_obj.at(obj_index).text == "NEXT" || i_obj.at(obj_index).text == "PREV" || i_obj.at(obj_index).text == "SELL" || i_obj.at(obj_index).text == "BUY")
				{
				SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
				TransparentBlt(BackBuffer, 
					GUIObjects.at(obj_index).area.left, 
					GUIObjects.at(obj_index).area.top, 
					260/2, 
					50/2, 
					Tiles_DC, 
					528, 
					(52*5)/2,
					260/2,
					52/2,
					RGB(255,0,255));


						DrawText(i_hdc,
							i_obj.at(obj_index).text.c_str(),
							strlen(i_obj.at(obj_index).text.c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
				}
				else
				{
					//DrawEdge(i_hdc,&i_obj.at(obj_index).area,i_edge,i_flags);
				}
			}
		}
	}

	void queue_menu(HDC i_hdc,std::vector<GUIObject> i_obj, UINT i_edge, UINT i_flags,std::string i_type, std::map<string,string> i_map)
	{
		for(unsigned int obj_index = 0; obj_index < i_obj.size(); obj_index++)
		{
			if( i_type == i_obj.at(obj_index).location )
			{
				if(i_obj.at(obj_index).location == "GUILD")
				{
				SelectObject(Tiles_DC,ImageMap["data/images/buttonsmisc2.bmp"].gs_hbmp);
				TransparentBlt(BackBuffer, 
					Screen.right/3, 
					GUIObjects.at(obj_index).area.top, 
					300, 
					50, 
					Tiles_DC, 
					0, 
					0,
					300,
					51,
					RGB(255,0,255));
				}
				else
				{

					int ButtonWidth = 300;
					int ButtonHeight = 51;
				SelectObject(Tiles_DC,ImageMap["data/images/buttonsmisc2.bmp"].gs_hbmp);
				TransparentBlt(BackBuffer, 
					i_obj.at(obj_index).x - (ButtonWidth / 2), 
					i_obj.at(obj_index).y - (ButtonHeight / 2), 
					ButtonWidth, 
					ButtonHeight, 
					Tiles_DC, 
					0, 
					0,
					ButtonWidth,
					ButtonHeight,
					RGB(255,0,255));
				}
				// KEY =STATUSBAR, VALUE = 1000
				if( i_map.end() == i_map.find(i_obj.at(obj_index).text) )
				{
					if( i_obj.at(obj_index).text != "X")
					{
						SIZE textsize    = {0, 0};
						GetTextExtentPoint(BackBuffer,i_obj.at(obj_index).text.c_str(),strlen(i_obj.at(obj_index).text.c_str()),&textsize);
											int ButtonWidth = 300;
					int ButtonHeight = 51;

						i_obj.at(obj_index).area.left = i_obj.at(obj_index).x - (ButtonWidth / 2);
						i_obj.at(obj_index).area.top = i_obj.at(obj_index).y - (ButtonHeight / 2);
						i_obj.at(obj_index).area.right = i_obj.at(obj_index).x + (ButtonWidth / 2);
						i_obj.at(obj_index).area.bottom = i_obj.at(obj_index).y + (ButtonHeight / 2);
						DrawText(i_hdc,
							i_obj.at(obj_index).text.c_str(),
							strlen(i_obj.at(obj_index).text.c_str()),
							&i_obj.at(obj_index).area,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }
					}
				}
				else
				{

						DrawText(i_hdc,
							i_map[i_obj.at(obj_index).text].c_str(),
							strlen(i_map[i_obj.at(obj_index).text].c_str()),
							&i_obj.at(obj_index).area,
							DT_WORDBREAK);
				}

				if ( ( (GetAsyncKeyState(VK_END)) ? 1 : 0) ) { exit(0); }

				//DrawEdge(i_hdc,&i_obj.at(obj_index).area,i_edge,i_flags);
			}
		}
	}

	int queue(HDC i_hdc, int i_x, int i_y, int i_cx, int i_cy, HDC i_hdcsrc, int i_x1, int i_y1, DWORD i_rop, int i_Z)
	{
		blit_object temp_obj;
		temp_obj.hdc = i_hdc;
		temp_obj.x = i_x;
		temp_obj.y = i_y;
		temp_obj.cx = i_cx;
		temp_obj.cy = i_cy;
		temp_obj.hdcsrc = i_hdcsrc;
		temp_obj.x1 = i_x1;
		temp_obj.y1 = i_y1;
		temp_obj.rop = i_rop;
		temp_obj.Z = i_Z;
		render_obj.push_back(temp_obj);

		if(temp_obj.Z == 0)
		{
			std::sort(render_obj.begin(), render_obj.end(), MyDataSortPredicate);
			temp_obj = render_obj.back();
			render_obj.pop_back();
			StretchBlt(
				temp_obj.hdc, 
				temp_obj.x, 
				temp_obj.y, 
				temp_obj.cx, 
				temp_obj.cy, 
				temp_obj.hdcsrc, 
				temp_obj.x1, 
				temp_obj.y1,
				temp_obj.cx,
				temp_obj.cy,
				temp_obj.rop);
			/*
			BitBlt(
				temp_obj.hdc, 
				temp_obj.x, 
				temp_obj.y, 
				temp_obj.cx, 
				temp_obj.cy, 
				temp_obj.hdcsrc, 
				temp_obj.x1, 
				temp_obj.y1, 
				temp_obj.rop
				);*/
		}
		return 0;
	}

	void tqueue(HDC i_hdc, int i_x, int i_y, int i_cx, int i_cy, HDC i_hdcsrc, int i_x1, int i_y1, DWORD i_rop, int i_Z)
	{
		blit_object temp_obj;
		temp_obj.hdc = i_hdc;
		temp_obj.x = i_x;
		temp_obj.y = i_y;
		temp_obj.cx = i_cx;
		temp_obj.cy = i_cy;
		temp_obj.hdcsrc = i_hdcsrc;
		temp_obj.x1 = i_x1;
		temp_obj.y1 = i_y1;
		temp_obj.rop = i_rop;
		temp_obj.Z = i_Z;
		render_obj.push_back(temp_obj);

		if(temp_obj.Z == 0)
		{
			std::sort(render_obj.begin(), render_obj.end(), MyDataSortPredicate);
			temp_obj = render_obj.back();
			render_obj.pop_back();
			TransparentBlt(
				temp_obj.hdc, 
				temp_obj.x, 
				temp_obj.y, 
				temp_obj.cx, 
				temp_obj.cy, 
				temp_obj.hdcsrc, 
				temp_obj.x1, 
				temp_obj.y1,
				temp_obj.cx,
				temp_obj.cy,
				temp_obj.rop
				);
		}
	}

	void finish()
	{
		BitBlt(
			gDC, 
			0, 
			0, 
			GetSystemMetrics(SM_CXSCREEN), 
			GetSystemMetrics(SM_CYSCREEN), 
			BackBuffer, 
			0, 
			0, 
			SRCCOPY
			);
	}

	~gs_BLITTER() {};
};
gs_BLITTER Blitter;
#endif