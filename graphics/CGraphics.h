#ifndef CGRAPHICS_H
#define CGRAPHICS_H 1

#include <windows.h>

#include "../XMLLogger/Source/MyLogHandler.h"
#include "../misc/constants.h"
#include "../misc/gs_gameengine.h"
// ARMOR 150X210PX 6WIDTHX5EIGHT
class CGraphics
{
private:

	HWND main_window_handle;

public:

	SStateManager game_engine,menu_state;
	POINT Mouse;
	std::map<string,string> DynamicText;
	std::string actor_name;
	bool EditName;

	bool Init(HWND hwnd);
	bool Execute(bool Focused);
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
	void Outro();
	void CreateCharacter();
	void Menu();
	void MenuOptions();
	void HallOfFame();
	void CharacterList();
	void CompanionList();
	void SpellList();
	void City();
	void Guild();
	void CallbackFox   (int Param);
	CGraphics();
	~CGraphics() {};
};
#endif