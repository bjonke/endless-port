#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cmath>
#include <map>
#include <strstream>
#include <crtdbg.h>
#include <algorithm>
#include <cctype>

using namespace std;
#include "../graphics/CGraphics.h"
#include "../misc/Library.h"

#include "../misc/globals.h"
#include "../font/gs_FONT.h"
#include "../graphics/gs_BLITTER.h"
#include "../misc/CInput.h"
#include "../graphics/SDevice.h"
#include "../CGameScreen.h"
#include "../HP_Render.h"
#include "../SAnimation.h"
#include "../SDrawFace.h"
#include "../sockan.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")

// Log related information
#define COMPILE_TIMESTAMP __DATE__ " " __TIME__
#define FILE_INFO __FILE__, __LINE__
CMyLogHandler LogHandler(".\\XMLLogger\\Log\\CGraphics.xml");
// end log related information

SDevice GfxDevice;

CHallOfFame cHallOfFame;
list<CGameScreen*> GameScreens;
GS_CSV gs_csv;

bool CGraphics::Init(HWND hwnd)
{
	LogHandler.AddTagTree("Project");
	LogHandler.AddTag("Name", "CGraphics");

	LogHandler.AddTag("Type", "Project");
	LogHandler.AddTag("FileName", __FILE__);
	LogHandler.AddTag("Compiled", COMPILE_TIMESTAMP);

	LogHandler.AddTagTree("CommentLines");
	LogHandler.AddTag("CommentLine", "Handles most of the game funtionality");
	LogHandler.CloseTagTree();	// CommentLines

	LogHandler.AddTagTree("LogEntries");
	LogHandler.WriteLog("Main", "main", "Info", "Starting up", FILE_INFO);

	// Base graphic setup
	main_hwnd = hwnd;
	GetClientRect(main_hwnd,&Screen);
	gDC = GetDC(main_hwnd);

	// För BackBuffer (Dubbel) buffring
	BackBuffer = CreateCompatibleDC(gDC);
	BackBuffer_bmp = CreateCompatibleBitmap(gDC, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	Old_bmp = (HBITMAP)SelectObject(BackBuffer, BackBuffer_bmp);

	Tiles_DC = CreateCompatibleDC(NULL);
	// Base ends here

	const int DECREASE_PLAYER_HEALTH = 0,DECREASE_PLAYER_STAMINA = 1,DECREASE_PLAYER_LEVEL = 2;
	const int INCREASE_ACTOR_ATTACK_POWER = 3,INCREASE_ACTOR_DAMAGE=4, EQUIP = 5;
	
	backpack.push_back(new CObjPotion);
	backpack.push_back(new CObjSword);
	backpack.push_back(new CObjBow);

	Beastiary.push_back(new Ant);
	Beastiary.push_back(new Antelope);
	Beastiary.push_back(new Amphisbaena);
	Beastiary.push_back(new Alerion);

	ActorCompanions.push_back(Beastiary.at(0));
	ActorCompanions.push_back(Beastiary.at(1));
	ActorCompanions.push_back(Beastiary.at(2));
	ActorCompanions.push_back(Beastiary.at(3));

	backpack.at(0)->name = "Blue potion";
	backpack.at(0)->description = "This is a mysterious blue potion";
	backpack.at(0)->scripts.push_back(DECREASE_PLAYER_HEALTH);
	backpack.at(0)->scripts.push_back(DECREASE_PLAYER_STAMINA);
	backpack.at(0)->scripts.push_back(DECREASE_PLAYER_LEVEL);

	backpack.at(1)->name = "Long sword";
	backpack.at(1)->description = "This is a magic long sword";
	backpack.at(1)->scripts.push_back(INCREASE_ACTOR_ATTACK_POWER);
	backpack.at(1)->scripts.push_back(INCREASE_ACTOR_DAMAGE);
	backpack.at(1)->scripts.push_back(EQUIP);
	backpack.at(1)->valid_actions.push_back(EQUIP);

   for ( unsigned int i = 0; i < backpack.size(); i++)
   {
      //delete backpack.at(i);
   }	

	//CInventory* pack = NULL;
	//pack = new CInventory;
	//InventoryTable packTable;
	//delete pack;

	menu_state.Init(AreaType::MENU);
	glob_anim_frame_x = glob_anim_frame_y = 0;
	
	LogHandler.WriteLog("CGraphics", "Init", "Info", menu_state.get_state(), FILE_INFO);
	LogHandler.WriteLog("CGraphics", "Init", "Info", "glob_anim_frame_x + y", FILE_INFO);
	LogHandler.WriteLog("CGraphics", "Init", "Info", glob_anim_frame_x, FILE_INFO);
	LogHandler.WriteLog("CGraphics", "Init", "Info", "main_hwnd == hwnd", FILE_INFO);
	
	// Related to Paint function for rendering the map
	
	//Setting size of viewport
	SetRect(&ViewPort,0,0,Screen.right,Screen.bottom);

	// Writes all the bmp filenames into a file
	FileData("data/images/*.bmp","data/logs/images.txt" );

	LogHandler.WriteLog("CGraphics", "Init", "Info", "FileData(data/logs/images.txt)", FILE_INFO);

	// Loades all the data for the tiles
	fmap.CreateTiles();

	LogHandler.WriteLog("CGraphics", "Init", "Info", "Creating tiles", FILE_INFO);
	
	FileManager.FileLoader(Spells.m_File,Spells.m_Vector,"data/spells/spells.txt");
	FileManager.FileLoader(Items.m_File,Items.m_Vector,"data/items/items.txt");
	FileManager.FileLoader(Quests.m_File,Quests.m_Vector,"data/quests/quests.txt");
	FileManager.FileLoader(Guilds.m_File,Guilds.m_Vector,"data/guilds/guilds.txt");
	FileManager.FileLoader(Images.m_File,Images.m_Vector,"data/gfx/images/images.txt");
	FileManager.FileLoader(Actors.m_File,Actors.m_Vector,"data/actors/actors.txt");
	FileManager.FileLoader(Monsters.m_File,Monsters.m_Vector,"data/monsters/monsters.txt");
	FileManager.FileLoader(Gui.m_File,Gui.m_Vector,"data/gui/gui.txt");

	int iGUIObjects = 0;
	for(unsigned int obj_index = 0; obj_index < Gui.m_Vector.size()-1; obj_index++ )
	{
		GUIObject tmp_obj;
		/*RECT tmp_rect = {
			atoi(Gui.m_Vector.at(obj_index).at(0).c_str()),
			atoi(Gui.m_Vector.at(obj_index).at(1).c_str()),
			atoi(Gui.m_Vector.at(obj_index).at(2).c_str()),
			atoi(Gui.m_Vector.at(obj_index).at(3).c_str())
		};*/

		RECT tmp_rect = {
			atoi(Gui.m_Vector.at(obj_index).at(8).c_str()) - 150,
			atoi(Gui.m_Vector.at(obj_index).at(9).c_str()) - 25,
			atoi(Gui.m_Vector.at(obj_index).at(8).c_str()) + 150,
			atoi(Gui.m_Vector.at(obj_index).at(9).c_str()) + 25
		};

		tmp_obj.area = tmp_rect;
		tmp_obj.z = atoi(Gui.m_Vector.at(obj_index).at(4).c_str());
		tmp_obj.action = Gui.m_Vector.at(obj_index).at(5).c_str();
		tmp_obj.location = Gui.m_Vector.at(obj_index).at(6).c_str();
		tmp_obj.text = Gui.m_Vector.at(obj_index).at(7).c_str();
		tmp_obj.x = atoi(Gui.m_Vector.at(obj_index).at(8).c_str());
		tmp_obj.y = atoi(Gui.m_Vector.at(obj_index).at(9).c_str());
		GUIObjects.push_back(tmp_obj);
		iGUIObjects++;
	}

	LogHandler.WriteLog("CGraphics", "Init", "Info", "Initializing GUI objects", FILE_INFO);
	LogHandler.WriteLog("CGraphics", "Init", "Info", iGUIObjects, FILE_INFO);

	std::vector<GUIObject>::iterator it;
	std::list<GUIObject> l(GUIObjects.begin(),GUIObjects.end());
	sort(GUIObjects.begin(),GUIObjects.end(),zDataSortPredicate);

	SActors.push_back(SActor());
	
	SActors.at(0).m_IsActive = true;

	SActors.at(0).IMods.Reset();
	SActors.at(0).GMods.Reset();
	SActors.at(0).VInventory.push_back(backpack.at(1));
	
	Text.Initialize();

	for(unsigned int i = 0; i < Images.m_Vector.size()-1; i++)
	{
		// new bitmap loading code
		GS_BITMAPS tmpBMP;

		tmpBMP.gs_hbmp = reinterpret_cast<HBITMAP>(LoadImage(0,Images.m_Vector.at(i).at(0).c_str(),
			IMAGE_BITMAP,0,0,
            LR_LOADFROMFILE));

		GetObject(tmpBMP.gs_hbmp,sizeof(BITMAP), &tmpBMP.gs_bmp);
		ImageMap[Images.m_Vector.at(i).at(0).c_str()] = tmpBMP;

		EraseScreen();

		int NewFrame = Images.m_Vector.size() / 30;
		static int CurrentFrame = 0;
		static int ImageID = 0;
		if( CurrentFrame >= NewFrame )
		{
			CurrentFrame = 0;
			ImageID++;
		}
		else
			++CurrentFrame;
		stringstream ss;
		ss << setw( 3 ) << setfill( '0' ) << ImageID;
		string ImagePath = BaseImagePath + "genetica/intro" + ss.str()+".bmp" ;
		SelectObject(Tiles_DC,ImageMap[ImagePath].gs_hbmp);

		TransparentBlt(BackBuffer, 
			0, 
			0, 
			Screen.right, 
			Screen.bottom, 
			Tiles_DC, 
			0, 
			0,
			ImageMap[ImagePath].gs_bmp.bmWidth,
			ImageMap[ImagePath].gs_bmp.bmHeight,
			RGB(0,0,0));

		RECT LoadingBar = {0,Screen.bottom - 48,(Screen.right / Images.m_Vector.size())*(i+1),Screen.bottom - 24};
		RECT LoadingBarBg = {0,Screen.bottom - 48,Screen.right,Screen.bottom - 24};
		FillRect(BackBuffer,&LoadingBarBg,CreateSolidBrush(RGB(0,0,0)));
		FillRect(BackBuffer,&LoadingBar,CreateSolidBrush(RGB(255,0,0)));

		DrawText(BackBuffer,Images.m_Vector.at(i).at(0).c_str(),strlen(Images.m_Vector.at(i).at(0).c_str()), &LoadingBarBg, DT_CENTER | DT_VCENTER);

		LogHandler.WriteLog("CGraphics", "Init", "Info", "Initiating image", FILE_INFO);
		LogHandler.WriteLog("CGraphics", "Init", "Info", Images.m_Vector.at(i).at(0).c_str(), FILE_INFO);
		Blitter.finish();
	}	
	ActorCompanions.at(0)->SetImage(ImageMap[BaseAnimationPath + "finalfantasy6.bmp"]);

	//GameScreens.push_front(new CMenuScreen);
	//GameScreens.push_front(new CPauseScreen);

	LogHandler.CloseTagTree(); // testEntries

	ClockInit=clock();
	return true;
}

// Get the id for the active actor
int GetActiveActor(std::vector<SActor>& i_SActors)
{
	for(unsigned int i = 0; i < i_SActors.size(); ++i)
	{
		if( i_SActors.at(i).m_IsActive )
		{
			return i;
		}
	}
	return 0;
}

bool CGraphics::Execute(bool Focused)
{	
	EraseScreen();


	//SelectObject(Tiles_DC,ImageMap["data/images/Title BG.bmp"].gs_hbmp);

	//GameScreens.front()->Update();

	//GameScreens.front()->Draw();
	//GameScreens.pop_front();
	//GameScreens.front()->Draw();
	
	//std::list <CGameScreen*>::iterator c1_Iter, c2_Iter;
	//for (c1_Iter = GameScreens.begin(); c1_Iter != GameScreens.end(); c1_Iter++) {
	//	(*c1_Iter)->Draw(Tiles_DC,BackBuffer,gs_tile.gs_hbmp[31],gs_tile.gs_bmp[31]);
	//	(*c1_Iter)->Update();
	//}
			//BitBlt(
			//gDC, 
			//0, 
			//0, 
			//GetSystemMetrics(SM_CXSCREEN), 
			//GetSystemMetrics(SM_CYSCREEN), 
			//BackBuffer, 
			//0, 
			//0, 
			//SRCCOPY
			//);
	//int tmp = 0;
	//SActors.at(GetActiveActor(SActors)).IMods.Reset();
	//for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
	//	citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
	//{
	//	if( SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) == 1 && _stricmp( Items.m_Vector.at(*citer).at(3).c_str(), "ATTACK" ) == 0 )
	//	{
	//		SActors.at(GetActiveActor(SActors)).IMods.ImproveAttack(atoi(Items.m_Vector.at((*citer)).at(4).c_str()));
	//	}
	//	if( SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) == 1 && _stricmp( Items.m_Vector.at(*citer).at(3).c_str(), "DEFENCE" ) == 0 )
	//		SActors.at(GetActiveActor(SActors)).IMods.ImproveDefence(atoi(Items.m_Vector.at((*citer)).at(4).c_str()));
	//	tmp++;
	//}

	for( std::vector<SActor>::iterator citer = SActors.begin();
	citer != SActors.end(); ++citer)
	{
		(*citer).Update();
	}
	
	switch( game_engine.get_state() )
	{
	case EGameState::MENU:
		Menu();
		break;
	case EGameState::COMPANION_LIST:
		CompanionList();
		break;
	case EGameState::CHARACTER_LIST:
		CharacterList();
		break;
    case EGameState::HALL_OF_FAME:
		HallOfFame();
		break;
    case EGameState::SPELL_LIST:
		SpellList();
		break;
	case EGameState::MENU_OPTIONS:
		MenuOptions();
		break;
	case EGameState::CREATE_CHARACTER:
		CreateCharacter();
		break;
	case EGameState::BATTLE:
		the_battle();
		break;
	case EGameState::CITY:
		City();
		break;
	case EGameState::GUILD:
		Guild();
		break;
	case EGameState::OUTRO:
		Outro();
		break;		
	default:
		{
			if( Focused )
			Input();
			Paint();
			break;
		}
	}
	return true;
}

void CGraphics::the_bank()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc.bmp"].gs_hbmp);
	
	DynamicText.clear();

	_itoa(SActors.at(GetActiveActor(SActors)).m_Gold,buffer,10);
	DynamicText.insert(make_pair(string("DEPOSIT"),string(buffer)));
	_itoa(bank.vault.gold,buffer,10);	
	DynamicText.insert(make_pair(string("WITHDRAW"),string(buffer)));
	
	Blitter.queue_render(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BANK",DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "BANK");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "DEPOSIT" )
		{
			if( SActors.at(GetActiveActor(SActors)).m_Gold > 0 )
			{
				bank.vault.gold += SActors.at(GetActiveActor(SActors)).m_Gold;
				SActors.at(GetActiveActor(SActors)).m_Gold = 0;
			}
		}
		if( GUIObjects.at(tmp_action).action == "WITHDRAW" )
		{
			if(bank.vault.gold > 0)
			{
				SActors.at(GetActiveActor(SActors)).m_Gold += bank.vault.gold;
				bank.vault.gold = 0;
			}
		}
	}
}

void CGraphics::RenderInventory(int i_Actor)
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "inventory.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		ImageMap[BaseImagePath + "inventory.bmp"].gs_bmp.bmWidth, 
		ImageMap[BaseImagePath + "inventory.bmp"].gs_bmp.bmHeight, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "inventory.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "inventory.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Inventory.size();
	

	RECT item_up = {Screen.right-64,Screen.top,Screen.right,Screen.top+64};
	RECT item_down = {Screen.right-64,64*5,Screen.right,64*6};

	RECT LeftRing = {64,32,96,64};
	RECT Head = {128,0,192,64};
	RECT RightRing = {224,32,256,64};
	RECT MainHand = {32,96,96,192};
	RECT UpperBody = {128,96,192,192};
	RECT OffHand = {224,96,288,192};
	RECT Inventory32x32 = {0,224,32,256};

	DrawEdge(BackBuffer,&item_up,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&item_down,BDR_RAISEDINNER,BF_RECT);

	
	if(PtInRect(&LeftRing,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, LeftRing.left,LeftRing.top,LeftRing.right,LeftRing.bottom,32,32);
	}
	if(PtInRect(&Head,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, Head.left,Head.top,Head.right,Head.bottom,64,64);
		SActors.at(GetActiveActor(SActors)).HEAD = 1;
		_itoa(SActors.at(GetActiveActor(SActors)).HEAD,buffer,10);
		TextOut(BackBuffer,Head.left,Head.top,buffer,strlen(buffer));
	}
	if(PtInRect(&RightRing,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, RightRing.left,RightRing.top,RightRing.right,RightRing.bottom,32,32);
		SActors.at(GetActiveActor(SActors)).HEAD = 1;
		_itoa(SActors.at(GetActiveActor(SActors)).HEAD,buffer,10);
		TextOut(BackBuffer,Head.left,Head.top,buffer,strlen(buffer));
	}
	if(PtInRect(&MainHand,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, MainHand.left,MainHand.top,MainHand.right,MainHand.bottom,32,96);
		SActors.at(GetActiveActor(SActors)).MAINHAND = 1;
		_itoa(SActors.at(GetActiveActor(SActors)).MAINHAND,buffer,10);
		TextOut(BackBuffer,MainHand.left,MainHand.top,buffer,strlen(buffer));
	}
	if(PtInRect(&UpperBody,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, UpperBody.left,UpperBody.top,UpperBody.right,UpperBody.bottom,32,96);
		SActors.at(GetActiveActor(SActors)).UPPERBODY = 1;
		_itoa(SActors.at(GetActiveActor(SActors)).UPPERBODY,buffer,10);
		TextOut(BackBuffer,UpperBody.left,UpperBody.top,buffer,strlen(buffer));
	}
	if(PtInRect(&OffHand,Mouse))
	{
		SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
		RoundRect(BackBuffer, OffHand.left,OffHand.top,OffHand.right,OffHand.bottom,32,96);
		SActors.at(GetActiveActor(SActors)).OFFHAND = 1;
		_itoa(SActors.at(GetActiveActor(SActors)).OFFHAND,buffer,10);
		TextOut(BackBuffer,OffHand.left,OffHand.top,buffer,strlen(buffer));
	}
	for(int iy = 0; iy < 4; iy++)
	{
			Inventory32x32.left =0;			
			Inventory32x32.right =32;

		for(int ix = 0; ix < 10; ix++)
		{			
			if(PtInRect(&Inventory32x32,Mouse))
			{
				SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
				RoundRect(BackBuffer, Inventory32x32.left,Inventory32x32.top,Inventory32x32.right,Inventory32x32.bottom,32,32);
			}
			Inventory32x32.left += 32;			
			Inventory32x32.right += 32;
		}
		Inventory32x32.top += 32;
		Inventory32x32.bottom += 32;

	}


	if(PtInRect(&item_up,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		if( top_item > 0 )
		{
			top_item--;
		}
	}
	if(PtInRect(&item_down,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		if( top_item < InventorySize-1 )
		{
			top_item++;
		}
	}

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
	{
		_itoa((*citer),buffer,10);
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "equipment.bmp"].gs_hbmp);

		if(tmp >= top_item && tmp < top_item+4)
		{
			tmp2++;
			RECT item_img = {Screen.right-64,tmp2*64,Screen.right,tmp2*64 + 64};


			BitBlt(
				BackBuffer, 
				Screen.right-64, 
				tmp2*64, 
				64, 
				64, 
				Tiles_DC, 
				((*citer))*64, 
				0, 
				SRCCOPY);

			if(SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) > 0)
			{
				DrawEdge(BackBuffer,&item_img,EDGE_BUMP,BF_RECT);
			}
			if(PtInRect(&item_img,Mouse))
			{
				SelectObject(BackBuffer, GetStockObject(WHITE_BRUSH));
				RoundRect(BackBuffer, Screen.right-192, tmp2*64, 
					Screen.right - 64, tmp2*64+64, 20, 20);

				TextOut(BackBuffer,Screen.right-176,tmp2*64,Items.m_Vector.at((*citer)).at(0).c_str(),strlen(Items.m_Vector.at((*citer)).at(0).c_str()));
				TextOut(BackBuffer,Screen.right-176,tmp2*64+16,Items.m_Vector.at((*citer)).at(1).c_str(),strlen(Items.m_Vector.at((*citer)).at(1).c_str()));
				TextOut(BackBuffer,Screen.right-176,tmp2*64+32,Items.m_Vector.at((*citer)).at(2).c_str(),strlen(Items.m_Vector.at((*citer)).at(2).c_str()));
				TextOut(BackBuffer,Screen.right-176,tmp2*64+48,Items.m_Vector.at((*citer)).at(3).c_str(),strlen(Items.m_Vector.at((*citer)).at(3).c_str()));
			}
		}
		tmp++;
	}
}

void CGraphics::EquipInventory(int i_Actor)
{
	int tmp = 0;
	int tmp2 = 0;

    vector<int>::iterator iter = SActors.at(GetActiveActor(SActors)).m_Equipped.begin();
	vector<int>::iterator iter2 = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Equipped.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Equipped.end(); ++citer)
	{
		_itoa((*citer),buffer,10);
		if(tmp >= top_item && tmp < top_item+4)
		{
			tmp2++;
			RECT item_img = {Screen.right-64,tmp2*64,Screen.right,tmp2*64 + 64};
			if(PtInRect(&item_img,Mouse))
			{
				_itoa(tmp,buffer,10);
				TextOut(BackBuffer,64,tmp2*64,buffer,strlen(buffer));
				if(KEY_DOWN(VK_LBUTTON))
				{
					if( SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) == 0 )
					{
						for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
							citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
						{
							if(tmp >= top_item && tmp < top_item+4)
							{
								tmp2++;
								RECT item_img = {Screen.right-64,tmp2*64,Screen.right,tmp2*64 + 64};
								if(PtInRect(&item_img,Mouse))
								{
									TextOut(BackBuffer,Screen.left,tmp2*64,Items.m_Vector.at((*citer)).at(0).c_str(),strlen(Items.m_Vector.at((*citer)).at(0).c_str()));
								}
							}
						}

						string effect = "HP_REPLENISH";
						std::transform(effect.begin(), effect.end(), effect.begin(), (int(*)(int)) tolower);

						if ( _stricmp( Items.m_Vector.at(*iter2).at(3).c_str(), "ATTACK" ) == 0 )
						{
							MessageBox(NULL,Items.m_Vector.at((*iter2)).at(3).c_str(),"EFFECT",MB_OK);
							SActors.at(GetActiveActor(SActors)).IMods.Reset();
							SActors.at(GetActiveActor(SActors)).IMods.ImproveAttack(atoi(Items.m_Vector.at((*iter2)).at(4).c_str()));
							SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 1;
						}
						if ( _stricmp( Items.m_Vector.at(*iter2).at(3).c_str(), "DEFENCE" ) == 0 )
						{
							MessageBox(NULL,Items.m_Vector.at((*iter2)).at(3).c_str(),"EFFECT",MB_OK);
							SActors.at(GetActiveActor(SActors)).IMods.Reset();
							SActors.at(GetActiveActor(SActors)).IMods.ImproveDefence(atoi(Items.m_Vector.at((*iter2)).at(4).c_str()));
							SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 1;
						}
						if ( _stricmp( Items.m_Vector.at(*iter2).at(3).c_str(), "MP_REPLENISH" ) == 0 )
						{
							MessageBox(NULL,Items.m_Vector.at((*iter2)).at(3).c_str(),"EFFECT",MB_OK);
							SActors.at(GetActiveActor(SActors)).m_MP += atoi(Items.m_Vector.at((*iter2)).at(4).c_str());
						}
						if ( _stricmp( Items.m_Vector.at(*iter2).at(3).c_str(), "HP_REPLENISH" ) == 0 )
						{
							MessageBox(NULL,Items.m_Vector.at((*iter2)).at(3).c_str(),"EFFECT",MB_OK);
							SActors.at(GetActiveActor(SActors)).m_HP += atoi(Items.m_Vector.at((*iter2)).at(4).c_str());
						}
						if ( _stricmp(Items.m_Vector.at(*iter2).at(3).c_str(), "HP_REPLENISH" ) == 0 )
						{
							//citer = SActors.at(GetActiveActor(SActors)).m_Equipped.erase(citer);
							SActors.at(GetActiveActor(SActors)).m_Inventory.erase(iter2);
						}
					}
					else
					{
						SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 0;
					}
					return;
				}
			}

		}
		tmp++;
		iter2++;
	}
}

void CGraphics::RemoveInventory(int i_Actor)
{
	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Inventory.size();

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
	{
		_itoa((*citer),buffer,10);
		if(tmp >= top_item && tmp < top_item+5)
		{
			tmp2++;
			RECT item_img = {64,tmp2*64,128,tmp2*64 + 64};
			if(PtInRect(&item_img,Mouse))
			{
				_itoa(tmp,buffer,10);
				TextOut(BackBuffer,64,tmp2*64,buffer,strlen(buffer));
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Inventory.erase(SActors.at(GetActiveActor(SActors)).m_Inventory.begin()+tmp);
					return;
				}
			}

		}
		tmp++;
	} 
}

// what we need is items that are selectable from both
// the inventory and the shop and viewed in the same
// information square.

void CGraphics::the_general_store()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc.bmp"].gs_hbmp);

	DynamicText.clear();

	if( SelectedItemId >= 0 )
	{
		for( int Values = 0; Values < Items.m_Vector.at(SelectedItemId).size(); Values++ )
		{
		}
	}

	if( Items.m_Vector.at(store_item_page).size() > 3 )
	{
		if( Items.m_Vector.at(store_item_page).size() >= 0 )
			DynamicText.insert(make_pair(string("STATUS"),string(Items.m_Vector.at(store_item_page).at(0).c_str())));
		if( Items.m_Vector.at(store_item_page).size() >= 1 )
			DynamicText.insert(make_pair(string("COST"),string(Items.m_Vector.at(store_item_page).at(1).c_str())));
		if( Items.m_Vector.at(store_item_page).size() >= 2 )
			DynamicText.insert(make_pair(string("ONE"),string(Items.m_Vector.at(store_item_page).at(2).c_str())));
		if( Items.m_Vector.at(store_item_page).size() >= 3 )
			DynamicText.insert(make_pair(string("TWO"),string(Items.m_Vector.at(store_item_page).at(3).c_str())));
	}

	int sizer = Items.m_Vector.size();

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"STORE",DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "STORE");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "SET_ONE" )
		{
		}
		if( GUIObjects.at(tmp_action).action == "SET_TWO" )
		{
		}
		if( GUIObjects.at(tmp_action).action == "PREV" )
		{
			if( store_item_page == 0)
			{
				store_item_page = Items.m_Vector.size()-1;
			}
			else
			{
				store_item_page--;
			}
		}
		if( GUIObjects.at(tmp_action).action == "NEXT" )
		{
			store_item_page++;
			if( store_item_page > Items.m_Vector.size()-1 )
			{
				store_item_page = 0;
			}
		}
		if( GUIObjects.at(tmp_action).action == "SELL" )
		{
			if(SActors.at(GetActiveActor(SActors)).m_Inventory.size() > 0 )
			{
				vector<int>::iterator iter = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
				vector<int>::iterator iter2 = SActors.at(GetActiveActor(SActors)).m_Equipped.begin();
				while( iter != SActors.at(GetActiveActor(SActors)).m_Inventory.end() )
				{
				  if (*iter == store_item_page)
				  {
					iter = SActors.at(GetActiveActor(SActors)).m_Inventory.erase( iter );
					SActors.at(GetActiveActor(SActors)).m_Equipped.erase( iter2 );
					SActors.at(GetActiveActor(SActors)).m_Gold += atoi(Items.m_Vector.at(store_item_page).at(1).c_str());
				  }
				  else
				  {
					++iter2;
					++iter;
				  }
				}
			}
		}

		if( GUIObjects.at(tmp_action).action == "BUY" )
		{
			// Takes the item information from csvItems and pushes it into
			// the backpack of the active actor. pod_item is the item type
			/*
			if(store_item_page < 0)
			{
				store_item_page = 0;
			}

			if( SActors.at(GetActiveActor(SActors)).m_gold >= atoi(Items.m_Vector.at(store_item_page).at(1).c_str()) )
			{
				SActors.at(GetActiveActor(SActors)).m_gold -= atoi(Items.m_Vector.at(store_item_page).at(1).c_str());
				SActors.at(GetActiveActor(SActors)).m_Inventory.push_back(store_item_page);
				SActors.at(GetActiveActor(SActors)).m_Equipped.push_back(0);

				//new
				
				SActors.at(GetActiveActor(SActors)).m_Item.m_ID.push_back(store_item_page);
				SActors.at(GetActiveActor(SActors)).m_Item.m_Equipped.push_back(0);
			}
			*/
		}
	}

	if( Items.m_Vector.at(store_item_page).size() >= 0 )
	{
		int image_id = atoi(Items.m_Vector.at(store_item_page).at(1).c_str());

		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "equipment.bmp"].gs_hbmp);
		
		TransparentBlt(
		BackBuffer, 
		192, 
		96, 
		64, 
		64, 
		Tiles_DC, 
		store_item_page*64, 
		0,
		64,
		64,
		RGB(0,0,0)
		);
	}
}

// Alter this according to the documentation mentioned in the class description
// also needs to be alot more dynamic so the code doesn't have to be changed with
// every guild alteration.

void CGraphics::the_guild()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc.bmp"].gs_hbmp);

	DynamicText.clear();
	DynamicText.insert(make_pair(string("DESCRIPTION"),string(Guilds.m_Vector.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(1).c_str())));

	Blitter.queue_render(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"GUILD",DynamicText);
	

	// finds the id for the first reference of the word guild in
	// guiobjects.
	int tmp_hover = GUIHover(GUIObjects, Mouse, "GUILD");

	

	if( tmp_hover != -1)
	{
		if( GUIObjects.at(tmp_hover).action == "GUILD_ONE" )
		{
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260/2, 
				51/2, 
				Tiles_DC, 
				528, 
				(52*5)/2,
				260/2,
				52/2,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "GUILD_TWO" )
		{
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260/2, 
				51/2, 
				Tiles_DC, 
				528, 
				(52*5)/2,
				260/2,
				52/2,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "GUILD_THREE" )
		{
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260/2, 
				51/2, 
				Tiles_DC, 
				528, 
				(52*5)/2,
				260/2,
				52/2,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "GUILD_FOUR" )
		{
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260/2, 
				51/2, 
				Tiles_DC, 
				528, 
				(52*5)/2,
				260/2,
				52/2,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
	}

	int tmp_action = GUIAction(GUIObjects, Mouse, "GUILD");

	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "GUILD_ONE" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 0;
			SActors.at(GetActiveActor(SActors)).GMods.Reset();
			SActors.at(GetActiveActor(SActors)).GMods.Fighter();
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_TWO" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 1;
			SActors.at(GetActiveActor(SActors)).GMods.Reset();
			SActors.at(GetActiveActor(SActors)).GMods.Mage();
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_THREE" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 2;
			SActors.at(GetActiveActor(SActors)).GMods.Reset();
			SActors.at(GetActiveActor(SActors)).GMods.Cleric();
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_FOUR" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 3;
			SActors.at(GetActiveActor(SActors)).GMods.Reset();
			SActors.at(GetActiveActor(SActors)).GMods.Thief();		}
	}
}

class iotaGen
{
public:
    iotaGen (int iv) : current (iv)
        { }
    int operator () () {
        return current++;
    }
private:
    int current;
};

string generateLabel () {
   // generate a unique label string of the form L_ddd
   static int lastLabel = 0;
   char labelBuffer[80];
   ostrstream ost(labelBuffer, 80);
   ost << "L_" << lastLabel++ << '\0';
   return string(labelBuffer);
}

// illustrate the use of the generate and generate_n algorithms
void generate_example ()
{
   // example 1, generate a list of label values
   list<string> labelList;
   generate_n (inserter(labelList, labelList.begin()), 
         4, generateLabel);

   // example 2, generate an arithmetic progression
   vector<int> iVec(10);
   generate (iVec.begin(), iVec.end(), iotaGen(2));
   generate_n (iVec.begin(), 5, iotaGen(7));
}

void CGraphics::the_battle()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "BattleBG.bmp"].gs_hbmp);

	StretchBlt(
		BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0, 
		ImageMap[BaseImagePath + "BattleBG.bmp"].gs_bmp.bmWidth/3,
		ImageMap[BaseImagePath + "BattleBG.bmp"].gs_bmp.bmHeight/7,
		SRCCOPY);

	static bool playing = false;

	DynamicText.clear();

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BATTLE",DynamicText);

	std::list<RECT> RActor_faces, RActor_hp, RActor_mp, RActor_xp, RActor_curr_hp;
	std::list<RECT>::iterator it;

	RECT Enemy_hp_container = { Screen.right - 128,Screen.top + 240,Screen.right - 128 + 64,Screen.top + 256};
	float EnemyHPProcent = 100.0f / (float)atoi(SEnemy.EnemyMaxHP.c_str()) * (float)atoi(SEnemy.EnemyHP.c_str());
	float test = 510.0f + (64.0f / 100.0f) * EnemyHPProcent;
	RECT Enemy_hp = { Screen.right - 128,Screen.top +240,Enemy_hp.left + (64.0f / 100.0f) * EnemyHPProcent,Screen.top + 256};

	DrawEdge(BackBuffer,&Enemy_hp_container,EDGE_RAISED,BF_RECT);
	FillRect(BackBuffer,&Enemy_hp,CreateSolidBrush(RGB(255,0,0)));
	// Drawing character icons

	for(int i = 0; i < SActors.size(); i++)
	{
		int num_actors = SActors.size();
		static int countr = 0;
		countr++;

		RECT SActor_face = { i*64,Screen.bottom - 96,i*64+64,Screen.bottom - 32};
		RECT SActor_hp = { Screen.left + 128,Screen.top + 240,Screen.left +128+64,Screen.top +256};
		float ActorHPProcent = 100.0f / (float)SActors.at(i).m_MaxHP * (float)SActors.at(i).m_HP;
		RECT SActor_curr_hp  = { Screen.left + 128,Screen.top + 240,SActor_curr_hp.left  + (64.0f / 100.0f) * ActorHPProcent,Screen.top + 256};
		RECT SActor_mp = { i*64,Screen.bottom - 16,i*64+64,Screen.bottom};
		RActor_faces.push_back(SActor_face);
		RActor_hp.push_back(SActor_hp);
		RActor_curr_hp.push_back(SActor_curr_hp);
		RActor_mp.push_back(SActor_mp);
	}

	for(int opt_val = 0; opt_val < 5; ++opt_val)
	{
		// check this out
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(0,0,0));
		std::for_each( RActor_hp.begin(), RActor_hp.end(),RenderHP);
		RenderHP.set_color(RGB(255,0,0));
		std::for_each( RActor_curr_hp.begin(), RActor_curr_hp.end(),RenderHP);
	}

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "pic.bmp"].gs_hbmp);
	if(fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_Coord.y,SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] >= 0)
	{
		Blitter.tqueue(BackBuffer, 
			Screen.right - 128, 
			Screen.top + 256, 
			64, 64, Tiles_DC, 
			(int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_Coord.y,SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT]*64,
			64, 
			RGB(255,0,255),
			0);

				Blitter.tqueue(BackBuffer, 
			Screen.left + 128, 
			Screen.top + 256, 
			64, 64, Tiles_DC, 
			0,
			64, 
			RGB(255,0,255),
			0);
		if( atoi(SEnemy.EnemyHP.c_str()) <= 0)
		{
			fmap.SetFrame(SActors.at(GetActiveActor(SActors)).m_Coord.y,SActors.at(GetActiveActor(SActors)).m_Coord.x,0);

			SActors.at(GetActiveActor(SActors)).m_Inventory.push_back(rand()%(Items.m_Vector.size()-1));
			SActors.at(GetActiveActor(SActors)).m_Equipped.push_back(0);

			SActors.at(GetActiveActor(SActors)).m_XP += atoi(SEnemy.EnemyKillXP.c_str());
			SActors.at(GetActiveActor(SActors)).m_Gold += atoi(SEnemy.EnemyXP.c_str());
			SActors.at(GetActiveActor(SActors)).getCharacterLevel(SActors.at(GetActiveActor(SActors)).m_XP);

			//gs_SOUND.StopAllSongs();
			game_engine.alter_state(EGameState::RUNNING);
			menu_state.alter_state(AreaType::WORLD_MAP);
		}
		if(SActors.at(GetActiveActor(SActors)).m_HP < 1)
		{
			game_engine.alter_state(EGameState::OUTRO);
		}
	}

	for( int i = 0; i < Monsters.m_Vector.at(fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_Coord.y,SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT]).size(); i++ )
{
	std::string fin = Monsters.m_Vector.at(fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_Coord.y,SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT]).at(i).c_str();
	TextOut(BackBuffer,Screen.right-96,Screen.top+i*16,fin.c_str(),strlen(fin.c_str()));
}
	// Check to se if mouse pointer is within a certain actor picture
	// and also checking if you press to activate that specific actor
	int n = 0;
	for (it=RActor_faces.begin(); it!=RActor_faces.end(); ++it)
	{
		++n;
		RECT tmp = *it;
		if(PtInRect(&tmp,Mouse))
		{
			_itoa(n,buffer,10);
			TextOut(BackBuffer,Screen.right - 96,Screen.bottom - 96,buffer, strlen(buffer));
			DrawFocusRect(BackBuffer,&tmp);
			if(KEY_DOWN(VK_LBUTTON))
			{
				SActors.at(GetActiveActor(SActors)).m_IsActive = false;
				SActors.at(n-1).m_IsActive = true;
				SActor_active = n;
				SActor_active_face = *it;
				SelectObject(Tiles_DC,ImageMap[BaseImagePath + "FireAnimations.bmp"].gs_hbmp);
				//AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK],it->left,it->top);
			}
		}
	}

	bool EnemyAction = false;

	int tmp_action = GUIAction(GUIObjects, Mouse, "BATTLE");

	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "BATTLE_ATTACK" )
		{
			int tmpHP = atoi(SEnemy.EnemyHP.c_str());
			tmpHP -= SActors.at(GetActiveActor(SActors)).m_Attack;

			itoa(tmpHP,buffer,10);
			
			SEnemy.EnemyHP = buffer; 
			EnemyAction = true;
		}
		if( GUIObjects.at(tmp_action).action == "BATTLE_DEFEND" )
		{
			SActors.at(GetActiveActor(SActors)).m_HP += SActors.at(GetActiveActor(SActors)).m_Defence;
			EnemyAction = true;
		}
		if( GUIObjects.at(tmp_action).action == "BATTLE_FLEE" )
		{
			SActors.at(0).m_Coord.x = 0;
			SActors.at(0).m_Coord.y = 0;
			game_engine.alter_state(EGameState::RUNNING);
			menu_state.alter_state(AreaType::WORLD_MAP);
		}
	}

	if(EnemyAction)
	{
		SelectObject(Tiles_DC,ImageMap[BaseAnimationPath + "FireAnimations.bmp"].gs_hbmp);
		SActors.at(GetActiveActor(SActors)).m_HP -= rand()%atoi(Monsters.m_Vector.at(EnemyID).at(1).c_str())/10;
		EnemyAction = false;
	}
	Blitter.finish();
}
// Here the player should go to turn in quest bounties, using a random time
// an array with bounties wil be filled in and the player have the option
// of removing none interesting bounties and turning in finished bounties
// to find out if the player have what is needed there must be a function
// checking all bounty monsters and the player must have a monster kill
// counter and then there should be an item search function that checks
// all the actors inventories for the specific item and if the item is found
// it should be removed from the inventory and the actor carrying the item
// gets the reward.

void CGraphics::the_tavern()
{
	if( Quests.m_Vector.size() == city_visited.size())
	{
		game_engine.alter_state(EGameState::OUTRO);
		return;
	}
	map<int,int>::iterator city_iter;
	city_iter = city_visited.find(SActors.at(GetActiveActor(SActors)).m_Coord.y);
	if( city_iter != city_visited.end() )
	{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "chapterimages.bmp"].gs_hbmp);

	int sizen = ImageMap[BaseImagePath + "chapterimages.bmp"].gs_bmp.bmHeight/5*city_visited.size();
	int sizen2 = ImageMap[BaseImagePath + "chapterimages.bmp"].gs_bmp.bmHeight/5*(city_visited.size()-1);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		ImageMap[BaseImagePath + "chapterimages.bmp"].gs_bmp.bmHeight/5*city_visited.size(),
		ImageMap[BaseImagePath + "chapterimages.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "chapterimages.bmp"].gs_bmp.bmHeight/5,
		RGB(255,0,255));
		Blitter.finish();

		menu_state.alter_state(AreaType::WORLD_MAP);
		game_engine.alter_state(EGameState::RUNNING);

		return;
	}

	generate_example ();
	DynamicText.clear();

	DynamicText.insert(make_pair(string("STATUSBAR"),string(Quests.m_Vector.at(QuestID).at(0).c_str())));

	if( Quests.m_Vector.at(quest_obj.active).size() > 0 )
		DynamicText.insert(make_pair(string("LIST"),string(Quests.m_Vector.at(QuestID).at(0).c_str())));
	if( Quests.m_Vector.at(quest_obj.active).size() > 1  )
		DynamicText.insert(make_pair(string("DESC"),string(Quests.m_Vector.at(QuestID).at(1).c_str())));
	if( Quests.m_Vector.at(quest_obj.active).size() > 3 )
		DynamicText.insert(make_pair(string("REWARD"),string(Quests.m_Vector.at(QuestID).at(3).c_str())));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"TAVERN",DynamicText);
	
	quest_obj.printCSV(BackBuffer,Mouse);

	int tmp_action = GUIAction(GUIObjects, Mouse, "TAVERN");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "TRACK_QUEST" )
		{
			if( quest_obj.active < Quests.m_Vector.size() - 1 )
			{
				int y = SActors.at(GetActiveActor(SActors)).m_Coord.y;
				int x = SActors.at(GetActiveActor(SActors)).m_Coord.x;
				
				map<int,int>::iterator it;

				it = city_visited.find(y);
				if( it == city_visited.end() )
				{
					city_visited.insert(make_pair(y,x));
					quest_obj.questList.push_back(Quests.m_Vector.at(QuestID));
					QuestID++;
					MessageBox(NULL,"Tracking quest","Status message",MB_OK);
				}
			}
			else
			{
				game_engine.alter_state(EGameState::OUTRO);
			}
		}
		if( GUIObjects.at(tmp_action).action == "TRASH_QUEST" )
		{
			MessageBox(NULL,"Trash quest","Status message",MB_OK);		
		}
		if( GUIObjects.at(tmp_action).action == "LIST_SCROLLUP" )
		{
			//if csvQuest array id is bigger then 0 else set to 0
			quest_obj.active++;
			if( quest_obj.active > Quests.m_Vector.size() - 1 )
			{
				quest_obj.active = 0;
			}
		}
		if( GUIObjects.at(tmp_action).action == "LIST_SCROLLDOWN" )
		{
			quest_obj.active--;
			if( quest_obj.active < 0 || quest_obj.active > Quests.m_Vector.size() - 1 )
			{ 
				quest_obj.active = Quests.m_Vector.size() - 1;
			}
		}
	}
}


void CGraphics::Menu()
{
		static int ImageID = 0;

		++ImageID;
		if(ImageID > 29)
			ImageID =0;

		stringstream ss;
		ss << setw( 3 ) << setfill( '0' ) << ImageID;
		string ImagePath = BaseImagePath + "genetica/intro" + ss.str()+".bmp" ;
		SelectObject(Tiles_DC,ImageMap[ImagePath].gs_hbmp);

		TransparentBlt(BackBuffer, 
			(Screen.right - ImageMap[ImagePath].gs_bmp.bmWidth )/2, 
			0, 
			ImageMap[ImagePath].gs_bmp.bmWidth, 
			ImageMap[ImagePath].gs_bmp.bmHeight, 
			Tiles_DC, 
			0, 
			0,
			ImageMap[ImagePath].gs_bmp.bmWidth,
			ImageMap[ImagePath].gs_bmp.bmHeight,
			RGB(0,0,0));
/*
	SelectObject(Tiles_DC,ImageMap["data/gfx/images/RickardEntertainment.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		ImageMap["data/gfx/images/RickardEntertainment.bmp"].gs_bmp.bmWidth,
		ImageMap["data/gfx/images/RickardEntertainment.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));
*/
	if( game_engine.get_state() == EGameState::MENU_OPTIONS )
	{
			//SDevice GfxDevice;
			GfxDevice.GetAllDisplaySettings(main_hwnd,Mouse);
	}

	//Rendering the text on top of the buttons only
	DynamicText.clear();
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc2.bmp"].gs_hbmp);
	Blitter.queue_menu(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"MENU",DynamicText);

	int tmp_hover = GUIHover(GUIObjects, Mouse, "MENU");
	if( tmp_hover != -1 )
	{
		int ButtonWidth = 300;
		int ButtonHeight = 50;
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc2.bmp"].gs_hbmp);
		TransparentBlt(BackBuffer, 
			GUIObjects.at(tmp_hover).x - (ButtonWidth / 2), 
			GUIObjects.at(tmp_hover).y - (ButtonHeight / 2), 
			300, 
			50, 
			Tiles_DC, 
			0, 
			51*5,
			300,
			51,
			RGB(255,0,255));

			RECT TmpRect = GUIObjects.at(tmp_hover).area;
						TmpRect.left = GUIObjects.at(tmp_hover).x - (ButtonWidth / 2);
						TmpRect.top = GUIObjects.at(tmp_hover).y - (ButtonHeight / 2);
						TmpRect.right = GUIObjects.at(tmp_hover).x + (ButtonWidth / 2);
						TmpRect.bottom = GUIObjects.at(tmp_hover).y + (ButtonHeight / 2);
		DrawText(BackBuffer,
							GUIObjects.at(tmp_hover).text.c_str(),
							strlen(GUIObjects.at(tmp_hover).text.c_str()),
							&TmpRect,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	int tmp_action = GUIAction(GUIObjects, Mouse, "MENU");
	if( tmp_action != -1  && game_engine.get_state() == EGameState::MENU)
	{
		int ButtonWidth = 300;
		int ButtonHeight = 50;
		// Renders a lightning bolt over button when pressed
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc2.bmp"].gs_hbmp);
		TransparentBlt(BackBuffer, 
			GUIObjects.at(tmp_hover).x - (ButtonWidth / 2), 
			GUIObjects.at(tmp_hover).y - (ButtonHeight / 2), 
			300, 
			50, 
			Tiles_DC, 
			0, 
			51,
			300,
			51,
			RGB(255,0,255));

		if( GUIObjects.at(tmp_action).action == "RUN" )
		{
			menu_state.alter_state(AreaType::WORLD_MAP);
			game_engine.alter_state(EGameState::RUNNING);
		}
		if( GUIObjects.at(tmp_action).action == "CREATE_CHARACTER" )
		{
			game_engine.alter_state(EGameState::CREATE_CHARACTER);
		}
		if( GUIObjects.at(tmp_action).action == "OPTIONS" )
		{
			game_engine.alter_state(EGameState::MENU_OPTIONS);
		}
		if( GUIObjects.at(tmp_action).action == "COMPANION_LIST" )
		{
			game_engine.alter_state(EGameState::COMPANION_LIST);
		}
		if( GUIObjects.at(tmp_action).action == "CREATE_CHARACTER" )
		{
			game_engine.alter_state(EGameState::CREATE_CHARACTER);
		}	
		if( GUIObjects.at(tmp_action).action == "CHARACTER_LIST" )
		{
			game_engine.alter_state(EGameState::CHARACTER_LIST);
		}		
		if( GUIObjects.at(tmp_action).action == "HALL_OF_FAME" )
		{
			game_engine.alter_state(EGameState::HALL_OF_FAME);
		}
		if( GUIObjects.at(tmp_action).action == "SPELL_LIST" )
		{
			game_engine.alter_state(EGameState::SPELL_LIST);
		}
		if( GUIObjects.at(tmp_action).action == "CITY" )
		{
			game_engine.alter_state(EGameState::CITY);
		}
		if( GUIObjects.at(tmp_action).action == "GUILD" )
		{
			game_engine.alter_state(EGameState::GUILD);
		}
		if( GUIObjects.at(tmp_action).action == "EXIT" )
		{
			game_engine.alter_state(EGameState::QUIT);
			PostQuitMessage(0);
		}
	}

	TextOut(BackBuffer,Screen.right - 128,Screen.bottom - 16,"v 1.0",strlen("v 1.0"));

	Blitter.finish();
}

void CGraphics::MenuOptions()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "RickardEntertainment.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "RickardEntertainment.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "RickardEntertainment.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));
	//SDevice GfxDevice;
	GfxDevice.GetAllDisplaySettings(main_hwnd,Mouse);
	RECT ExitTrigger = {16,Screen.bottom - 100,276,Screen.bottom - 50};
	if(PtInRect(&ExitTrigger,Mouse))
	{
		DrawFocusRect(BackBuffer,&ExitTrigger);
		if(KEY_DOWN(VK_LBUTTON))
		{
			game_engine.alter_state(EGameState::MENU);
		}
	}
/*
			TransparentBlt(BackBuffer, 
				316, 
				Screen.bottom - 100, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));

			TextOut(BackBuffer,316+50,Screen.bottom - 80,"Volume+",strlen("Volume+"));
			TransparentBlt(BackBuffer, 
				316, 
				Screen.bottom - 150, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));

			TextOut(BackBuffer,316+50,Screen.bottom - 130,"Volume-",strlen("Volume-"));



	
	RECT VolumeIncTrigger = {316,Screen.bottom - 100,576,Screen.bottom - 50};
	RECT VolumeDecTrigger = {316,Screen.bottom - 150,576,Screen.bottom - 100};


	if(PtInRect(&VolumeIncTrigger,Mouse))
	{
		DrawFocusRect(BackBuffer,&VolumeIncTrigger);
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&VolumeDecTrigger,Mouse))
	{
		DrawFocusRect(BackBuffer,&VolumeDecTrigger);
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
*/
		Blitter.finish();
}

//movement
bool CGraphics::Input()
{
	CInput Input;
	if( game_engine.get_state() == EGameState::RUNNING && menu_state.get_state() == AreaType::WORLD_MAP && !SActors.at(0).m_ShowInventory)
	Input.get_input(&SActors.at(GetActiveActor(SActors)));
	if(KEY_DOWN(VK_ESCAPE))
	{
		if(game_engine.get_state() != EGameState::MENU )
		{
			game_engine.alter_state(EGameState::MENU);
		}
	}

	if(KEY_DOWN(VK_F1))
	{
		SActors.at(0).Save();
		SActors.at(0).Load();
		fmap.SaveWorld();
		fmap.CreateTiles();
		FileManager.FileLoader(Gui.m_File,Gui.m_Vector,"data/gui/gui.txt");
		//Gui.m_Vector.clear();
		//std::fstream gui;
		//Gui.m_File.open("data/gui/gui.txt", ios::in);
		//gs_csv.GS_readCSV(Gui.m_File,Gui.m_Vector);
		//Gui.m_File.close();

		GUIObjects.clear();
		for(unsigned int obj_index = 4; obj_index < Gui.m_Vector.size()-1; obj_index++ )
		{
			GUIObject tmp_obj;
			RECT tmp_rect = {
				atoi(Gui.m_Vector.at(obj_index).at(0).c_str()),
				atoi(Gui.m_Vector.at(obj_index).at(1).c_str()),
				atoi(Gui.m_Vector.at(obj_index).at(2).c_str()),
				atoi(Gui.m_Vector.at(obj_index).at(3).c_str())
			};

			tmp_obj.area = tmp_rect;
			tmp_obj.z = atoi(Gui.m_Vector.at(obj_index).at(4).c_str());
			tmp_obj.action = Gui.m_Vector.at(obj_index).at(5).c_str();
			tmp_obj.location = Gui.m_Vector.at(obj_index).at(6).c_str();
			tmp_obj.text = Gui.m_Vector.at(obj_index).at(7).c_str();

			GUIObjects.push_back(tmp_obj);
		}
	}
	return false;
}


struct optionclass {
  int current;
  optionclass() {current=0;}
	void operator() (RECT i) 
	{
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "options.bmp"].gs_hbmp);
		TransparentBlt(BackBuffer,
		i.left, //left
		i.top,   // down
		32, //size x
		32, //size y
		Tiles_DC, 
		0, //src x
		0, //Src y
		64,
		64,
		SRCCOPY);
		
		++current;
	}
} myoption;

void CGraphics::ScrollViewPort(RECT& ViewPort)
{
	if( Mouse.y <= Screen.top + 5 && fmap.get_offsetY() > 0)
	{
		cout << fmap.get_offsetY() << "Calling ScrollViewPort" << endl;
	}
	if( Mouse.y >= Screen.bottom - 5 && fmap.get_offsetY() < fmap.get_height()*64)
	{
		cout << fmap.get_offsetY()  << "Calling ScrollViewPort" << endl;
	}
	if( Mouse.x <= Screen.left + 5 && fmap.get_offsetX() > 0)
	{
		cout << fmap.get_offsetX() << "Calling ScrollViewPort" << endl;
	}
	if( Mouse.x >= Screen.right - 5 && fmap.get_offsetX() < fmap.get_width()*64)
	{
		cout << fmap.get_offsetX() << "Calling ScrollViewPort" << endl;
	}
}

// in: i_x and i_y number of tiles to render in each direction
// ViewportOffsetX,Y altered width and height into tile number

void CGraphics::CallbackFox(int Param)
{
    char Buf[50];
    sprintf(Buf, "Now I'm in Rabbit with Param %d !\n", Param);
    printf(Buf);
}

void TestTheCallback(cCallback *pCallbackFunction, int Param)
{
    pCallbackFunction->Execute(Param * Param);
}
/*
void CGraphics::CreateBoard()
        {
            //Start to create the tiles
            CENTER_OF_BOARD_X = (int)camera.OrthogonalWidth / 2;
            CENTER_OF_BOARD_Y = (int)camera.OrthogonalHeight / 2;

           
            for (int i = 0; i < BOARD_WIDTH; i++)
            {
                for (int j = 0; j < BOARD_HEIGHT; j++)
                {
                    Tile tile = new Tile(this.contentManagerName);

                    //Mathe-magical equation to center the board and get it to build in the right place.
                    tile.X = i - CENTER_OF_BOARD_X + X_FIELD_OFFSET + (i * (tile.VisibleRepresentation.ScaleX - .5f)) * 2;
                    tile.Y = j - CENTER_OF_BOARD_Y + Y_FIELD_OFFSET + (j * (tile.VisibleRepresentation.ScaleY - .5f)) * 2;
                    tile.ArrayI = i;
                    tile.ArrayJ = j;
                    _board[i, j] = tile;
                }
            }
*/
void CGraphics::RenderViewport(int i_x, int i_y, HBITMAP i_Tiles)
{
	/*
		Rendering the viewport first we count the number of tiles
		needed in x,y to fill the screen and that we ca do by
		taking the width / TileSize and Height / TileSize;
	*/

	int ScreenTilesX = 0;
	int ScreenTilesY = 0;

	ScreenTilesX = Screen.right / TileSize;
	ScreenTilesY = Screen.bottom / TileSize;

	SelectObject(Tiles_DC,i_Tiles);

	//Rendering actors on the map
	std::vector<SActor>::iterator actorit;

	for (actorit=SActors.begin(); actorit!=SActors.end(); ++actorit)
	{
		float column_start = 0;
		float column_end = Screen.bottom / TileSize +1;
		float row_start = 0;
		float row_end = Screen.right / TileSize + 1;

		for(int col = (int)column_start; col < (int)column_end; col++)
		{
			for(int row = (int)row_start; row < (int)row_end; row++)
			{
				int border_height = actorit->m_Coord.y - column_end/2 +1+ col;
				int border_width = actorit->m_Coord.x - row_end/2+1 + row;
				if(border_width <= 49 &&
					border_width >= 0 &&
					border_height <= 49 &&
					border_height >= 0)
				{
					for(int cLayer = 0; cLayer < iLayers; cLayer++)
					{/*
						if(fmap.get_tile(border_height,border_width).STrigger.mEvents & EVENT_TELEPORT )
						{
							Blitter.tqueue(BackBuffer, 
							row*64, 
							col*64, 
							64, 64, Tiles_DC, 
							(int)fmap.get_tile(border_height,border_width).Layer[cLayer]*64,
							0, RGB(255,0,255),0);
						}
						else
						{
							Blitter.tqueue(BackBuffer, 
							row*64, 
							col*64, 
							64, 64, Tiles_DC, 
							64*10,
							0, RGB(255,0,255),0);

						}*/
						if(fmap.get_tile(border_height,border_width).Layer[cLayer] != -1)
						{
							
							Blitter.tqueue(BackBuffer, 
							row*64, 
							col*64, 
							64, 64, Tiles_DC, 
							(int)fmap.get_tile(border_height,border_width).Layer[cLayer]*64,
							0, RGB(255,0,255),0);

	static CVector3 NPC(0,0,0);
	static CVector3 Velocity(0,0,0);
	static int Times = 0;
	if( Times <= 0 )
	{
		Times = rand()% 10;
		Velocity.x = rand()%3-1;
		Velocity.y = rand()%3-1;
	}

	if( (NPC.x + Velocity.x) < 0.0f)
		Times--;
	else if( (NPC.y + Velocity.y) < 0.0f)
		Times--;
	else if( (NPC.x + Velocity.x) >= fmap.get_width() )
		Times--;
	else if( (NPC.y + Velocity.y) >= fmap.get_height() )
		Times--;
	else
	{
		ClockFinal=clock()-ClockInit;
		if( (double)ClockFinal / ((double)CLOCKS_PER_SEC) > 1000 )
		{
			NPC.x += Velocity.x;
			NPC.y += Velocity.y;
			Times--;
			ClockInit = clock();
		}
	}

	if(col == NPC.y && row == NPC.x)
	{/*
			Blitter.tqueue(BackBuffer, 
				NPC.x *64, 
				NPC.y *64, 
			64, 64, Tiles_DC, 64, 64*actorit->m_Facing, RGB(255,0,255),0);*/
	}
						}
					}
				}
			}
		}

		//_itoa(actorit->coord.x,buffer,10);
		//TextOut(BackBuffer,Screen.right-90,0,buffer, strlen(buffer));
		//_itoa(actorit->coord.y,buffer,10);
		//TextOut(BackBuffer,Screen.right-90,30,buffer, strlen(buffer));
		//TextOut(BackBuffer,Screen.right-120,0,"X:", strlen("X:"));
		//TextOut(BackBuffer,Screen.right-120,30,"Y:", strlen("Y:"));
		_itoa((int)fmap.get_tile(actorit->m_Coord.y,actorit->m_Coord.x).Layer[fmap.GetLayer()],buffer,10);
		TextOut(BackBuffer,Screen.right-120,45,buffer, strlen(buffer));
		_itoa((int)fmap.GetLayer(),buffer,10);
		TextOut(BackBuffer,Screen.right-120,65,buffer, strlen(buffer));

		if(actorit->State() == ActorState::IDLE)
			TextOut(BackBuffer,0,0,"Idle..", strlen("Idle.."));
		else if( actorit->State() == ActorState::MOVING )
			TextOut(BackBuffer,0,0,"Moving..", strlen("Moving.."));
		else
			TextOut(BackBuffer,0,0,"Attacking..", strlen("Attacking.."));

		if(fmap.GetWorldEdit(0))
		{
			RECT CharacterRect = {(Screen.right / 64)/2 *64,
				(Screen.bottom / 64)/2 *64,
				(Screen.right / 64)/2 *64+ 64,
				(Screen.bottom / 64)/2 *64+64};
			DrawFocusRect(BackBuffer,&CharacterRect);
			if(PtInRect(&CharacterRect,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					fmap.SetTile(actorit->m_Coord.y,actorit->m_Coord.x,1,fmap.GetLayer());
				}
				if(KEY_DOWN(VK_RBUTTON))
				{
					fmap.SetTile(actorit->m_Coord.y,actorit->m_Coord.x,-1,fmap.GetLayer());
				}
			}
		}

		if(fmap.get_tile(actorit->m_Coord.y,actorit->m_Coord.x).Layer[MapTile::OBJECT] >= 0)
		{
			/*
			EnemyID = fmap.get_tile(actorit->coord.y,actorit->coord.x).Layer[MapTile::OBJECT];
			if( SEnemy.EnemyHP.compare("") == 0 )
			{
				//SEnemy.Init(Monsters.m_Vector);
				game_engine.alter_state(EGameState::BATTLE);
			}
			else
			{
				SEnemy.Reset();
			}*/
		}

		// Handling the animation for the player actor
		
		static int frame = 0;
		static int animation = 0;
		static int move64 = 0;

		if(frame > 7)
		{
			actorit->m_IsMoving = -1;
			frame = 0;
		}
		if(move64 > 63)
			move64 = 0;

		if(actorit->m_IsMoving > 0)
		frame++;

		if( CurrentMovementFrame > MOVEMENT_FRAMES )
		{
			CurrentMovementFrame = 0;
		}
		if( actorit->m_Facing == ActorFacing::UP || actorit->m_Facing == ActorFacing::DOWN )
		{
			if( CurrentAttackFrame > ATTACK_VERTICAL_FRAMES )
			{
				CurrentAttackFrame = 0;
			}
		}
		else if( actorit->m_Facing == ActorFacing::LEFT || actorit->m_Facing == ActorFacing::RIGHT)
		{
			if( CurrentAttackFrame > ATTACK_HORIZONTAL_FRAMES )
			{
				CurrentAttackFrame = 0;
			}
		}



		if(frame > 0 && actorit->m_Facing == ActorFacing::RIGHT)
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, frame*64, 64*actorit->m_Facing, RGB(255,0,255),0);
		}
		else if(frame > 0 && actorit->m_Facing == ActorFacing::LEFT)
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, frame*64, 64*actorit->m_Facing, RGB(255,0,255),0);
		}
		else if(frame > 0 && actorit->m_Facing == ActorFacing::UP)
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, frame*64, 64*actorit->m_Facing, RGB(255,0,255),0);
		}
		else if(frame > 0 && actorit->m_Facing == ActorFacing::DOWN)
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, frame*64, 64*actorit->m_Facing, RGB(255,0,255),0);
		}
		else
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, frame*64, 64*actorit->m_Facing, RGB(255,0,255),0);
		}

		CurrentMovementFrame++;
		CurrentAttackFrame++;

		move64++;


		static int attack_frames = 0;
		if(actorit->m_State == ActorState::ATTACKING)
		{
			if(attack_frames == 0)
				attack_frames = 7;
		}

		if(attack_frames > 0 && actorit->m_IsMoving == -1)
		{
			Blitter.tqueue(BackBuffer, 
				(Screen.right / 64)/2 *64, 
				(Screen.bottom / 64)/2 *64, 
			64, 64, Tiles_DC, attack_frames*64, 64*(actorit->m_Facing+1), RGB(255,0,255),0);
			attack_frames--;
		}
		// Character stats'
		

		static bool sue = true;

	if(sue )
	{
		actorit->Buffs.insert(std::make_pair( "Acumen 1", 31 ));
		actorit->Buffs.insert(std::make_pair( "Acumen 2", 31 ));
		actorit->Buffs.insert(std::make_pair( "Acumen 3", 31 ));
		actorit->Buffs.insert(std::make_pair( "Advanced Block 1", 31 ));
		actorit->Buffs.insert(std::make_pair( "Advanced Block 2", 31 ));
		actorit->Buffs.insert(std::make_pair( "Advanced Block 3", 31 ));
		actorit->Buffs.insert(std::make_pair( "Agility 1", 31 ));
		actorit->Buffs.insert(std::make_pair( "Agility 2", 31 ));
		actorit->Buffs.insert(std::make_pair( "Agility 3", 31 ));
		actorit->Buffs.insert(std::make_pair( "Angelic Icon 1", 31 ));
		actorit->Buffs.insert(std::make_pair( "Angelic Icon 2", 31 ));
		actorit->Buffs.insert(std::make_pair( "Angelic Icon 3", 31 ));

  sue = false;
	}

	
	int counter = 0;
	for (SActor::Typen::iterator it = actorit->Buffs.begin(); it != actorit->Buffs.end(); ++it)
    {
			_itoa((*it).second,buffer,10);
			TextOut(BackBuffer,Screen.left,Screen.top+ 140+counter*16,(*it).first, strlen((*it).first));
			TextOut(BackBuffer,Screen.left+50,Screen.top+ 140+counter*16,buffer, strlen(buffer));
			(*it).second--;
			counter++;
			if((*it).second <= 0)
			{
				STile_data TileInfo = fmap.GetTriggers(*actorit);
				if( TileInfo.STrigger.mEvents & EVENT_TELEPORT)
				{
					actorit->m_Coord.x = TileInfo.STrigger.mX;
					actorit->m_Coord.y = TileInfo.STrigger.mY;
				}
				actorit->m_HP -= TileInfo.STrigger.mValue;
				actorit->Buffs.erase(it);
				it = actorit->Buffs.begin();
			}
	}
	actorit->cur  = actorit->Buffs.find("Acumen 1");
	if ( actorit->cur == actorit->Buffs.end( ) )
	{
	}
	else
	{
		if((*actorit->cur).second <= 0)
		{
			actorit->Buffs.erase(actorit->Buffs.find("Acumen 1"));
		}
		else
		{
			_itoa((*actorit->cur).second,buffer,10);
			TextOut(BackBuffer,Screen.left,Screen.top+ 140,(*actorit->cur).first, strlen((*actorit->cur).first));
			TextOut(BackBuffer,Screen.left+50,Screen.top+ 140,buffer, strlen(buffer));
			actorit->Buffs[(*actorit->cur).first] = 100;
		}
	}

/*
  actorit->Buffs.erase(actorit->Buffs.find("june"));
 actorit->cur  = actorit->Buffs.find("december");

  actorit->prev = actorit->cur;
  actorit->next = actorit->cur;    
  ++actorit->next;
  --actorit->prev;
  cout << "Previous (in alphabetical order) is " << (*actorit->prev).first << endl;
  cout << "Next (in alphabetical order) is " << (*actorit->next).first << endl;

  _itoa((*actorit->cur).second,buffer,10);
TextOut(BackBuffer,Screen.left,Screen.top+ 140,(*actorit->prev).first, strlen((*actorit->prev).first));
TextOut(BackBuffer,Screen.left,Screen.top+ 160,(*actorit->next).first, strlen((*actorit->next).first));
TextOut(BackBuffer,Screen.left,Screen.top+ 180,buffer, strlen(buffer));
*/


		TextOut(BackBuffer,Screen.left,Screen.top+ 80,actorit->m_Name.c_str(), strlen(actorit->m_Name.c_str()));
		_itoa((int)actorit->m_HP,buffer,10);
		TextOut(BackBuffer,Screen.left,Screen.top+ 100,buffer, strlen(buffer));
				_itoa((int)actorit->m_MP,buffer,10);
		TextOut(BackBuffer,Screen.left,Screen.top+ 120,buffer, strlen(buffer));
	}

// calc the ratio between world and map sizes
// at init time -- only need to do it once unless
// the size of your world or map changes
//   float xmod = world.width / minimap.width;
//   float ymod = world.height / minimap.height;

// when drawing on the minimap you need
// to convert from world coordinates to map
// coordinates
//   map.x = world.x / xmod;
//   map.y = world.y / ymod;

	POINT MiniMapOffset = {-10,10};
	fmap.MiniMap(BackBuffer, SActors.at(GetActiveActor(SActors)).m_Coord,Screen,MiniMapOffset);
	POINT pointy = {-100,100};


	

	GameMessageWindow.Render(BackBuffer, Screen, pointy);
}

bool CGraphics::Paint()
{	
	//SelectObject(Tiles_DC,ImageMap["data/gfx/images/backpanel.bmp"].gs_hbmp);
/*
	TransparentBlt(
		BackBuffer, 
		0, 
		0, 
		ImageMap["data/gfx/images/backpanel.bmp"].gs_bmp.bmWidth, 
		ImageMap["data/gfx/images/backpanel.bmp"].gs_bmp.bmHeight, 
		Tiles_DC, 
		0, 
		0, 
		ImageMap["data/gfx/images/backpanel.bmp"].gs_bmp.bmWidth,
		ImageMap["data/gfx/images/backpanel.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));
*/
	// Interface buttons
	/*
		TransparentBlt(
		BackBuffer, 
		Screen.right-400, 
		0, 
		400, 
		300, 
		Tiles_DC, 
		0, 
		0, 
		400,
		300,
		RGB(255,0,255));
	*/
		int XOffset = Screen.right - 256;
		int YOffset = Screen.bottom - 192;
		RECT InterfaceButton[12] = {
			{XOffset,YOffset,XOffset + 64,YOffset + 64}, {XOffset + 64,YOffset,XOffset + 128,YOffset + 64}, {XOffset + 128,YOffset,XOffset + 192,YOffset + 64}, {XOffset + 192,YOffset,XOffset + 256,YOffset + 64},
			{XOffset,YOffset + 64,XOffset + 64,YOffset + 128}, {XOffset + 64,YOffset + 64,XOffset + 128,YOffset + 128}, {XOffset + 128,YOffset + 64,XOffset + 192,YOffset + 128}, {XOffset + 192,YOffset + 64,XOffset + 256,YOffset + 128},
			{XOffset,YOffset + 128,XOffset + 64,YOffset + 192}, {XOffset + 64,YOffset + 128,XOffset + 128,YOffset + 192}, {XOffset + 128,YOffset + 128,XOffset + 192,YOffset + 192}, {XOffset + 192,YOffset + 128,XOffset + 256,YOffset + 192}
		};
		
		for(int i = 0; i < 12; i++)
		if(PtInRect(&InterfaceButton[i],Mouse))
		{
			DrawFocusRect(BackBuffer,&InterfaceButton[i]);
		}

		// Drawing character icons

	std::list<RECT> RActor_faces,RActor_hp,RActor_curr_hp,RActor_mp;
	std::list<RECT>::iterator it;

	int n = 0;

	// in city menu options
	city_options.clear();
	
	
	n = 0;

	// Check to se if mouse pointer is within a certain actor picture
	// and also checking if you press to activate that specific actor
	n = 0;

	std::vector<SActor>::iterator actorit;
	STile_data TileTriggers= fmap.GetTriggers(SActors.at(GetActiveActor(SActors)));

	if(TileTriggers.STrigger.mState == TRIGGER_INITIALLY_ENABLED)
	{
		if(TileTriggers.STrigger.mConditions & CONDITION_PLAYER)
		{
			if(TileTriggers.STrigger.mEvents & EVENT_POISON)
			{	
				GameMessageWindow.Post("Poisoned!");
				SActors.at(GetActiveActor(SActors)).Buffs.insert(std::make_pair( "Poison", TileTriggers.STrigger.mTickTimer ));
			}
		}
	}
	if(TileTriggers.STrigger.mState == TRIGGER_INITIALLY_ENABLED)
	{
		if(TileTriggers.STrigger.mConditions & CONDITION_PLAYER)
		{
			if(TileTriggers.STrigger.mEvents & EVENT_DROWNING)
			{
				static int counter = 0;
				itoa(counter++,buffer,10);
				string message = "Drowning";
					message += buffer;
				GameMessageWindow.Post(message.c_str());
				SActors.at(GetActiveActor(SActors)).Buffs.insert(std::make_pair( "Drowning", TileTriggers.STrigger.mTickTimer ));
			}
		}
	}
	if(TileTriggers.STrigger.mState == TRIGGER_INITIALLY_ENABLED)
	{
		if(TileTriggers.STrigger.mConditions & CONDITION_PLAYER)
		{
			if(TileTriggers.STrigger.mEvents & EVENT_TELEPORT)
			{	
				GameMessageWindow.Post("Teleported!");
				SActors.at(GetActiveActor(SActors)).Buffs.insert(std::make_pair( "Teleporting", TileTriggers.STrigger.mTickTimer ));
			}
		}
	}
	int tempo = (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT];
	if( tempo == -1 )
	{
		
		//SActors.at(GetActiveActor(SActors)).Buffs["Poison"] = 666;
		//SActors.at(GetActiveActor(SActors)).Buffs.insert(std::make_pair( "Poison", 666 ));
		//fmap.SetFrame((int)SActors.at(GetActiveActor(SActors)).coord.y,(int)SActors.at(GetActiveActor(SActors)).coord.x,0);
		//SActors.at(GetActiveActor(SActors)).coord.y = 10;
		//SActors.at(GetActiveActor(SActors)).coord.x = 10;
	}

	if( (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] == CITY_TILE )
	for (it=city_options.begin(); it!=city_options.end(); ++it)
	{
		++n;
		RECT tmp = *it;
		if(PtInRect(&tmp,Mouse))
		{
			DrawFocusRect(BackBuffer,&tmp);                                                                     
			if(KEY_DOWN(VK_LBUTTON))
			{
				if( n == AreaType::GENERAL_STORE )
					if( (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] == CITY_TILE )
					menu_state.alter_state(AreaType::GENERAL_STORE);
				if( n == AreaType::BANK )
					if( (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] == CITY_TILE )
					menu_state.alter_state(AreaType::BANK);
				if( n == AreaType::WORLD_MAP )
					menu_state.alter_state(AreaType::WORLD_MAP);
				if( n == AreaType::TAVERN )
					if( (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] == CITY_TILE )
					menu_state.alter_state(AreaType::TAVERN);
				if( n == AreaType::GUILD )
					if( (int)fmap.get_tile((int)SActors.at(GetActiveActor(SActors)).m_Coord.y,(int)SActors.at(GetActiveActor(SActors)).m_Coord.x).Layer[MapTile::OBJECT] == CITY_TILE )
					menu_state.alter_state(AreaType::GUILD);
			}
		}
	}

	std::vector<X>      myVec;

    std::for_each(  myVec.begin(),
                    myVec.end(),
                    std::bind2nd(std::mem_fun_ref(&X::doWhat),Mouse)
                 );


    std::for_each(  myVec.begin(),
                    myVec.end(),
                    CallWhatIfOk(Mouse)
                 );

	// Map stuff
	/*
		SelectObject(Tiles_DC,ImageMap["data/gfx/images/face_sprites.bmp"].gs_hbmp);
		Blitter.tqueue(BackBuffer, Screen.left+30, Screen.bottom - 87, 64, 64, Tiles_DC, (SActors.at(GetActiveActor(SActors)).m_Sprite-1)*96, 0, RGB(255,0,255), 0);
*/

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "backpanel.bmp"].gs_hbmp);

	TransparentBlt(
		BackBuffer, 
		0, 
		0, 
		ImageMap[BaseImagePath + "backpanel.bmp"].gs_bmp.bmWidth, 
		ImageMap[BaseImagePath + "backpanel.bmp"].gs_bmp.bmHeight, 
		Tiles_DC, 
		0, 
		0, 
		ImageMap[BaseImagePath + "backpanel.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "backpanel.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));


	if( menu_state.get_state() == AreaType::WORLD_MAP )
	{
		// what we need is a viewport starting at 64px in width
		// and 128px less in height then the actual screen
		// add a function that takes a rectangle and the pointer 
		// and check if pointer moves outside edges of viewport
		ScrollViewPort(ViewPort);

		// Viewport tiles in x and y possible on screen
		int ViewportTilesX = ViewPort.right / TileSize;
		int ViewportTilesY = ViewPort.bottom / TileSize;

		// Viewport is set size and then there is a camera
		// that sets the viewport starting x,y coordinates
		// and the camera coordinates should be recalculated
		// to how many tiles that correspondes to and sent to
		// RenderViewport
		
		// Vector crash bug in RenderViewport
		RenderViewport(ViewportTilesX-1,ViewportTilesY,ImageMap[BaseImagePath + "pic.bmp"].gs_hbmp);

		// rendering character faces ontop of the map
		HBRUSH hbr,hbb,hby,oldbrBrush,oldbbBrush,oldbyBrush;
		hbr = CreateSolidBrush(RGB(255, 0, 0));
		hbb = CreateSolidBrush(RGB(0, 0, 255));
		hby = CreateSolidBrush(RGB(255, 255, 0));
		
		
		oldbrBrush  = (HBRUSH)SelectObject(BackBuffer,hbr);
		oldbbBrush  = (HBRUSH)SelectObject(BackBuffer,hbb);
		oldbyBrush  = (HBRUSH)SelectObject(BackBuffer,hby);

		// check this out
		/*
		SelectObject(Tiles_DC,ImageMap["data/gfx/images/face_sprites.bmp"].gs_hbmp);
		Blitter.tqueue(BackBuffer, Screen.left+30, Screen.bottom - 87, 64, 64, Tiles_DC, (SActors.at(GetActiveActor(SActors)).m_Sprite-1)*96, 0, RGB(255,0,255), 0);
		*/
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(0,0,0));

		std::for_each( RActor_hp.begin(), RActor_hp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(255,0,0));
		std::for_each( RActor_curr_hp.begin(), RActor_curr_hp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(0,0,255));
		std::for_each( RActor_mp.begin(), RActor_mp.end(),RenderHP);

		DeleteObject(hbr);
		DeleteObject(hbb);
		DeleteObject(hby);
	}

	ValidateArea(menu_state.get_state());
	//Page flipping

	// Render complete scene Rendering the inventory
	if(KEY_DOWN(VK_B))
	{
		SActors.at(GetActiveActor(SActors)).ShowInventory();
		/*		
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[26]);

		TransparentBlt(BackBuffer, 
			0, 
			0, 
			gs_tile.gs_bmp[26].bmWidth, 
			Screen.bottom, 
			Tiles_DC, 
			0, 
			0,
			gs_tile.gs_bmp[26].bmWidth,
			gs_tile.gs_bmp[26].bmHeight,
			RGB(255,0,255));
		
		RECT char_info = {0,0,256,256};

		SActors.at(GetActiveActor(SActors)).info(BackBuffer,(HBRUSH)RGB(0,255,0),char_info);
		*/
	}
	if(KEY_DOWN(VK_C))
	{
		SActors.at(0).CharSheet();
	}

	if(KEY_DOWN(VK_F5))
	{
		fmap.SetWorldEdit(0);
	}
	if(KEY_DOWN(VK_PRIOR))
	{
		fmap.LayerUp();
	}
	if(KEY_DOWN(VK_NEXT))
	{
		fmap.LayerDown();
	}
	

	if( SActors.at(GetActiveActor(SActors)).m_ShowCharSheet )
	{
		GameMessageWindow.Post("Charactersheet active for inspection!");
		RECT ingo = {0,0,640,479};
		SActors.at(GetActiveActor(SActors)).Info(BackBuffer,GetSysColorBrush(COLOR_WINDOW),ingo,ImageMap["data/gfx/images/face_sprites.bmp"].gs_hbmp,ImageMap["data/gfx/images/face_sprites.bmp"].gs_bmp,Tiles_DC);
	}

	if( SActors.at(GetActiveActor(SActors)).m_ShowInventory )
	{
		RenderInventory(GetActiveActor(SActors));
		EquipInventory(GetActiveActor(SActors));
/*		
		SelectObject(Tiles_DC,ImageMap["data/gfx/images/charsheet.bmp"].gs_hbmp);

		TransparentBlt(BackBuffer, 
			0, 
			0, 
			ImageMap["data/gfx/images/charsheet.bmp"].gs_bmp.bmWidth, 
			Screen.bottom, 
			Tiles_DC, 
			0, 
			0,
			ImageMap["data/gfx/images/charsheet.bmp"].gs_bmp.bmWidth,
			ImageMap["data/gfx/images/charsheet.bmp"].gs_bmp.bmHeight,
			RGB(255,0,255));

		RECT char_info = {0,0,256,256};

		SActors.at(GetActiveActor(SActors)).info(BackBuffer,(HBRUSH)RGB(0,255,0),char_info);
		*/
	}

	n = 0;

	// Here the code for showing actor information when hovering over the image o fthe character

	for (it=RActor_faces.begin(); it!=RActor_faces.end(); ++it)
	{
		RECT tmp = *it;
		++n;
		if(PtInRect(&tmp,Mouse))
		{
			// Info window for active character
			RECT char_info = {0,0,256,256};
		}
	}

	Blitter.finish();
	return true;
}

void CGraphics::ValidateArea(int i_area)
{
	for(int Area = AreaType::GENERAL_STORE; Area < AreaType::BATTLE; Area++)
	{			
		if( i_area == Area )
		{
			if( Area == AreaType::TAVERN )
				the_tavern();
			if( Area == AreaType::BANK )
				the_bank();
			if( Area == AreaType::GUILD )
				the_guild();
			if( Area == AreaType::GENERAL_STORE )
				the_general_store();
		}
	}
}

void CGraphics::CreateCharacter()
{
	// 80 height 96 width
	const int FaceSpriteRows = 25;
	const int FaceSpriteColumns = 8;
	const int FaceSpriteHeight = ImageMap[BaseImagePath + "face_sprites.bmp"].gs_bmp.bmHeight / FaceSpriteRows;
	const int FaceSpriteWidth = ImageMap[BaseImagePath + "face_sprites.bmp"].gs_bmp.bmWidth / FaceSpriteColumns;

	std::list<RECT> RActor_faces,RActor_hp,RActor_curr_hp,RActor_mp;
	std::list<RECT>::iterator it;

	static int face = 1;
	RECT SActor_face = { 400,Screen.top,464,Screen.top+64};
	RECT PreviousFace = { 400-32,Screen.top,400,Screen.top+64};
	RECT NextFace = { 496,Screen.top,496+32,Screen.top+64};

	DrawFrameControl(
  BackBuffer,
  &PreviousFace,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);

	DrawFrameControl(
  BackBuffer,
  &NextFace,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);

	if(PtInRect(&PreviousFace,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			if(SActors.at(GetActiveActor(SActors)).m_FaceSpriteID > 0)
			SActors.at(GetActiveActor(SActors)).m_FaceSpriteID--;
		}
	}
	if(PtInRect(&NextFace,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			if(SActors.at(GetActiveActor(SActors)).m_FaceSpriteID < FaceSpriteRows -1)
			SActors.at(GetActiveActor(SActors)).m_FaceSpriteID++;
		}
	}
/*
	if(PtInRect(&SActor_face,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			if(SActors.at(GetActiveActor(SActors)).m_FaceSpriteID >= 0 && SActors.at(GetActiveActor(SActors)).m_FaceSpriteID < FaceSpriteRows)
			SActors.at(GetActiveActor(SActors)).m_FaceSpriteID++;
			else
				SActors.at(GetActiveActor(SActors)).m_FaceSpriteID = 0;
			if( face < 8)
			face++;
		}

		if(KEY_DOWN(VK_RBUTTON))
		{
			if(face > 1)
			face--;
		}
	}
*/
	SActors.at(GetActiveActor(SActors)).m_Sprite = face;
	RActor_faces.clear();
	for(int i = 0; i < face; i++)
		RActor_faces.push_back(SActor_face);

	//SelectObject(Tiles_DC,ImageMap["data/gfx/images/face_sprites.bmp"].gs_hbmp);
	//std::for_each( RActor_faces.begin(), RActor_faces.end(),SDrawFaceOBj);

	
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "face_sprites.bmp"].gs_hbmp);
				TransparentBlt(BackBuffer, 
				400, 
				0, 
				FaceSpriteWidth, 
				FaceSpriteHeight, 
				Tiles_DC, 
				0, 
				FaceSpriteHeight*SActors.at(GetActiveActor(SActors)).m_FaceSpriteID,
				FaceSpriteWidth,
				FaceSpriteHeight,
				RGB(255,0,255));

	static int count = 0;
	SActors.at(GetActiveActor(SActors)).m_Name = this->actor_name;
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "buttonsmisc.bmp"].gs_hbmp);

		RECT EditNameBox={0,0,300,50};
	DrawFrameControl(
  BackBuffer,
  &EditNameBox,
  DFC_BUTTON,
 DFCS_BUTTONPUSH
);

			if(PtInRect(&EditNameBox,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					EditName = true;
				}
			}
			if(!PtInRect(&EditNameBox,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					EditName = false;
				}
			}
		DrawText(BackBuffer,
							SActors.at(GetActiveActor(SActors)).m_Name.c_str(),
							strlen(SActors.at(GetActiveActor(SActors)).m_Name.c_str()),
							&EditNameBox,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);						
			//TextOut(BackBuffer,25,25,SActors.at(GetActiveActor(SActors)).m_name.c_str(), strlen(SActors.at(GetActiveActor(SActors)).m_name.c_str()));
/*
			TransparentBlt(BackBuffer, 
				356, 
				400, 
				300, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
*/
			RECT CANCEL={356,400,356+260,450};

	DrawFrameControl(
  BackBuffer,
  &CANCEL,
  DFC_BUTTON,
 DFCS_BUTTONPUSH
);

			if(PtInRect(&CANCEL,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(game_engine.get_state() != EGameState::MENU )
					{
						game_engine.alter_state(EGameState::MENU);
						menu_state.alter_state(AreaType::MENU);
					}
				}
			}

		DrawText(BackBuffer,
							"CANCEL",
							strlen("CANCEL"),
							&CANCEL,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);		
			//TextOut(BackBuffer,381,425,"Cancel", strlen("Cancel"));
/*
			TransparentBlt(BackBuffer, 
				0, 
				104, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));*/
			RECT GENDER = {0,104,260,156};
	DrawFrameControl(
  BackBuffer,
  &GENDER,
  DFC_BUTTON,
 DFCS_BUTTONPUSH
);
			RECT CLASS = {0,156,260,208};
	DrawFrameControl(
  BackBuffer,
  &CLASS,
  DFC_BUTTON,
 DFCS_BUTTONPUSH
);
					if( SActors.at(GetActiveActor(SActors)).m_Gender == 0 )
					{
		DrawText(BackBuffer,
							"Male",
							strlen("Male"),
							&GENDER,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						//TextOut(BackBuffer,25,104,"Male", strlen("Male"));
					}
					else
					{
								DrawText(BackBuffer,
							"Female",
							strlen("Female"),
							&GENDER,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						//TextOut(BackBuffer,25,104,"Female", strlen("Female"));
					}

			if(PtInRect(&GENDER,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Gender = 0;
				}

				if(KEY_DOWN(VK_RBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Gender = 1;
				}
			}
			if(PtInRect(&CLASS,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_ClassID = 0;
				}

				if(KEY_DOWN(VK_RBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_ClassID = 1;
				}
			}
/*
			TransparentBlt(BackBuffer, 
				0, 
				156, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));*/
								DrawText(BackBuffer,
							SActors.at(GetActiveActor(SActors)).ClassName(SActors.at(GetActiveActor(SActors)).m_ClassID).c_str(),
							strlen(SActors.at(GetActiveActor(SActors)).ClassName(SActors.at(GetActiveActor(SActors)).m_ClassID).c_str()),
							&CLASS,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			//TextOut(BackBuffer,25,181,SActors.at(GetActiveActor(SActors)).ClassName(SActors.at(GetActiveActor(SActors)).m_ClassID).c_str(),strlen(SActors.at(GetActiveActor(SActors)).ClassName(SActors.at(GetActiveActor(SActors)).m_ClassID).c_str()));
/*
			TransparentBlt(BackBuffer, 
				0, 
				400, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));*/

			RECT SAVE={0,400,260,450};
	DrawFrameControl(
  BackBuffer,
  &SAVE,
  DFC_BUTTON,
 DFCS_BUTTONPUSH
);
			if(PtInRect(&SAVE,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).Save();
					//SActors.at(GetActiveActor(SActors)).Load();

					if(game_engine.get_state() != EGameState::MENU )
					{
						game_engine.alter_state(EGameState::MENU);
						menu_state.alter_state(AreaType::MENU);
					}
				}
			}
								DrawText(BackBuffer,
							"Save character",
							strlen("Save character"),
							&SAVE,
							DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			//TextOut(BackBuffer,25,425,"Save character", strlen("Save character"));

		for(int i = 0; i < 6; i++)
		{
			RECT trigger1 = {381,200,381+32,232};
			RECT trigger2 = {381,233,381+32,265};
			RECT trigger3 = {381,266,381+32,298};
			RECT trigger4 = {381,299,381+32,331};
			RECT trigger5 = {381,332,381+32,364};
			RECT trigger6 = {381,365,381+32,397};

			RECT trigger7 = {381+64,200,381+96,232};
			RECT trigger8 = {381+64,233,381+96,265};
			RECT trigger9 = {381+64,266,381+96,298};
			RECT trigger10 = {381+64,299,381+96,331};
			RECT trigger11 = {381+64,332,381+96,364};
			RECT trigger12 = {381+64,365,381+96,397};


			TextOut(BackBuffer,381,150,"Stat Points Left", strlen("Stat Points Left"));
			TextOut(BackBuffer,381+128,150,to_string(SActors.at(GetActiveActor(SActors)).m_StatPoints,std::dec).c_str(), strlen(to_string(SActors.at(GetActiveActor(SActors)).m_StatPoints,std::dec).c_str()));

			TextOut(BackBuffer,381+128,200,"Strength", strlen("Strength"));
			TextOut(BackBuffer,381+128,233,"Charisma", strlen("Charisma"));
			TextOut(BackBuffer,381+128,266,"Wisdom", strlen("Wisdom"));
			TextOut(BackBuffer,381+128,299,"Dexterity", strlen("Dexterity"));
			TextOut(BackBuffer,381+128,332,"Stamina", strlen("Stamina"));
			TextOut(BackBuffer,381+128,365,"Intelligence", strlen("Intelligence"));

			if(PtInRect(&trigger1,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Charisma > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Charisma--;
					}
				}
			}
			if(PtInRect(&trigger2,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Constitution > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Constitution--;
					}
				}
			}
			if(PtInRect(&trigger3,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Dexterity > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Dexterity--;
					}
				}
			}
			if(PtInRect(&trigger4,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Intelligence > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Intelligence--;
					}
				}
			}
			if(PtInRect(&trigger5,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Strength > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Strength--;
					}
				}
			}
			if(PtInRect(&trigger6,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_Wisdom > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints++;
						SActors.at(GetActiveActor(SActors)).m_Wisdom--;
					}
				}
			}
			if(PtInRect(&trigger7,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Charisma++;
					}
				}
			}
			if(PtInRect(&trigger8,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Constitution++;
					}		
				}
			}
			if(PtInRect(&trigger9,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Dexterity++;
					}	
				}
			}
			if(PtInRect(&trigger10,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Intelligence++;
					}
				}
			}
			if(PtInRect(&trigger11,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Strength++;
					}
				}
			}
			if(PtInRect(&trigger12,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints > 0)
					{
						SActors.at(GetActiveActor(SActors)).m_StatPoints--;
						SActors.at(GetActiveActor(SActors)).m_Wisdom++;
					}
				}
			}

	DrawFrameControl(
  BackBuffer,
  &trigger1,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger2,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger3,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger4,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger5,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger6,
  DFC_SCROLL,
 DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &trigger7,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger8,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger9,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger10,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger11,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger12,
  DFC_SCROLL,
 DFCS_SCROLLRIGHT
);
	/*
		TransparentBlt(BackBuffer, 
				381, 
				200+(32*i), 
				32, 
				32, 
				Tiles_DC, 
				0, 
				424,
				32,
				32,
				RGB(255,0,255));

			TransparentBlt(BackBuffer, 
				381+64, 
				200+(32*i), 
				32, 
				32, 
				Tiles_DC, 
				0, 
				457,
				32,
				32,
				RGB(255,0,255));*/
		}

		
		for(int i = 0; i < 4; i++)
		{
			RECT button = {0,0,260,51};
			RECT smallbutton = {260,0,290,30};
			RECT smallbutton1 = {290,0,320,30};
			RECT smallbutton2 = {320,0,350,30};
			RECT smallbutton3 = {350,0,380,30};

			if(PtInRect(&button,Mouse))
			{
				/*
				TransparentBlt(BackBuffer, 
					0, 
					0, 
					260, 
					50, 
					Tiles_DC, 
					0, 
					51*5,
					260,
					51,
					RGB(255,0,255));*/
			}
			else
			{
			}

			_itoa(SActors.at(GetActiveActor(SActors)).m_Charisma,buffer,10);
			TextOut(BackBuffer,391+32,205,buffer, strlen(buffer));
			_itoa(SActors.at(GetActiveActor(SActors)).m_Constitution,buffer,10);
			TextOut(BackBuffer,391+32,205+(32),buffer, strlen(buffer));
			_itoa(SActors.at(GetActiveActor(SActors)).m_Dexterity,buffer,10);
			TextOut(BackBuffer,391+32,205+(64),buffer, strlen(buffer));
			_itoa(SActors.at(GetActiveActor(SActors)).m_Intelligence,buffer,10);
			TextOut(BackBuffer,391+32,205+(96),buffer, strlen(buffer));
			_itoa(SActors.at(GetActiveActor(SActors)).m_Strength,buffer,10);
			TextOut(BackBuffer,391+32,205+(128),buffer, strlen(buffer));
			_itoa(SActors.at(GetActiveActor(SActors)).m_Wisdom,buffer,10);
			TextOut(BackBuffer,391+32,205+(160),buffer, strlen(buffer));


			Blitter.finish();
		}

		if(KEY_DOWN(VK_LBUTTON))
		{
			count++;
		}

		if(KEY_DOWN(VK_RBUTTON))
		{
			count--;
		}
}

void CGraphics::Outro()
{
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "gameover.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "gameover.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "gameover.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	for (int i = 19; i >= 5; i--)
	{
		EraseScreen(); 
		TextOut(BackBuffer,50,i*16,"Programming", strlen("Programming"));
		TextOut(BackBuffer,200,i*16,"Rickard Skeppström", strlen("Rickard Skeppström"));
		TextOut(BackBuffer,200,(i+1)*16,"Andreas Ohlsson", strlen("Andreas Ohlsson"));
		TextOut(BackBuffer,50,(i+2)*16,"Graphics", strlen("Graphics"));
		TextOut(BackBuffer,200,(i+2)*16,"Rickard Skeppström", strlen("Rickard Skeppström"));
		TextOut(BackBuffer,50,(i+3)*16,"Sound & Music", strlen("Sound & Music"));
		TextOut(BackBuffer,200,(i+3)*16,"Rpg maker", strlen("Rpg maker"));
		TextOut(BackBuffer,50,(i+4)*16,"Others", strlen("Others"));
		TextOut(BackBuffer,200,(i+4)*16,"Marcel Risberg", strlen("Marcel Risberg"));
		TextOut(BackBuffer,200,(i+5)*16,"Daniel Risberg", strlen("Daniel Risberg"));
		Text.Initialize();
		Blitter.finish();
		if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
		{
				game_engine.alter_state(EGameState::QUIT);
				PostQuitMessage(0);
		}
	}

}

void CGraphics::HallOfFame()
{
	EraseScreen();
	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "qataria_elfwood.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "qataria_elfwood.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "qataria_elfwood.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));
	
		SelectObject(Tiles_DC,ImageMap[BaseImagePath + "pqpc_dialoguemain.bmp"].gs_hbmp);
		TransparentBlt(BackBuffer, 
			0, 
			0, 
			150, 
			64, 
			Tiles_DC, 
			0, 
			261,
			150,
			64,
			RGB(255,0,255));
		TransparentBlt(BackBuffer, 
			150, 
			0, 
			150, 
			64, 
			Tiles_DC, 
			150, 
			261,
			150,
			64,
			RGB(255,0,255));
		TransparentBlt(BackBuffer, 
			0, 
			128, 
			150, 
			64, 
			Tiles_DC, 
			0, 
			320,
			150,
			64,
			RGB(255,0,255));
		TransparentBlt(BackBuffer, 
			150, 
			128, 
			150, 
			64, 
			Tiles_DC, 
			150, 
			320,
			150,
			64,
			RGB(255,0,255));
		TransparentBlt(BackBuffer, 
			0, 
			64, 
			150, 
			64, 
			Tiles_DC, 
			0, 
			270,
			150,
			64,
			RGB(255,0,255));
		TransparentBlt(BackBuffer, 
			150, 
			64, 
			150, 
			64, 
			Tiles_DC, 
			150, 
			270,
			150,
			64,
			RGB(255,0,255));


		RECT Location = {0,0,300,16};
	cHallOfFame.Refresh(SActors.at(GetActiveActor(SActors)));
	cHallOfFame.Render(&BackBuffer,&Location);

	Blitter.finish();
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}

void CGraphics::CharacterList()
{


	//CDemoThread dmt;


 //SleepEx(1 * 1000, FALSE);
	CheckSaveGames("data/actors/*.sav","data/logs/SaveGameLog.txt");
	ListCharacters("data/actors/*.sav","data/logs/actorlist.txt");
	EraseScreen();
 //silly( (void*)&test );
  //dmt.Start(NULL);

  // ... do something useful
	//SleepEx(1 * 1000, FALSE);

  //t1.Start();

  //  ... Do something useful 

  //t1.Stop();


  //t1.Stop(true); // force kill if running.
	RECT SaveButton = {192,64,256,128};
	RECT LoadButton = {192,128,256,192};

	DrawFrameControl(
	  BackBuffer,
	  &SaveButton,
	  DFC_BUTTON,
	  DFCS_BUTTONPUSH
	);
	DrawFrameControl(
	  BackBuffer,
	  &LoadButton,
	  DFC_BUTTON,
	  DFCS_BUTTONPUSH
	);
	DrawText(
	  BackBuffer,
	  "Load",
	  strlen("Load"),
	  &SaveButton,
	  DT_SINGLELINE | DT_CENTER | DT_VCENTER
	);
	DrawText(
	  BackBuffer,
	  "Save",
	  strlen("Save"),
	  &LoadButton,
	  DT_SINGLELINE | DT_CENTER | DT_VCENTER
	);

	RECT SaveSlot[9] = {{64,64,192,96},{64,96,192,128},{64,128,192,160},
						{64,160,192,192},{64,192,192,224},{64,224,192,256},
						{64,256,192,288},{64,288,192,320},{64,320,192,352}};

	for(int i = 0; i < (sizeof(SaveSlot)) / (sizeof(RECT)); i++)
	{
		char buffer[100];
		string SaveSlotName;

		SaveSlotName += "SaveSlot";
		SaveSlotName += itoa(i,buffer,10);
		DrawFrameControl(
		  BackBuffer,
		  &SaveSlot[i],
		  DFC_BUTTON,
		  DFCS_BUTTONPUSH
		);
		DrawText(
		  BackBuffer,
		  SaveSlotName.c_str(),
		  strlen(SaveSlotName.c_str()),
		  &SaveSlot[i],
		  DT_SINGLELINE | DT_CENTER | DT_VCENTER
		);
		if(PtInRect(&SaveSlot[i],Mouse))
		{
			DrawFocusRect(BackBuffer,&SaveSlot[i]);
			if(KEY_DOWN(VK_LBUTTON))
			{

			}
		}
	}


	int Index = 0;
	for (vector<string>::iterator it = MenuLoadActor.begin(); it!=MenuLoadActor.end(); ++it) {
		/*
			SelectObject(Tiles_DC,ImageMap["data/gfx/images/buttonsmisc2.bmp"].gs_hbmp);
			TransparentBlt(BackBuffer, 
				16, 
				(Index*50), 
				300, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				300,
				51,
				RGB(255,0,255));
		TextOut(BackBuffer,25,(Index*50)+9,it->c_str(), strlen(it->c_str()));
		Index++;
		*/
	}

	Blitter.finish();
  //dmt.Stop(true);
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}

/*

SPawn types
Animal
Cleanup
Demon
DEvil
Dragon
ELemental
Giant
Humanoid
Indigini
Insect
Lycanthrope
Mage
Mythical
Reptile
Thief
Undead
Warrior
Water
*/
void CGraphics::CompanionList()
{
	EraseScreen();

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "dungeon.bmp"].gs_hbmp);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "dungeon.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "dungeon.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	int Index = 0;
	for (vector<Companion*>::iterator it = ActorCompanions.begin(); it!=ActorCompanions.end(); ++it) 
	{
		int XOffset = Screen.right / 4 + Index*128;
		int YOffset = Screen.bottom / 2;
		SelectObject(Tiles_DC,(*it)->Image.gs_hbmp);

		static int AnimationFrame = 0;
		static int AnimationID = 0;
		int FrameDelay = 500;
		
		static clock_t init, final;
		if( init == 0 )
		{
			init=clock();
		}

		RECT AnimationHolder = {XOffset, 
			YOffset, 
			XOffset + 32, 
			YOffset + 32};

		DrawFrameControl( BackBuffer, &AnimationHolder, DFC_BUTTON, DFCS_BUTTONPUSH );
		TransparentBlt(BackBuffer, 
			XOffset, 
			YOffset, 
			22, 
			30, 
			Tiles_DC, 
			62 + (AnimationFrame*22), 
			88,
			22,
			30,
			RGB(255,0,255));

		
		final=clock()-init; 
		if( final >= FrameDelay )
		{
			init = 0;
			++AnimationFrame;
		}

		if(AnimationFrame == 5)
		{
			AnimationFrame = 0;
			AnimationID = rand()%20+1;
		}
		TextOut(BackBuffer,XOffset,YOffset- 25,(*it)->myType().c_str(), strlen((*it)->myType().c_str()));
		Index++;
	}

	Blitter.finish();
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}

void CGraphics::SpellList()
{
	RECT Spell1 = {Screen.left + 32,Screen.top + 0,Screen.left + 96,Screen.top + 64};
	RECT Spell2 = {Screen.left + 128,Screen.top + 0,Screen.left + 192,Screen.top + 64};
	RECT Spell3 = {Screen.left + 32,Screen.top + 64,Screen.left + 96,Screen.top + 128};
	RECT Spell4 = {Screen.left + 128,Screen.top + 64,Screen.left + 192,Screen.top + 128};
	RECT Spell5 = {Screen.left + 32,Screen.top + 128,Screen.left + 96,Screen.top + 192};
	RECT Spell6 = {Screen.left + 128,Screen.top + 128,Screen.left + 192,Screen.top + 192};

	ListCharacters("data/actors/*.sav","data/logs/actorlist.txt");
	EraseScreen();

	// Dialogue box for the information of the spell


	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/arcane_circle.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell1.left, 
		Spell1.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/arcane_circle.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/arcane_circle.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/blinding_snow.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell2.left, 
		Spell2.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/blinding_snow.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/blinding_snow.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/cosmic_energy.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell3.left, 
		Spell3.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/cosmic_energy.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/cosmic_energy.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/evil.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell4.left, 
		Spell4.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/evil.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/evil.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/fireball.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell5.left, 
		Spell5.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/fireball.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/fireball.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,ImageMap[BaseImagePath + "spellicons/frost.bmp"].gs_hbmp);
	TransparentBlt(BackBuffer, 
		Spell6.left, 
		Spell6.top, 
		64, 
		64, 
		Tiles_DC, 
		0, 
		0,
		ImageMap[BaseImagePath + "spellicons/frost.bmp"].gs_bmp.bmWidth,
		ImageMap[BaseImagePath + "spellicons/frost.bmp"].gs_bmp.bmHeight,
		RGB(255,0,255));

	RECT LevelTitle = {200,0,300,16};
	RECT SkillTitle = {200,16,300,32};
	RECT ManaCostTitle = {200,32,300,48};
	RECT CastingTimeTitle = {200,48,300,64};
	RECT SpellEffectTitle = {200,64,300,76};

	RECT LevelValue = {300,0,400,16};
	RECT SkillValue = {300,16,400,32};
	RECT ManaCostValue = {300,32,400,48};
	RECT CastingTimeValue = {300,48,400,64};
	RECT SpellEffectValue = {300,64,400,76};


	DrawText(BackBuffer,"LEVEL",strlen("LEVEL"), &LevelTitle, DT_CENTER);
	DrawText(BackBuffer,"SKILL",strlen("SKILL"), &SkillTitle, DT_CENTER);
	DrawText(BackBuffer,"MANA COST",strlen("MANA COST"), &ManaCostTitle, DT_CENTER);
	DrawText(BackBuffer,"CASTING TIME",strlen("CASTING TIME"), &CastingTimeTitle, DT_CENTER);
	DrawText(BackBuffer,"SPELL EFFECT",strlen("SPELL EFFECT"), &SpellEffectTitle, DT_CENTER);


	DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(1).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(1).c_str()), &LevelValue, DT_CENTER);
	DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(3).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(3).c_str()), &SkillValue, DT_CENTER);
	DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(4).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(4).c_str()), &ManaCostValue, DT_CENTER);
	DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(5).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(5).c_str()), &CastingTimeValue, DT_CENTER);
	int TextHeight = DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(2).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(2).c_str()), &SpellEffectValue, DT_LEFT | DT_WORDBREAK);
	SpellEffectValue.bottom  = SpellEffectValue.top + TextHeight;
	DrawText(BackBuffer,Spells.m_Vector.at(SelectedSpellId).at(2).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(2).c_str()), &SpellEffectValue, DT_LEFT | DT_WORDBREAK);


 	RECT PreviousPage = {Screen.left + 32,Screen.bottom - 64,Screen.left + 64,Screen.bottom - 32};
	RECT NextPage = {Screen.right -  64,Screen.bottom - 64 ,Screen.right - 32,Screen.bottom - 32};

	DrawFrameControl(
	  BackBuffer,
	  &PreviousPage,
	  DFC_SCROLL,
	  DFCS_SCROLLLEFT
	);

	DrawFrameControl(
	  BackBuffer,
	  &NextPage,
	  DFC_SCROLL,
	  DFCS_SCROLLRIGHT
	);

	int Index = 0;

	if(PtInRect(&Spell1,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 0;
		}
	}

	if(PtInRect(&Spell2,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 1;
		}
	}

	if(PtInRect(&Spell3,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 2;
		}
	}

	if(PtInRect(&Spell4,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 3;
		}
	}

	if(PtInRect(&Spell5,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 4;
		}
	}

	if(PtInRect(&Spell6,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			SelectedSpellId = 5;
		}
	}

	//TextOut(BackBuffer,0,0,Spells.m_Vector.at(SelectedSpellId).at(0).c_str(),strlen(Spells.m_Vector.at(SelectedSpellId).at(0).c_str()));
	/*
	for(unsigned int i = 0; i < Spells.m_Vector.size()-1; i++)
	{
		int ScreenItems = SCREEN_HEIGHT / 16;
		int Current = i / ScreenItems;
		if(Current == 0)
		TextOut(BackBuffer,Current*200,i*16-(SCREEN_HEIGHT*Current),Spells.m_Vector.at(i).at(0).c_str(),strlen(Spells.m_Vector.at(i).at(0).c_str()));
		else if(Current == 1)
			TextOut(BackBuffer,Current*200,i*16-SCREEN_HEIGHT,Spells.m_Vector.at(i).at(0).c_str(),strlen(Spells.m_Vector.at(i).at(0).c_str()));
		else if (Current == 2)
			TextOut(BackBuffer,Current*200,i*16-(SCREEN_HEIGHT*2),Spells.m_Vector.at(i).at(0).c_str(),strlen(Spells.m_Vector.at(i).at(0).c_str()));
	}*/

	Blitter.finish();
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}
void CGraphics::City()
{
	RECT GeneralStore = { Screen.left, Screen.top, Screen.left + 150, Screen.top + 50};
	RECT Morgue =		{ Screen.left + 150, Screen.top, Screen.left + 300, Screen.top + 50};
	RECT Guilds =		{ Screen.left + 300, Screen.top, Screen.left + 450, Screen.top + 50};
	RECT Dungeon =		{ Screen.left + 450, Screen.top, Screen.left + 600, Screen.top + 50};

	RECT Confinement =  { Screen.left, Screen.top + 50, Screen.left + 150, Screen.top + 100};
	RECT Seer =			{ Screen.left + 150, Screen.top + 50, Screen.left + 300, Screen.top + 100};
	RECT Bank =			{ Screen.left + 300, Screen.top + 50, Screen.left + 450, Screen.top + 100};
	RECT Exit =			{ Screen.left + 450, Screen.top + 50, Screen.left + 600, Screen.top + 100};

	MapDialogRect(main_hwnd,&GeneralStore);
	ListCharacters("data/actors/*.sav","data/logs/actorlist.txt");
	EraseScreen();

	DrawFrameControl( BackBuffer, &GeneralStore, DFC_BUTTON,DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &Morgue, DFC_BUTTON, DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &Guilds, DFC_BUTTON, DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &Dungeon, DFC_BUTTON, DFCS_BUTTONPUSH	);
	DrawFrameControl( BackBuffer, &Confinement, DFC_BUTTON, DFCS_BUTTONPUSH	);
	DrawFrameControl( BackBuffer, &Seer, DFC_BUTTON, DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &Bank, DFC_BUTTON, DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &Exit, DFC_BUTTON, DFCS_BUTTONPUSH );

	DrawText(BackBuffer,"G&eneral Store",strlen("G&eneral Store"), &GeneralStore, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Morgue",strlen("Morgue"), &Morgue, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Guilds",strlen("Guilds"), &Guilds, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Dungeon",strlen("Dungeon"), &Dungeon, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Confinement",strlen("Confinement"), &Confinement, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Seer",strlen("Seer"), &Seer, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Bank",strlen("Bank"), &Bank, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Exit",strlen("Exit"), &Exit, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	if(PtInRect(&GeneralStore,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Morgue,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Guilds,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Dungeon,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			game_engine.alter_state(EGameState::RUNNING);
			menu_state.alter_state(AreaType::WORLD_MAP);
		}
	}
	if(PtInRect(&Confinement,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Seer,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Bank,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&Exit,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			game_engine.alter_state(EGameState::MENU);
		}
	}

	Blitter.finish();
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}

void CGraphics::Guild()
{
	EraseScreen();

	const int GuildTitle = 0;
	const int GuildDesc = 1;
	const int GuildStrengthReq = 2;
	const int GuildIntelligenceReq = 3;
	const int GuildWisdomReq = 4;
	const int GuildConstitutionReq = 5;
	const int GuildCharismaReq = 6;
	const int GuildDexterityReq = 7;

	const int FighterGuildID = 0;
	const int MageGuildID = 1;
	const int ClericGuildID = 2;
	const int ThiefGuildID = 3;

	RECT FighterGuildButton = { 480, 160, 640, 192};
	RECT MageGuildButton = { 480, 192, 640, 224};
	RECT ClericGuildButton = { 480, 224, 640, 256};
	RECT ThiefGuildButton = { 480, 256, 640, 288};

	DrawFrameControl( BackBuffer, &FighterGuildButton, DFC_BUTTON,DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &MageGuildButton, DFC_BUTTON,DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &ClericGuildButton, DFC_BUTTON,DFCS_BUTTONPUSH );
	DrawFrameControl( BackBuffer, &ThiefGuildButton, DFC_BUTTON,DFCS_BUTTONPUSH );

	DrawText(BackBuffer,"Fighter",strlen("Fighter"), &FighterGuildButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Mage",strlen("Mage"), &MageGuildButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Cleric",strlen("Cleric"), &ClericGuildButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Thief",strlen("Thief"), &ThiefGuildButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	RECT GuildStrengthReqTitle			= { 0, 160, 48, 192};
	RECT GuildIntelligenceReqTitle		= { 48, 160, 96, 192};
	RECT GuildWisdomReqTitle			= { 96, 160, 144, 192};
	RECT GuildConstitutionReqTitle		= { 144, 160, 192, 192};
	RECT GuildCharismaReqTitle			= { 192, 160, 240, 192};
	RECT GuildDexterityReqTitle			= { 240,160, 288, 192};

	DrawText(BackBuffer,"Str",strlen("Str"), &GuildStrengthReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Int",strlen("Int"), &GuildIntelligenceReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Wis",strlen("Wis"), &GuildWisdomReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Con",strlen("Con"), &GuildConstitutionReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Cha",strlen("Cha"), &GuildCharismaReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(BackBuffer,"Dex",strlen("Dex"), &GuildDexterityReqTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	RECT GuildMasterTitleText		= { 32, 32, 480, 100};
	RECT GuildStatsReqTitleText		= { 32, 128, 480, 160};
	RECT GuildsTitleText			= { 480, 100, 640, 128};

	RECT Messages = {0,0,0,0};
	RECT GuildMasterTitleButton				= { Screen.left, Screen.top, Screen.left + 150, Screen.top + 50};
	RECT GuildDescButton				= { Screen.left + 150, Screen.top, Screen.left + 300, Screen.top + 50};

	RECT GuildStrengthReqButton			= { 0, 192, 48, 224};
	RECT GuildIntelligenceReqButton		= { 48, 192, 96, 224};
	RECT GuildWisdomReqButton			= { 96, 192, 144, 224};
	RECT GuildConstitutionReqButton		= { 144, 192, 192, 224};
	RECT GuildCharismaReqButton			= { 192, 192, 240, 224};
	RECT GuildDexterityReqButton		= { 240,192, 288, 224};

	DrawText(BackBuffer,"Guildmaster",strlen("Guildmaster"), &GuildMasterTitleText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	DrawFrameControl( BackBuffer, &GuildMasterTitleButton, DFC_BUTTON,DFCS_BUTTONPUSH );

	DrawText(BackBuffer,"GuildStatsReqTitleText",strlen("GuildStatsReqTitleText"), &GuildStatsReqTitleText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	DrawText(BackBuffer,"Guilds",strlen("Guilds"), &GuildsTitleText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	for(unsigned int ObjIndex = 0; ObjIndex < Guilds.m_Vector.size()-1; ObjIndex++ )
	{
		DrawFrameControl( BackBuffer, &GuildMasterTitleButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildDescButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildStrengthReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildIntelligenceReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildWisdomReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildConstitutionReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildCharismaReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );
		DrawFrameControl( BackBuffer, &GuildDexterityReqButton, DFC_BUTTON,DFCS_BUTTONPUSH );

		Guilds.m_Vector.at(ObjIndex).at(GuildTitle).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildDesc).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildStrengthReq).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildIntelligenceReq).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildWisdomReq).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildConstitutionReq).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildCharismaReq).c_str();
		Guilds.m_Vector.at(ObjIndex).at(GuildDexterityReq).c_str();

		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildTitle).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildTitle).c_str()), &GuildMasterTitleButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildDesc).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildDesc).c_str()), &GuildDescButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildStrengthReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildStrengthReq).c_str()), &GuildStrengthReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildIntelligenceReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildIntelligenceReq).c_str()), &GuildIntelligenceReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildWisdomReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildWisdomReq).c_str()), &GuildWisdomReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildConstitutionReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildConstitutionReq).c_str()), &GuildConstitutionReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildCharismaReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildCharismaReq).c_str()), &GuildCharismaReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DrawText(BackBuffer,Guilds.m_Vector.at(ObjIndex).at(GuildDexterityReq).c_str(),strlen(Guilds.m_Vector.at(ObjIndex).at(GuildDexterityReq).c_str()), &GuildDexterityReqButton, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	if(PtInRect(&GuildMasterTitleButton,Mouse))
	{
		DrawText(BackBuffer,Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str(),strlen(Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str()), &Messages, DT_CALCRECT);
		DrawText(BackBuffer,Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str(),strlen(Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str()), &Messages, DT_WORDBREAK | DT_LEFT);
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	if(PtInRect(&GuildDescButton,Mouse))
	{
		DrawText(BackBuffer,Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str(),strlen(Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str()), &Messages, DT_CALCRECT);
		DrawText(BackBuffer,Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str(),strlen(Guilds.m_Vector.at(FighterGuildID).at(GuildDesc).c_str()), &Messages, DT_WORDBREAK | DT_LEFT);
		if(KEY_DOWN(VK_LBUTTON))
		{
		}
	}
	Blitter.finish();
	if(KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
	{
			game_engine.alter_state(EGameState::MENU);
	}
}

void CGraphics::EraseScreen()
{
	GetClientRect(main_hwnd,&Screen);
	ValidateRect(main_hwnd,NULL);
	FillRect(BackBuffer, &Screen, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

CGraphics::CGraphics()
{
}