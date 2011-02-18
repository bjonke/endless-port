#include <cmath>
#include <map>
#include <strstream>
#include <crtdbg.h>

#include "misc/Library.h"
#include "graphics/CVector.h"
#include "graphics/graphics.h"
#include "map/cmapstuff.h"
#include "misc/globals.h"
#include "font/gs_FONT.h"
#include "map/gs_map.h"
#include "gs_BLITTER.h"
#include "misc/CInput.h"
#include "graphics/SDevice.h"
#include "sound/gs_FMOD.h"

#pragma comment(lib, "winmm.lib")

gs_FMOD gs_SOUND;
CInventory Actors_inventory;
csvVector csvSpells;
csvVector csvItems;
csvVector csvQuests;
csvVector csvGuilds;
csvVector csvActors;
csvVector csvImages;
csvVector csvGUI;
csvVector csvTemp;
GS_CSV gs_csv;

void FileLoader(std::fstream& i_stream,csvVector& i_csvVector,std::string i_filepath)
{
	i_stream.open(i_filepath.c_str(), ios::in);
	gs_csv.GS_readCSV(i_stream,i_csvVector);
	i_stream.close();
}

bool CGraphics::Init(HWND hwnd)
{
	CInventory* pack = NULL;
	pack = new CInventory;
	InventoryTable packTable;
	delete pack;

	pod_player* mongo = NULL;
	mongo = new pod_player;
	PlayerTable PT;
	PlayerTable::hasher hfunc = PT.hash_function();
	PlayerTable::key_equal eqfunc = PT.key_eq();
	if(mongo)
	{
		mongo->m_IsNPC = false;
		PT["dde"].m_name = "dekdedede";
	}
	mongo = new pod_player;
	if(mongo)
	{
		mongo->m_IsNPC = false;
		PT["jkalle"].m_name = "kalle";
	}

	int test = eqfunc("dde", "dde");
	if(test == 1)
		mongo = &PT["dde"];
	ofstream systemconf;
	systemconf.open("conf/system.txt");

	gs_SOUND.play_sound("CITY.mid");
	gs_SOUND.load_sounds();

	#ifdef _DEBUG

	RedirectIOToConsole();

	#endif
	menu_state.m_state = AreaType::STREET;
	systemconf << "menu_state:" << menu_state.m_state << endl;
	glob_anim_frame_x = glob_anim_frame_y = 0;
	systemconf << "glob_anim_frame_x:" << glob_anim_frame_x << endl;
	main_window_handle = hwnd;
	main_hwnd = hwnd;
	GetClientRect(main_window_handle,&rect);
	
	// Related to Paint function for rendering the map
	GetClientRect(main_window_handle,&Screen);
	SetRect(&ViewPort,0,0,Screen.right-64,Screen.bottom-128);

	// Writes all the bmp filenames into a file
	systemconf << "filedata" << FileData("data/*.bmp","data/pics.txt" );

	// Loades all the data form the file level1.txt into gs_tiles
	fmap.create_tiles("levels/level1.txt");

	// Loading most data from file into memory
	std::fstream spells;
	std::fstream items;
	std::fstream quests;
	std::fstream npc;
	std::fstream guilds;
	std::fstream images;
	std::fstream actors;
	std::fstream gui;
	FileLoader(spells,csvSpells,"data/spells/spells.txt");
	FileLoader(items,csvItems,"data/items/items.txt");
	FileLoader(quests,csvQuests,"data/quests/quests.txt");
	FileLoader(guilds,csvGuilds,"data/guilds/guilds.txt");
	FileLoader(images,csvImages,"data/images/images.txt");
	FileLoader(actors,csvActors,"data/actors/actors.txt");
	FileLoader(gui,csvGUI,"data/gui/gui.txt");

	for(unsigned int obj_index = 4; obj_index < csvGUI.size()-1; obj_index++ )
	{
		GUIObject tmp_obj;
		RECT tmp_rect = {
			atoi(csvGUI.at(obj_index).at(0).c_str()),
			atoi(csvGUI.at(obj_index).at(1).c_str()),
			atoi(csvGUI.at(obj_index).at(2).c_str()),
			atoi(csvGUI.at(obj_index).at(3).c_str())
		};

		tmp_obj.area = tmp_rect;
		tmp_obj.z = atoi(csvGUI.at(obj_index).at(4).c_str());
		tmp_obj.action = csvGUI.at(obj_index).at(5).c_str();
		tmp_obj.location = csvGUI.at(obj_index).at(6).c_str();
		tmp_obj.text = csvGUI.at(obj_index).at(7).c_str();

		GUIObjects.push_back(tmp_obj);
	}

	std::vector<GUIObject>::iterator it;
	std::list<GUIObject> l(GUIObjects.begin(),GUIObjects.end());
	sort(GUIObjects.begin(),GUIObjects.end(),zDataSortPredicate);

	//SActor.m_hp = 10;
	for(int Actor_id = 0; Actor_id < 7; Actor_id++)
	{
		systemconf << "Initiating actor " << Actor_id << endl;
		/*SActor.m_gold = atoi(csvGUI.at(Actor_id).at(1).c_str());
		
		SActor.m_mp = atoi(csvGUI.at(Actor_id).at(2).c_str());
		SActor.m_hp = atoi(csvGUI.at(Actor_id).at(3).c_str());
		
		SActor.m_max_mp = atoi(csvGUI.at(Actor_id).at(4).c_str());
		SActor.m_max_hp = atoi(csvGUI.at(Actor_id).at(5).c_str());

		SActor.m_ix = atoi(csvGUI.at(Actor_id).at(6).c_str());
		SActor.m_iy = atoi(csvGUI.at(Actor_id).at(7).c_str());
		SActor.m_iz = atoi(csvGUI.at(Actor_id).at(8).c_str());
		
		SActor.m_isactive = atoi(csvGUI.at(Actor_id).at(9).c_str());
		*/
		pod_player SActor;
		SActors.push_back(SActor);
		// Something strange is happening here the first loop the id and all is getting set correct
		// though the SActor object gets destroyed after the first loop and are thereafter set to 0
		// so possibly you will have to somehow make the SActor object in here and then copy it to
		// the vector of objects to get it right.
		// Seems that SActors vector somewhere gets destroy and thereby creating a negative id
	}

	gDC = GetDC(hwnd);

	// För BackBuffer (Dubbel) buffring
	BackBuffer = CreateCompatibleDC(gDC);
	BackBuffer_bmp = CreateCompatibleBitmap(gDC, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	Old_bmp = (HBITMAP)SelectObject(BackBuffer, BackBuffer_bmp);

	for(unsigned int i = 0; i < csvImages.size()-5; i++)
	{
		gs_tile.gs_hbmp[i] = reinterpret_cast<HBITMAP>(LoadImage(0,csvImages.at(i+4).at(0).c_str(),
			IMAGE_BITMAP,0,0,
            LR_LOADFROMFILE));
		GetObject(gs_tile.gs_hbmp[i],sizeof(BITMAP), &gs_tile.gs_bmp[i]);
		systemconf << "Initiating image " << i << endl;
	}
	
	Tiles_DC = CreateCompatibleDC(NULL);

	systemconf << "game engine state:" << game_engine.alter_state(EGameState::SPLASH);
	if( systemconf.is_open())
		systemconf.close();
	return true;
}

// Part for the intro of the game, should be loaded first and then
// after you should be entering the menu state.
const int TileSize = 64;

// Get the id fr the active actor
int GetActiveActor(std::vector<pod_player>& i_SActors)
{
	for(int i = 0; i < i_SActors.size(); ++i)
	{
		if( i_SActors.at(i).m_isactive )
		{
			return i;
		}
	}
	return 0;
}
// By reference HDC& DrawSurface
int AnimateExplosion(HDC DrawSurface, HDC TileSurface, BITMAP& bmp, int i_x = 0, int i_y = 0)
{
	glob_anim_frame_x++;
	glob_anim_frame_y++;

	if( glob_anim_frame_x*TileSize > bmp.bmWidth )
		glob_anim_frame_x = 0;
	if( glob_anim_frame_y*TileSize > bmp.bmHeight )
		glob_anim_frame_y = 0;

	// i_rop is the color value that will be transparent
	Blitter.tqueue(DrawSurface,
		i_x,
		i_y,
		TileSize,
		TileSize,
		TileSurface,
		TileSize*glob_anim_frame_x,
		TileSize*glob_anim_frame_y,
		RGB(0,0,0),
		0);
	return glob_anim_frame_x;
}

void CGraphics::SplashScreen()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[23]);

	StretchBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[23].bmWidth,
		gs_tile.gs_bmp[23].bmHeight,
		SRCCOPY);

	SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_HAWK]);

	AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK]);
	AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK],rect.right - TileSize);

	Blitter.finish();

	if(KEY_DOWN(VK_SPACE))
	{
		game_engine.alter_state(EGameState::RUNNING);
		menu_state.alter_state(AreaType::STREET);
		//active_area = AreaType::STREET;
	}
}

void CGraphics::the_bank()
{
	_itoa(SActors.at(GetActiveActor(SActors)).m_gold,buffer,10);

	std::map<string,string> DynamicText;

	DynamicText.insert(make_pair(string("STATUSBAR"),string(buffer)));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BANK", DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "BANK");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
			//active_area = AreaType::STREET;
		}
		if( GUIObjects.at(tmp_action).action == "DEPOSIT" )
		{
			if( SActors.at(GetActiveActor(SActors)).m_gold > 0 )
			{
				bank.vault.gold += SActors.at(GetActiveActor(SActors)).m_gold;
				SActors.at(GetActiveActor(SActors)).m_gold = 0;
			}
		}
		if( GUIObjects.at(tmp_action).action == "WITHDRAW" )
		{
			if(bank.vault.gold > 0)
			{
				SActors.at(GetActiveActor(SActors)).m_gold += bank.vault.gold;
				bank.vault.gold = 0;
			}
		}
	}
}

pod_item Get_pod_item(std::vector<std::string>& i_vstring )
{
	pod_item tmppod;
	tmppod.m_name = i_vstring.at(0).c_str();
	tmppod.equipped = false;
	return tmppod;
}

void RenderInventory(int i_Actor, POINT i_pt)
{
	GetClientRect(main_hwnd,&rect);
	int tmp = 0;
	int tmp2 = 0;

	//int backpack_items = SActors.at(GetActiveActor(SActors)).backpack.item.size();
	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Inventory.size();
	

	RECT item_up = {rect.right-32,rect.bottom-(16*5),rect.right,rect.bottom-(16*4)};
	RECT item_down = {rect.right-32,rect.bottom-16,rect.right,rect.bottom};

	DrawEdge(BackBuffer,&item_up,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&item_down,BDR_RAISEDINNER,BF_RECT);

	if(PtInRect(&item_up,i_pt))
	{
		if(KEY_DOWN(VK_LBUTTON))
		if( top_item > 0 )
		{
			top_item--;
		}
	}
	if(PtInRect(&item_down,i_pt))
	{
		if(KEY_DOWN(VK_LBUTTON))
		if( top_item < InventorySize-4 )
		{
			top_item++;
		}
	}

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
	{
		char buffer[100];
		_itoa((*citer),buffer,10);
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_EQ]);

		if(tmp >= top_item && tmp < top_item+4)
		{
			tmp2++;
			RECT item_img = {0,tmp2*64,64,tmp2*64 + 64};

			BitBlt(
				BackBuffer, 
				0, 
				tmp2*64, 
				64, 
				64, 
				Tiles_DC, 
				((*citer)-5)*64, 
				0, 
				SRCCOPY);

			if(SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) > 0)
			{
				DrawEdge(BackBuffer,&item_img,EDGE_BUMP,BF_RECT);
			}
			if(PtInRect(&item_img,i_pt))
			{
				TextOut(BackBuffer,0,tmp2*64,buffer,strlen(buffer));
				TextOut(BackBuffer,0,tmp2*64,csvItems.at((*citer)).at(2).c_str(),strlen(csvItems.at((*citer)).at(2).c_str()));
				TextOut(BackBuffer,0,tmp2*64+16,csvItems.at((*citer)).at(1).c_str(),strlen(csvItems.at((*citer)).at(1).c_str()));
			}
		}
		tmp++;
	}
/*
	for (list<pod_item>::const_iterator citer = SActors.at(GetActiveActor(SActors)).backpack.item.begin();
     citer != SActors.at(GetActiveActor(SActors)).backpack.item.end(); ++citer)
	{
	tmp++;
	if(tmp >= top_item && tmp < top_item+5)
	{
		tmp2++;
		int image_id = atoi(csvItems.at(store_item_page).at(1).c_str());

		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_EQ]);

		RECT item_img = {
			rect.right-(gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12+96),
			rect.bottom-(tmp2*16),
			rect.right - gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12,
			rect.bottom-(tmp2*16)+16
		};

		if(PtInRect(&item_img,i_pt))
		{
			TextOut(BackBuffer,rect.right - 96,rect.top+(tmp2*16),(*citer).m_name.c_str(),strlen((*citer).m_name.c_str()));
		}
		BitBlt(
			BackBuffer, 
			rect.right-(gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12+96), 
			rect.bottom-(tmp2*16), 
			gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12, 
			16, 
			Tiles_DC, 
			5+(gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12*image_id)+gs_tile.gs_bmp[MapTileType::STD_EQ].bmWidth/12, 
			0, 
			SRCCOPY);

		TextOut(BackBuffer,rect.right - 96,rect.bottom-(tmp2*16),(*citer).m_name.c_str(),strlen((*citer).m_name.c_str()));
	}
  }
*/
}
void EquipInventory(int i_Actor, POINT i_pt)
{
	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Equipped.size();

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Equipped.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Equipped.end(); ++citer)
	{
		char buffer[100];
		_itoa((*citer),buffer,10);
		if(tmp >= top_item && tmp < top_item+4)
		{
			tmp2++;
			RECT item_img = {64,tmp2*64,128,tmp2*64 + 64};
			if(PtInRect(&item_img,i_pt))
			{
				_itoa(tmp,buffer,10);
				TextOut(BackBuffer,64,tmp2*64,buffer,strlen(buffer));
				if(KEY_DOWN(VK_LBUTTON))
				{
					if( SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) == 0 )
					{
						SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 1;
					}
					else
					{
						SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 0;
					}

					//SActors.at(GetActiveActor(SActors)).m_Equipped.erase(SActors.at(GetActiveActor(SActors)).m_Equipped.begin()+tmp);
					Sleep(1000);
					return;
				}
			}

		}
		tmp++;
	}
}

void RemoveInventory(int i_Actor, POINT i_pt)
{
	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Inventory.size();

	for( std::vector<int>::const_iterator citer = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
		citer != SActors.at(GetActiveActor(SActors)).m_Inventory.end(); ++citer)
	{
		char buffer[100];
		_itoa((*citer),buffer,10);
		if(tmp >= top_item && tmp < top_item+5)
		{
			tmp2++;
			RECT item_img = {64,tmp2*64,128,tmp2*64 + 64};
			if(PtInRect(&item_img,i_pt))
			{
				_itoa(tmp,buffer,10);
				TextOut(BackBuffer,64,tmp2*64,buffer,strlen(buffer));
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Inventory.erase(SActors.at(GetActiveActor(SActors)).m_Inventory.begin()+tmp);
					Sleep(1000);
					return;
				}
			}

		}
		tmp++;
	}
}

void CGraphics::the_general_store()
{
	std::map<string,string> DynamicText;

	if( csvItems.at(store_item_page).size() > 3 )
	{
		if( csvItems.at(store_item_page).size() >= 0 )
			DynamicText.insert(make_pair(string("STATUSBAR"),string(csvItems.at(store_item_page).at(0).c_str())));
		if( csvItems.at(store_item_page).size() >= 1 )
			DynamicText.insert(make_pair(string("COST"),string(csvItems.at(store_item_page).at(1).c_str())));
		if( csvItems.at(store_item_page).size() >= 2 )
			DynamicText.insert(make_pair(string("ONE"),string(csvItems.at(store_item_page).at(2).c_str())));
		if( csvItems.at(store_item_page).size() >= 3 )
			DynamicText.insert(make_pair(string("TWO"),string(csvItems.at(store_item_page).at(3).c_str())));
	}

	int sizer = csvItems.size();

	_itoa(SActors.at(GetActiveActor(SActors)).getGold(),buffer,10);
	DynamicText.insert(make_pair(string("GOLD"),string(buffer)));

	Actors_inventory.list(BackBuffer,500,100);

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"STORE",DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "STORE");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
			//active_area = AreaType::STREET;
		}
		if( GUIObjects.at(tmp_action).action == "SET_ONE" )
		{
		}
		if( GUIObjects.at(tmp_action).action == "SET_TWO" )
		{
		}
		if( GUIObjects.at(tmp_action).action == "PREV" )
		{
			// Entries before row four in the data file is not used
			store_item_page--;
			if( store_item_page < 5 )
			{
				store_item_page = csvItems.size()-1;
			}
		}
		if( GUIObjects.at(tmp_action).action == "NEXT" )
		{
			store_item_page++;
			if( store_item_page > csvItems.size()-1 )
			{
				store_item_page = 5;
			}
		}
		if( GUIObjects.at(tmp_action).action == "SELL" )
		{
			if(SActors.at(GetActiveActor(SActors)).m_Inventory.size() > 0 )
			{
				vector<int>::iterator iter = SActors.at(GetActiveActor(SActors)).m_Inventory.begin();
				while( iter != SActors.at(GetActiveActor(SActors)).m_Inventory.end() )
				{
				  if (*iter == store_item_page)
					iter = SActors.at(GetActiveActor(SActors)).m_Inventory.erase( iter );
				  else
					++iter;
				}
			}
			//RemoveInventory(GetActiveActor(SActors), pt);
		}

		if( GUIObjects.at(tmp_action).action == "BUY" )
		{
			// Takes the item information from csvItems and pushes it into
			// the backpack of the active actor. pod_item is the item type
			
			if(store_item_page < 5)
			{
				store_item_page = 5;
			}

			SActors.at(GetActiveActor(SActors)).m_Inventory.push_back(store_item_page);
			SActors.at(GetActiveActor(SActors)).m_Equipped.push_back(0);

			if( SActors.at(GetActiveActor(SActors)).getGold() >= atoi(csvItems.at(store_item_page).at(1).c_str()))
			{
				SActors.at(GetActiveActor(SActors)).m_Inventory.push_back(store_item_page);
				SActors.at(GetActiveActor(SActors)).m_Equipped.push_back(0);
			}
		}
	}

	if( csvItems.at(store_item_page).size() > 3 )
	{
		int image_id = atoi(csvItems.at(store_item_page).at(1).c_str());

		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_EQ]);
		
		RECT item_img = {320,96,512,256};

		TransparentBlt(
		BackBuffer, 
		0, 
		0, 
		64, 
		64, 
		Tiles_DC, 
		(store_item_page-5)*64, 
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
	std::map<string,string> DynamicText;
	cout << "Id counting down: " << SActors.at(GetActiveActor(SActors)).GetID() << endl;
	//DynamicText.insert(make_pair(string("STATUSBAR"),string(csvGuilds.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(0).c_str())));
	DynamicText.insert(make_pair(string("STATUSBAR"),string(csvGuilds.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(0).c_str())));
	DynamicText.insert(make_pair(string("ONE"),string(csvGuilds.at(4).at(0).c_str())));
	DynamicText.insert(make_pair(string("TWO"),string(csvGuilds.at(5).at(0).c_str())));
	DynamicText.insert(make_pair(string("THREE"),string(csvGuilds.at(6).at(0).c_str())));
	DynamicText.insert(make_pair(string("FOUR"),string(csvGuilds.at(7).at(0).c_str())));
	DynamicText.insert(make_pair(string("DESCRIPTION"),string(csvGuilds.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(1).c_str())));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"GUILD",DynamicText);

	// finds the id for the first reference of the word guild in
	// guiobjects.

	int tmp_action = GUIAction(GUIObjects, Mouse, "GUILD");

	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
			//active_area = AreaType::STREET;
		}

		if( GUIObjects.at(tmp_action).action == "GUILD_ONE" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 4;
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_TWO" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 5;
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_THREE" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 6;
		}
		if( GUIObjects.at(tmp_action).action == "GUILD_FOUR" )
		{
			SActors.at(GetActiveActor(SActors)).m_ClassID = 7;
		}
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
	std::map<string,string> DynamicText;
	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BATTLE",DynamicText);
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
	generate_example ();
	std::map<string,string> DynamicText;

	DynamicText.insert(make_pair(string("STATUSBAR"),string(csvQuests.at(quest_obj.active).at(0).c_str())));

	if( csvQuests.at(quest_obj.active).size() > 0 )
		DynamicText.insert(make_pair(string("LIST"),string(csvQuests.at(quest_obj.active).at(0).c_str())));
	if( csvQuests.at(quest_obj.active).size() > 1  )
		DynamicText.insert(make_pair(string("DESC"),string(csvQuests.at(quest_obj.active).at(1).c_str())));
	if( csvQuests.at(quest_obj.active).size() > 2 )
		DynamicText.insert(make_pair(string("REWARD"),string(csvQuests.at(quest_obj.active).at(2).c_str())));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"TAVERN",DynamicText);
	
	quest_obj.printCSV(BackBuffer,Mouse);

	int tmp_action = GUIAction(GUIObjects, Mouse, "TAVERN");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
			//active_area = AreaType::STREET;
		}
		if( GUIObjects.at(tmp_action).action == "TRACK_QUEST" )
		{
			quest_obj.questList.push_back(csvQuests.at(quest_obj.active));
			MessageBox(NULL,"Tracking quest","Status message",MB_OK);		
		}
		if( GUIObjects.at(tmp_action).action == "TRASH_QUEST" )
		{
			MessageBox(NULL,"Trash quest","Status message",MB_OK);		
		}
		if( GUIObjects.at(tmp_action).action == "LIST_SCROLLUP" )
		{
			//if csvQuest array id is bigger then 0 else set to 0
			quest_obj.active++;
			if( quest_obj.active > csvQuests.size() - 2 )
			{
				quest_obj.active = 4;
			}
		}
		if( GUIObjects.at(tmp_action).action == "LIST_SCROLLDOWN" )
		{
			quest_obj.active--;
			if( quest_obj.active < 4 || quest_obj.active > csvQuests.size() - 2 )
			{ 
				quest_obj.active = csvQuests.size() - 2;
			}
		}
	}
}

void CGraphics::Create()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_640x480]);

	Blitter.queue(BackBuffer,0,0,
		gs_tile.gs_bmp[MapTileType::STD_640x480].bmWidth,
		gs_tile.gs_bmp[MapTileType::STD_640x480].bmHeight,
		Tiles_DC,
		0,0,
		SRCCOPY,0);

	ifstream menu_file ( "menu.txt", ios_base::in);
	ofstream logfile("logs/menulogger.txt", ios_base::out);

	if ( !menu_file.is_open() ) 
	{
		return;
	}

	while( !menu_file.eof() )
	{
		char temp_obj[100];
		menu_file >> temp_obj;
		logfile << temp_obj;
	}

	menu_file.close();
	logfile.close();

	if( active_area == AreaType::MENU_CREATE )
	{
		RECT rect_run = {63,63,192,95};
		RECT rect_create = {63,96,192,127};
		RECT rect_options = {63,128,192,159};
		RECT rect_exit = {63,160,192,191};

		DrawEdge(BackBuffer,&rect_run,BDR_RAISEDINNER,BF_RECT);
		DrawEdge(BackBuffer,&rect_create,BDR_RAISEDINNER,BF_RECT);
		DrawEdge(BackBuffer,&rect_options,BDR_RAISEDINNER,BF_RECT);
		DrawEdge(BackBuffer,&rect_exit,BDR_RAISEDINNER,BF_RECT);
		// Run game
		if(clip_region(BackBuffer,rect_run,Mouse,"ggggRun"))
		{
			DrawFocusRect(BackBuffer,&rect_run);
			if(KEY_DOWN(VK_LBUTTON))
			{
				game_engine.m_state = AreaType::STREET;
				active_area = AreaType::STREET;
			}
		}

		// Run create character
		if(clip_region(BackBuffer,rect_create,Mouse,"Create"))
		{
			DrawFocusRect(BackBuffer,&rect_create);
			if(KEY_DOWN(VK_LBUTTON))
			{
				game_engine.m_state = AreaType::MENU_CREATE;
				active_area = AreaType::MENU_CREATE;
			}
		}

		// Run options
		if(clip_region(BackBuffer,rect_options,Mouse,"Options"))
		{
			DrawFocusRect(BackBuffer,&rect_options);
			if(KEY_DOWN(VK_LBUTTON))
			{
				if(game_engine.m_state == AreaType::MENU)
					game_engine.m_state = AreaType::MENU_OPTIONS;
				else
					game_engine.m_state = AreaType::MENU;
			}
		}

		// Run exit
		if(clip_region(BackBuffer,rect_exit,Mouse,"Exit"))
		{
			DrawFocusRect(BackBuffer,&rect_exit);
			if(KEY_DOWN(VK_LBUTTON))
			{
				PostQuitMessage(0);
			}
		}

		Blitter.finish();
	}
}

void CGraphics::Menu()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[23]);

	StretchBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[23].bmWidth,
		gs_tile.gs_bmp[23].bmHeight,
		SRCCOPY);

	ofstream sndconf;
	if(KEY_DOWN(VK_F2))
	{
		if(gs_SOUND.master_vol < 1.0f)
		{
			gs_SOUND.master_vol+= 0.1f;
			gs_SOUND.effects_vol+= 0.1f;
			gs_SOUND.music_vol+= 0.1f;
		}
		
		sndconf.open("conf/snd.txt", ios_base::out);
		sndconf << "Master volume:" << gs_SOUND.master_vol << endl;
		sndconf << "Effects volume:" << gs_SOUND.effects_vol << endl;
		sndconf << "Music volume:" << gs_SOUND.music_vol;		Sleep(100);
	}
	if(KEY_DOWN(VK_F3))
	{
		if(gs_SOUND.master_vol > 0.0f)
		{
			gs_SOUND.master_vol-= 0.1f;
			gs_SOUND.effects_vol-= 0.1f;
			gs_SOUND.music_vol-= 0.1f;
		}
		if(gs_SOUND.master_vol < 0.1f)
		{
			gs_SOUND.master_vol = 0.0f;
			gs_SOUND.effects_vol = 0.0f;
			gs_SOUND.music_vol = 0.0f;
		}
		sndconf.open("conf/snd.txt", ios_base::out);
		sndconf << "Master volume:" << gs_SOUND.master_vol << endl;
		sndconf << "Effects volume:" << gs_SOUND.effects_vol << endl;
		sndconf << "Music volume:" << gs_SOUND.music_vol;

		Sleep(100);
	}
	if(sndconf.is_open())
	sndconf.close();
	gs_SOUND.channel->setVolume(gs_SOUND.master_vol);

	SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_HAWK]);

	ofstream animateconf;
	animateconf.open("conf/animate.txt");
	animateconf << "Explosion frame x:" << AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK]);
	animateconf << "Explosion frame x:" << AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK],rect.right - TileSize);
	if(animateconf.is_open())
		animateconf.close();
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_MENU]);
	ofstream blitterconf;
	blitterconf.open("conf/blitter.txt");
	blitterconf << "queue blitt result:" << Blitter.queue(BackBuffer,64,64,
		gs_tile.gs_bmp[MapTileType::STD_MENU].bmWidth,
		gs_tile.gs_bmp[MapTileType::STD_MENU].bmHeight,
		Tiles_DC,
		0,0,SRCCOPY,0);
	if(blitterconf.is_open())
		blitterconf.close();
	if( game_engine.get_state() == EGameState::MENU_OPTIONS )
	{
			SDevice GfxDevice;
			GfxDevice.GetAllDisplaySettings(main_window_handle,Mouse);
	}

	std::map<string,string> DynamicText;

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"MENU",DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "MENU");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "RUN" )
		{
			game_engine.alter_state(EGameState::RUNNING);
			// Here we need a old active area trigger like in the game_engine
			menu_state.alter_state(menu_state.m_old_state);
			active_area = AreaType::STREET;
		}
		if( GUIObjects.at(tmp_action).action == "OPTIONS" )
		{
			if(game_engine.get_state() == EGameState::MENU)
			{
				game_engine.alter_state(EGameState::MENU_OPTIONS);
			}
			else
			{
				game_engine.alter_state(EGameState::MENU);
			}
		}
		if( GUIObjects.at(tmp_action).action == "EXIT" )
		{
			game_engine.alter_state(EGameState::QUIT);
			PostQuitMessage(0);
		}
	}

	// volume rendering functionality for options menu
	//master vol
	int decimal,sign;
	char *buffy;
	buffy = _fcvt(gs_SOUND.master_vol,1,&decimal,&sign);
	TextOut(BackBuffer,190,200,buffy, strlen(buffy));
	TextOut(BackBuffer,90,200,"MASTER", strlen("MASTER"));
	RECT tmp_rect = {
		190,
		200,
		190+200 * gs_SOUND.master_vol,
		232
	};
	RECT master_vol_border = {
		190,
		200,
		190+200,
		232
	};
	DrawEdge(BackBuffer,&master_vol_border,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&tmp_rect,BDR_RAISEDINNER,BF_RECT);
	//effects gui
	buffy = _fcvt(gs_SOUND.effects_vol,1,&decimal,&sign);
	TextOut(BackBuffer,190,264,buffy, strlen(buffy));
	TextOut(BackBuffer,90,264,"EFFECTS", strlen("EFFECTS"));
	RECT fx_tmp_rect = {
		190,
		264,
		190+200 * gs_SOUND.effects_vol,
		296
	};
	RECT effects_vol_border = {
		190,
		264,
		190+200,
		296
	};
	DrawEdge(BackBuffer,&effects_vol_border,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&fx_tmp_rect,BDR_RAISEDINNER,BF_RECT);

	// music gui
	buffy = _fcvt(gs_SOUND.music_vol,1,&decimal,&sign);
	TextOut(BackBuffer,190,324,buffy, strlen(buffy));
	TextOut(BackBuffer,90,324,"MUSIC", strlen("MUSIC"));
	RECT music_tmp_rect = {
		190,
		324,
		190+200 * gs_SOUND.music_vol,
		360
	};
	RECT music_vol_border = {
		190,
		324,
		190+200,
		360
	};
	DrawEdge(BackBuffer,&music_vol_border,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&music_tmp_rect,BDR_RAISEDINNER,BF_RECT);

	Blitter.finish();
}

//movement
bool CGraphics::Input()
{
	CInput Input;
	Input.get_input(&SActors.at(GetActiveActor(SActors)));
	if(KEY_DOWN(VK_ESCAPE))
	{
		if(game_engine.get_state() != AreaType::MENU )
		{
			game_engine.alter_state(EGameState::MENU);
		}
		else
		{
			game_engine.alter_state(game_engine.m_old_state);
		}
		/*
		if( active_area != AreaType::MENU )
		{
			game_engine.alter_state(EGameState::MENU);
			active_area = AreaType::MENU;
		}
		else
		{
			game_engine.alter_state(EGameState::RUNNING);
			active_area = AreaType::STREET;
		}
		*/
	}

	if(KEY_DOWN(VK_F1))
	{
		fmap.create_tiles("levels/level1.txt");
		csvGUI.clear();
		std::fstream gui;
		gui.open("data/gui/gui.txt", ios::in);
		gs_csv.GS_readCSV(gui,csvGUI);
		gui.close();

		GUIObjects.clear();
		for(unsigned int obj_index = 4; obj_index < csvGUI.size()-1; obj_index++ )
		{
			GUIObject tmp_obj;
			RECT tmp_rect = {
				atoi(csvGUI.at(obj_index).at(0).c_str()),
				atoi(csvGUI.at(obj_index).at(1).c_str()),
				atoi(csvGUI.at(obj_index).at(2).c_str()),
				atoi(csvGUI.at(obj_index).at(3).c_str())
			};

			tmp_obj.area = tmp_rect;
			tmp_obj.z = atoi(csvGUI.at(obj_index).at(4).c_str());
			tmp_obj.action = csvGUI.at(obj_index).at(5).c_str();
			tmp_obj.location = csvGUI.at(obj_index).at(6).c_str();
			tmp_obj.text = csvGUI.at(obj_index).at(7).c_str();

			GUIObjects.push_back(tmp_obj);
		}
	}
	return false;
}

struct myclass 
{
  int current;
  myclass() {current=0;}
	void operator() (RECT i) 
	{
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_FACE_ICONS]);
		Blitter.tqueue(BackBuffer, i.left, i.top, 64, 64, Tiles_DC, 96*current, 0, RGB(255,255,255), 0);
		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		++current;
	}
} myobject;

struct HP_Renderer 
{
  int current;
  POINT pt;
  COLORREF color;
  float units;
  HP_Renderer() {current=0;}
  void set_mouse(POINT p){ pt=p; };

  void set_color(COLORREF c)
  {
	  color = c;
  };

  void operator() (pod_player p)
	{
			_itoa(current,buffer,10);
			TextOut(BackBuffer,rect.right - 96,rect.bottom - 96,buffer, strlen(buffer));
	}

  void operator() (RECT i)
	{
		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		InflateRect(&i, -1, -1);
		HBRUSH brush;
		brush = CreateSolidBrush(color);

		FillRect(BackBuffer, &i, brush);
		DeleteObject(brush);

		if(PtInRect(&i,pt))
		{
			_itoa(current,buffer,10);
			TextOut(BackBuffer,rect.right - 96,rect.bottom - 96,buffer, strlen(buffer));
			DrawFocusRect(BackBuffer,&i);
			if(KEY_DOWN(VK_LBUTTON))
			{
			}
		}

		++current;
	}
} Render_hp;


struct optionclass {
  int current;
  optionclass() {current=0;}
	void operator() (RECT i) 
	{
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_OPTION_ICONS]);
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


struct myclass2 {
  int current;
  myclass2() {current=0;}
	void operator() (RECT i) 
	{
		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		InflateRect(&i, -1, -1);
		HBRUSH hbr,hbb,hby,oldbrBrush,oldbbBrush,oldbyBrush;
		hbr = CreateSolidBrush(RGB(250, 25, 5));
		hbb = CreateSolidBrush(RGB(25, 5, 255));
		hby = CreateSolidBrush(RGB(255, 255, 0));

		oldbrBrush  = (HBRUSH)SelectObject(BackBuffer,hbr);
		oldbbBrush  = (HBRUSH)SelectObject(BackBuffer,hbb);
		oldbyBrush  = (HBRUSH)SelectObject(BackBuffer,hby);

		FillRect(BackBuffer, &i, hby);
		DeleteObject(hbr);
		DeleteObject(hbb);
		DeleteObject(hby);

		++current;
	}
} myobject2;

struct a_info_square
{
	RECT r;
	int x;
	int y;
	a_info_square()
	{
		x=y = 0;
	}
	void set(int i_x, int i_y, RECT i_r)
	{
		r = i_r; 
		x = i_x;
		y = i_y;
	}
	void render(HDC i_hdc, string i_str)
	{
		DrawEdge(i_hdc,&r,BDR_RAISEDINNER,BF_RECT);
		TextOut(BackBuffer,x,y,i_str.c_str(), 
			strlen(i_str.c_str()));
	}
} test_square;


class X
{
    public:
        void doWhat(POINT x) {}
        bool IsOK() const {return true;}
};
class CallWhatIfOk
{
    public:
        CallWhatIfOk(POINT p): param(p) {}

        void operator()(X& x) const
        {   if (x.IsOK())    {x.doWhat(param);}}
    private:
        POINT param;
};

// Get random object from a list should be used for the quests
// call this everytime there should be a new quest with the CSVQuest item

template<typename T, typename F>
typename T::iterator& get_random(T & list,const F & random)
{
  return std::advance(list.begin(),random(0,list.size()-1));
}

void ScrollViewPort(RECT& ViewPort,POINT pt)
{
	// Todo: Add scrolling for when actor moves outside the viewport
	if( pt.y <= Screen.top + 5 && fmap.get_offsetY() > 0)
	{
		cout << fmap.get_offsetY() << "Calling ScrollViewPort" << endl;

		//fmap.offsetY(-64);
		Sleep(100);
	}
	if( pt.y >= Screen.bottom - 5 && fmap.get_offsetY() < fmap.get_height()*64)
	{
		cout << fmap.get_offsetY()  << "Calling ScrollViewPort" << endl;

		//fmap.offsetY(64);
		Sleep(100);
	}
	if( pt.x <= Screen.left + 5 && fmap.get_offsetX() > 0)
	{
		cout << fmap.get_offsetX() << "Calling ScrollViewPort" << endl;

		//fmap.offsetX(-64);
		Sleep(100);
	}
	if( pt.x >= Screen.right - 5 && fmap.get_offsetX() < fmap.get_width()*64)
	{
		cout << fmap.get_offsetX() << "Calling ScrollViewPort" << endl;

		//fmap.offsetX(64);
		Sleep(100);
	}
}

void RenderViewport(int i_x, int i_y, HBITMAP i_Tiles,POINT pt)
{
	int ViewportOffsetX = fmap.get_offsetX() / TileSize;
	int ViewportOffsetY = fmap.get_offsetY() / TileSize;

	SelectObject(Tiles_DC,i_Tiles);
	for(int col = ViewportOffsetY; col < ViewportOffsetY + i_y; col++)
	{
		for(int row = ViewportOffsetX; row < ViewportOffsetX + i_x; row++)
		{
				RECT rect_run = {(row - ViewportOffsetX)*64,
					(col - ViewportOffsetY)*64,
					(row - ViewportOffsetX)*64+64,
					(col - ViewportOffsetY)*64+64};

			Blitter.queue(BackBuffer, 
				(row - ViewportOffsetX)*64, 
				(col - ViewportOffsetY)*64, 
				64, 64, Tiles_DC, 
				(int)fmap.get_tile(col,row).dnum*64,
				0, SRCCOPY,0);
			if(PtInRect(&rect_run,pt))
			{
				DrawFocusRect(BackBuffer,&rect_run);
				if(KEY_DOWN(VK_LBUTTON))
				{
					fmap.set_tile(col,row,1);
				}

				if(KEY_DOWN(VK_RBUTTON))
				{
					fmap.set_tile(col,row,-1);
				}
			}
		}
	}


	int tmp_actorx =(SActors.at(GetActiveActor(SActors)).m_ix*64)-ViewportOffsetX*64;
	int tmp_actory =(SActors.at(GetActiveActor(SActors)).m_iy*64)-ViewportOffsetY*64;
	int tmp_viewportx = (ViewportOffsetX + i_x) * 64;
	int tmp_viewporty = (ViewportOffsetY + i_y) * 64;

	cout << "Actor position( " << SActors.at(GetActiveActor(SActors)).m_ix << "," << SActors.at(GetActiveActor(SActors)).m_iy << " )@" <<(int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_iy,SActors.at(GetActiveActor(SActors)).m_ix).dnum  << endl;
	if( tmp_actorx >= 0 && tmp_actory >= 0 && (tmp_actorx/64) <= 6 && (tmp_actory/64) <= 5)
	{
		Blitter.tqueue(BackBuffer, 
			tmp_actorx, 
			tmp_actory, 
			64, 64, Tiles_DC, 0, 0, RGB(255,0,255),0);
	}
}

bool CGraphics::Paint()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[23]);
	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[23].bmWidth,
		gs_tile.gs_bmp[23].bmHeight,
		RGB(255,0,255));

	// Drawing character icons

	std::list<RECT> RActor_faces;
	std::list<RECT> RActor_hp;
	std::list<RECT> RActor_mp;
	//std::list<RECT> RActor_xp;
	std::list<RECT>::iterator it;

	SActors.at(0).m_xp =6400;
	for(int i = 0; i < 4; i++)
	{
		static int countr = 0;
		countr++;
		RECT SActor_face = { i*64,rect.bottom - 96,i*64+64,rect.bottom - 32};
		RECT SActor_hp = { i*64,rect.bottom - 32,i*64+64,rect.bottom - 16};
		RECT SActor_mp = { i*64,rect.bottom - 16,i*64+64,rect.bottom};
		//RECT SActor_xp = { i*64,rect.bottom - 112,(i*64)+(SActors.at(0).m_xp),rect.bottom - 96};
		RActor_faces.push_back(SActor_face);
		RActor_hp.push_back(SActor_hp);
		RActor_mp.push_back(SActor_mp);
		//RActor_xp.push_back(SActor_xp);
	}

	int n = 0;

	// in city menu options
	city_options.clear();
	
	for(int opt_val = 0; opt_val < 5; ++opt_val)
	{
		int menu_offsetX = 128;
		int menu_offsetY = 256;
		int menu_spacing = 32;

		RECT tmp_right = {
			474,
			38*opt_val+146,
			537,
			38*opt_val+32+146
		};

	// left options menu buttons

	// check this out
	std::for_each( RActor_faces.begin(), RActor_faces.end(),myobject);
	Render_hp.set_mouse(Mouse);
	Render_hp.set_color(RGB(250,25,5));

	std::for_each( RActor_hp.begin(), RActor_hp.end(),Render_hp);
	Render_hp.set_color(RGB(25,250,5));

	std::for_each( RActor_mp.begin(), RActor_mp.end(),Render_hp);
	Render_hp.set_color(RGB(255,255,0));
	winLog << endl << "crap ... ";

	//std::for_each( RActor_xp.begin(), RActor_xp.end(),Render_hp);
	std::for_each( SActors.begin(), SActors.end(),Render_hp);

		city_options.push_back(tmp_right);
	}
	
	n = 0;

	// Check to se if mouse pointer is within a certain actor picture
	// and also checking if you press to activate that specific actor
	for (it=RActor_faces.begin(); it!=RActor_faces.end(); ++it)
	{
		++n;
		RECT tmp = *it;
		if(PtInRect(&tmp,Mouse))
		{
			_itoa(n,buffer,10);
			TextOut(BackBuffer,rect.right - 96,rect.bottom - 96,buffer, strlen(buffer));
			DrawFocusRect(BackBuffer,&tmp);
			if(KEY_DOWN(VK_LBUTTON))
			{
				SActors.at(GetActiveActor(SActors)).m_isactive = false;
				SActors.at(n-1).m_isactive = true;
				SActor_active = n;
				SActor_active_face = *it;
				DrawFocusRect(BackBuffer,&tmp);
			}
		}
	}

	n = 0;
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
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_iy,SActors.at(GetActiveActor(SActors)).m_ix).dnum == 17 )
					menu_state.alter_state(AreaType::GENERAL_STORE);
					//active_area = AreaType::GENERAL_STORE;
				if( n == AreaType::BANK )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_iy,SActors.at(GetActiveActor(SActors)).m_ix).dnum == 17 )
					menu_state.alter_state(AreaType::BANK);
					//active_area =	 AreaType::BANK;
				if( n == AreaType::WORLD_MAP )
					menu_state.alter_state(AreaType::WORLD_MAP);
					//active_area = AreaType::WORLD_MAP;
				if( n == AreaType::TAVERN )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_iy,SActors.at(GetActiveActor(SActors)).m_ix).dnum == 17 )
					menu_state.alter_state(AreaType::TAVERN);
					//active_area = AreaType::TAVERN;
				if( n == AreaType::GUILD )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).m_iy,SActors.at(GetActiveActor(SActors)).m_ix).dnum == 17 )
					menu_state.alter_state(AreaType::GUILD);
					//active_area = AreaType::GUILD;
			}
		}
	}
/*
	n = 0;
	for (it=right_options.begin(); it!=right_options.end(); ++it)
	{
		++n;
		RECT tmp = *it;
		if(PtInRect(&tmp,pt))
		{
			DrawFocusRect(BackBuffer,&tmp);                                                                     
			if(KEY_DOWN(VK_LBUTTON))
			{
				if( n == AreaType::WORLD_MAP )
					active_area = AreaType::WORLD_MAP;
			}
		}
	}
*/
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

	//if( active_area == AreaType::WORLD_MAP )
	if( menu_state.get_state() == AreaType::WORLD_MAP )
	{
		// what we need is a viewport starting at 64px in width
		// and 128px less in height then the actual screen
		// add a function that takes a rectangle and the pointer 
		// and check if pointer moves outside edges of viewport
		ScrollViewPort(ViewPort,Mouse);

		// Viewport tiles in x and y possible on screen
		int ViewportTilesX = ViewPort.right / TileSize;
		int ViewportTilesY = ViewPort.bottom / TileSize;

		// Viewport is set size and then there is a camera
		// that sets the viewport starting x,y coordinates
		// and the camera coordinates should be recalculated
		// to how many tiles that correspondes to and sent to
		// RenderViewport
		
		RenderViewport(ViewportTilesX-1,ViewportTilesY,gs_tile.gs_hbmp[MapTileType::STD_TILES],Mouse);
		//DrawFocusRect(BackBuffer,&ViewPort);

		// rendering character faces ontop of the map
		HBRUSH hbr,hbb,hby,oldbrBrush,oldbbBrush,oldbyBrush;
		hbr = CreateSolidBrush(RGB(250, 25, 5));
		hbb = CreateSolidBrush(RGB(25, 5, 255));
		hby = CreateSolidBrush(RGB(255, 255, 0));

		oldbrBrush  = (HBRUSH)SelectObject(BackBuffer,hbr);
		oldbbBrush  = (HBRUSH)SelectObject(BackBuffer,hbb);
		oldbyBrush  = (HBRUSH)SelectObject(BackBuffer,hby);

		// check this out
		std::for_each( RActor_faces.begin(), RActor_faces.end(),myobject);
		Render_hp.set_mouse(Mouse);
		Render_hp.set_color(RGB(250,25,5));

		std::for_each( RActor_hp.begin(), RActor_hp.end(),Render_hp);
		Render_hp.set_mouse(Mouse);
		Render_hp.set_color(RGB(25,250,5));
		std::for_each( RActor_mp.begin(), RActor_mp.end(),Render_hp);
		Render_hp.set_mouse(Mouse);
		Render_hp.set_color(RGB(255,255,0));
		//std::for_each( RActor_xp.begin(), RActor_xp.end(),Render_hp);
		std::for_each( SActors.begin(), SActors.end(),Render_hp);

		DeleteObject(hbr);
		DeleteObject(hbb);
		DeleteObject(hby);
	}


	// City action code
	//if( show_city_buttons )
	//if( active_area != AreaType::WORLD_MAP )
	if( menu_state.get_state() != AreaType::WORLD_MAP )
	{
		//General city graphics
	}

	ValidateArea(menu_state.get_state());
	//ValidateArea(active_area,pt);
	//Page flipping

	// Render complete scene
	if(KEY_DOWN(0x42))
	{
		SActors.at(GetActiveActor(SActors)).ShowInventory();
	}
	if( SActors.at(GetActiveActor(SActors)).m_ShowInventory )
	{
		RenderInventory(GetActiveActor(SActors),Mouse);
		//RemoveInventory(GetActiveActor(SActors), pt);
		EquipInventory(GetActiveActor(SActors),Mouse);
	}

	n = 0;
	for (it=RActor_faces.begin(); it!=RActor_faces.end(); ++it)
	{
		RECT tmp = *it;
		++n;
		if(PtInRect(&tmp,Mouse))
		{

		// Info window for active character
		RECT char_info = {0,0,256,256};
		//FillRect(BackBuffer, &char_info, hbr);
		if(SActors.at(n-1).m_isactive)
			SActors.at(GetActiveActor(SActors)).info(BackBuffer,(HBRUSH)RGB(0,255,0),char_info);
		}
	}

	Blitter.finish();
	return true;
}

void ReadKeyboard( char* keys )
{ 
  for (int x = 0; x < 256; x++)
    keys[x] = (char) (GetAsyncKeyState(x) >> 8);
}


void CGraphics::ValidateArea(int i_area)
{
	for(int Area = AreaType::GENERAL_STORE; Area < AreaType::BATTLE; Area++)
	{			
		if( i_area == Area )
		{
			if( Area == AreaType::SPELLBOOK )
				the_spellbook();
			if( Area == AreaType::TAVERN )
				the_tavern();
			if( Area == AreaType::BANK )
				the_battle();
			if( Area == AreaType::GUILD )
				the_guild();
			if( Area == AreaType::GENERAL_STORE )
				the_general_store();
		}
	}
}

void CGraphics::the_spellbook()
{
	std::map<string,string> DynamicText;

	if( csvSpells.at(spellbook_page).size() > 5 )
	{
		if( csvSpells.at(spellbook_page).size() >= 0 )
			DynamicText.insert(make_pair(string("STATUSBAR"),string(csvSpells.at(spellbook_page).at(0).c_str())));
		if( csvSpells.at(spellbook_page).size() >= 1 )
			DynamicText.insert(make_pair(string("SPELLBOOK_ONE"),string(csvSpells.at(spellbook_page).at(1).c_str())));
		if( csvSpells.at(spellbook_page).size() >= 2 )
			DynamicText.insert(make_pair(string("SPELLBOOK_TWO"),string(csvSpells.at(spellbook_page).at(2).c_str())));
		if( csvSpells.at(spellbook_page).size() >= 3 )
			DynamicText.insert(make_pair(string("SPELLBOOK_THREE"),string(csvSpells.at(spellbook_page).at(3).c_str())));
		if( csvSpells.at(spellbook_page).size() >= 4 )
			DynamicText.insert(make_pair(string("SPELLBOOK_FOUR"),string(csvSpells.at(spellbook_page).at(4).c_str())));
		if( csvSpells.at(spellbook_page).size() >= 5 )
			DynamicText.insert(make_pair(string("SPELLBOOK_FIVE"),string(csvSpells.at(spellbook_page).at(5).c_str())));
	}
	int tmp = csvSpells.size();
	_itoa(tmp,buffer,10);
	TextOut(BackBuffer,0,0,buffer,strlen(buffer));
	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"SPELLBOOK",DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "SPELLBOOK");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
			//active_area = AreaType::STREET;
		}
		if( GUIObjects.at(tmp_action).action == "PREV" )
		{
			// Four is the number of data rows in the data file not used for spell information
			if( spellbook_page > 4 )
			{
				spellbook_page--;
			}
		}
		if( GUIObjects.at(tmp_action).action == "NEXT" )
		{
			spellbook_page++;
			if( spellbook_page > csvSpells.size()-1 )
			{
				spellbook_page = 5;
			}
		}
	}
}

void CGraphics::EraseScreen(HWND& wnd_handle)
{
	GetClientRect(wnd_handle,&Screen);
	ValidateRect(wnd_handle,NULL);
	FillRect(BackBuffer, &Screen, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

CGraphics::CGraphics()
{
}