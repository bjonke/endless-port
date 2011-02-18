#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include <windows.h>

#include "../misc/constants.h"
#include "../misc/gs_gameengine.h"
#include "../player/pod_player.h"

// ARMOR 150X210PX 6WIDTHX5EIGHT
class CGraphics
{
private:

	HWND main_window_handle;

public:

	SStateManager game_engine,menu_state;
	POINT Mouse;
	std::map<string,string> DynamicText;

	bool Init(HWND hwnd);
	bool Execute();
	void RenderViewport(int i_x, int i_y, HBITMAP i_Tiles);
	void ScrollViewPort(RECT& ViewPort);
	void EquipInventory(int i_Actor);
	void RenderInventory(int i_Actor);
	void RemoveInventory(int i_Actor);
	bool Paint();
	bool Input();
	void EraseScreen();
	void the_general_store();
	void the_bank();
	void the_guild();
	void the_tavern();
	void the_battle();
	void ValidateArea(int i_area);
	void SplashScreen();
	void CreateCharacter();
	void Menu();

	CGraphics();
	~CGraphics() {};
};
#endif