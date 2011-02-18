#include <cmath>
#include <map>
#include <strstream>
#include <crtdbg.h>

#include "../misc/Library.h"
#include "../graphics/CVector.h"
#include "../graphics/graphics.h"
#include "../map/cmapstuff.h"
#include "../misc/globals.h"
#include "../font/gs_FONT.h"
#include "../map/gs_map.h"
#include "../graphics/gs_BLITTER.h"
#include "../misc/CInput.h"
#include "../graphics/SDevice.h"
#include "../HP_Render.h"

#pragma comment(lib, "winmm.lib")

struct S_CSV
{
	csvVector m_Vector;
	std::fstream m_File;
};

CInventory Actors_inventory;

csvVector csvSpells, csvItems, csvQuests, csvGuilds, csvActors, csvImages, csvGUI, csvTemp;
GS_CSV gs_csv;

// TODO rename struct

struct SDrawFace 
{
  int current;
  SDrawFace() {current=0;}
	void operator() (RECT i) 
	{
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_FACE_ICONS]);
		Blitter.tqueue(BackBuffer, i.left, i.top, 64, 64, Tiles_DC, 96*current, 0, RGB(255,0,255), 0);

		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		++current;
	}
} SDrawFaceOBj;

// TODO Move to filehandler
void FileLoader(std::fstream& i_stream,csvVector& i_csvVector,std::string i_filepath)
{
	i_stream.open(i_filepath.c_str(), ios::in);
	gs_csv.GS_readCSV(i_stream,i_csvVector);
	i_stream.close();
}

// TODO Cleanup from messy code
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

	#ifdef _DEBUG

	//RedirectIOToConsole();

	#endif
	menu_state.Init(AreaType::STREET);
	systemconf << "menu_state:" << menu_state.get_state() << endl;
	glob_anim_frame_x = glob_anim_frame_y = 0;
	systemconf << "glob_anim_frame_x:" << glob_anim_frame_x << endl;
	main_hwnd = hwnd;
	//GetClientRect(main_hwnd,&rect);
	
	// Related to Paint function for rendering the map
	GetClientRect(main_hwnd,&Screen);
	//Setting size of viewport
	SetRect(&ViewPort,0,0,Screen.right-64,Screen.bottom-128);

	// Writes all the bmp filenames into a file
	systemconf << "filedata" << FileData("data/*.bmp","data/pics.txt" );

	// Loades all the data form the file level1.txt into gs_tiles
	fmap.create_tiles();

	// Loading most data from file into memory
	std::fstream spells, items, quests, npc, guilds, images, actors, gui;
	S_CSV Spells, Items, Quests, NPC, Guilds, Images, Actors, Gui;

	FileLoader(Spells.m_File,Spells.m_Vector,"data/spells/spells.txt");
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

	for(int Actor_id = 0; Actor_id < 4; Actor_id++)
	{
		systemconf << "Initiating actor " << Actor_id << endl;
		pod_player SActor;
		SActors.push_back(SActor);
		SActors.push_back(SActor);
	}

	SActors.at(0).m_isactive = true;
	std::vector<InventoryItem> tmp;
	InventoryItem dummy;
	dummy.dateAdded = 1;
	dummy.Description =" description";
	dummy.Item = "item";
	dummy.Quantity = 10;
	dummy.wholesaleCost =100;
	dummy.retailCost = 40;
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	ofstream out("data/actors/SActors.dat");
	writeVector(out,tmp);
	out.close();
	ifstream in("data/actors/SActors.dat");

	std::vector<InventoryItem> structList = readVector<InventoryItem>( in );

	gDC = GetDC(main_hwnd);

	// För BackBuffer (Dubbel) buffring
	BackBuffer = CreateCompatibleDC(gDC);
	BackBuffer_bmp = CreateCompatibleBitmap(gDC, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	Old_bmp = (HBITMAP)SelectObject(BackBuffer, BackBuffer_bmp);

	map< string, BITMAP > ImageMap;
	
	for(unsigned int i = 0; i < csvImages.size()-5; i++)
	{
		gs_tile.gs_hbmp[i] = reinterpret_cast<HBITMAP>(LoadImage(0,csvImages.at(i+4).at(0).c_str(),
			IMAGE_BITMAP,0,0,
            LR_LOADFROMFILE));
		
		GetObject(gs_tile.gs_hbmp[i],sizeof(BITMAP), &gs_tile.gs_bmp[i]);
		ImageMap[csvImages.at(i+4).at(0).c_str()] = gs_tile.gs_bmp[i];
		systemconf << "Initiating image " << i << endl;
	}
	
	Tiles_DC = CreateCompatibleDC(NULL);

	systemconf << "game engine state:" << game_engine.alter_state(EGameState::SPLASH);

	if( systemconf.is_open())
		systemconf.close();
	return true;
}

bool CGraphics::Execute()
{
	EraseScreen();
	Input();
	switch( game_engine.get_state() )
	{
	case EGameState::SPLASH:
		SplashScreen();
		break;
	case EGameState::MENU:
		Menu();
		break;
	case EGameState::MENU_OPTIONS:
		Menu();
		break;
	case EGameState::CREATE_CHARACTER:
		CreateCharacter();
		break;
	default:
		Paint();
		break;
	}
	return true;
}

// Get the id for the active actor
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
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[24]);

	TransparentBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[24].bmWidth,
		gs_tile.gs_bmp[24].bmHeight,
		RGB(255,0,255));

	SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_HAWK]);

	Blitter.finish();

	if(KEY_DOWN(VK_SPACE))
	{
		game_engine.alter_state(EGameState::MENU);
	}
}

void CGraphics::the_bank()
{
	_itoa(SActors.at(GetActiveActor(SActors)).m_gold,buffer,10);

	DynamicText.clear();

	DynamicText.insert(make_pair(string("STATUSBAR"),string(buffer)));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BANK", DynamicText);

	int tmp_action = GUIAction(GUIObjects, Mouse, "BANK");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
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

void CGraphics::RenderInventory(int i_Actor)
{
	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Inventory.size();
	

	RECT item_up = {Screen.right-64,Screen.top,Screen.right,Screen.top+64};
	RECT item_down = {Screen.right-64,64*5,Screen.right,64*6};

	DrawEdge(BackBuffer,&item_up,BDR_RAISEDINNER,BF_RECT);
	DrawEdge(BackBuffer,&item_down,BDR_RAISEDINNER,BF_RECT);

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
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_EQ]);

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
				TextOut(BackBuffer,Screen.right-128,tmp2*64,csvItems.at((*citer)).at(0).c_str(),strlen(csvItems.at((*citer)).at(0).c_str()));
				TextOut(BackBuffer,Screen.right-128,tmp2*64+16,csvItems.at((*citer)).at(1).c_str(),strlen(csvItems.at((*citer)).at(1).c_str()));
				TextOut(BackBuffer,Screen.right-128,tmp2*64+32,csvItems.at((*citer)).at(2).c_str(),strlen(csvItems.at((*citer)).at(2).c_str()));
				TextOut(BackBuffer,Screen.right-128,tmp2*64+48,csvItems.at((*citer)).at(3).c_str(),strlen(csvItems.at((*citer)).at(3).c_str()));
			}
		}
		tmp++;
	}
}

void CGraphics::EquipInventory(int i_Actor)
{
	int tmp = 0;
	int tmp2 = 0;

	int InventorySize = SActors.at(GetActiveActor(SActors)).m_Equipped.size();

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
						SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 1;
					}
					else
					{
						SActors.at(GetActiveActor(SActors)).m_Equipped.at(tmp) = 0;
					}
					Sleep(100);
					return;
				}
			}

		}
		tmp++;
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
	DynamicText.clear();

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
				store_item_page = csvItems.size()-1;
			}
			else
			{
				store_item_page--;
			}
			Sleep(100);
		}
		if( GUIObjects.at(tmp_action).action == "NEXT" )
		{
			store_item_page++;
			if( store_item_page > csvItems.size()-1 )
			{
				store_item_page = 0;
			}
			Sleep(100);
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
		}

		if( GUIObjects.at(tmp_action).action == "BUY" )
		{
			// Takes the item information from csvItems and pushes it into
			// the backpack of the active actor. pod_item is the item type
			
			if(store_item_page < 0)
			{
				store_item_page = 0;
			}

			SActors.at(GetActiveActor(SActors)).m_Inventory.push_back(store_item_page);
			SActors.at(GetActiveActor(SActors)).m_Equipped.push_back(0);
		}
	}

	if( csvItems.at(store_item_page).size() >= 0 )
	{
		int image_id = atoi(csvItems.at(store_item_page).at(1).c_str());

		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_EQ]);
		
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
	DynamicText.clear();
	cout << "Id counting down: " << SActors.at(GetActiveActor(SActors)).GetID() << endl;
	DynamicText.insert(make_pair(string("STATUSBAR"),string(csvGuilds.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(0).c_str())));
	DynamicText.insert(make_pair(string("ONE"),string(csvGuilds.at(4).at(0).c_str())));
	DynamicText.insert(make_pair(string("TWO"),string(csvGuilds.at(5).at(0).c_str())));
	DynamicText.insert(make_pair(string("THREE"),string(csvGuilds.at(6).at(0).c_str())));
	DynamicText.insert(make_pair(string("FOUR"),string(csvGuilds.at(7).at(0).c_str())));
	DynamicText.insert(make_pair(string("DESCRIPTION"),string(csvGuilds.at(SActors.at(GetActiveActor(SActors)).m_ClassID).at(1).c_str())));

	Blitter.queue_render(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"GUILD",DynamicText);

	// finds the id for the first reference of the word guild in
	// guiobjects.
	int tmp_hover = GUIHover(GUIObjects, Mouse, "GUILD");

	if( tmp_hover != -1)
	{
		if( GUIObjects.at(tmp_hover).action == "GUILD_ONE" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
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
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
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
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
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
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
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
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
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
	DynamicText.clear();

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"BATTLE",DynamicText);

	std::list<RECT> RActor_faces, RActor_hp, RActor_mp, RActor_xp;
	std::list<RECT>::iterator it;

	SActors.at(0).m_xp =6400;
	RECT SActor_face = {0,0,0,0};
	for(int i = 0; i < 4; i++)
	{
		SetRect(&SActor_face,32+i*64+(i*32),Screen.bottom - 238,96+i*64+(i*32),Screen.bottom - 174);
		RActor_faces.push_back(SActor_face);
	}
	for(int i = 0; i < 4; i++)
	{
		SetRect(&SActor_face,32+i*64+(i*32),Screen.bottom - 412,96+i*64+(i*32),Screen.bottom - 348);
		RActor_faces.push_back(SActor_face);
	}

	std::for_each( RActor_faces.begin(), RActor_faces.end(),SDrawFaceOBj);

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
				SActors.at(GetActiveActor(SActors)).m_isactive = false;
				SActors.at(n-1).m_isactive = true;
				SActor_active = n;
				SActor_active_face = *it;
				SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_HAWK]);
				AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK],it->left,it->top);
			}
		}
	}


	int tmp_action = GUIAction(GUIObjects, Mouse, "BATTLE");

	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "BATTLE_ATTACK" )
		{
			SActors.at(GetActiveActor(SActors)).m_hp -= 1;
		}
		if( GUIObjects.at(tmp_action).action == "BATTLE_DEFEND" )
		{
			SActors.at(GetActiveActor(SActors)).m_hp += 1;
		}
		if( GUIObjects.at(tmp_action).action == "BATTLE_SPELL" )
		{
			SActors.at(GetActiveActor(SActors)).m_hp += 1;
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_HAWK]);
			AnimateExplosion(BackBuffer, Tiles_DC, gs_tile.gs_bmp[MapTileType::STD_HAWK],RActor_faces.begin()->left,RActor_faces.begin()->top);
		}
	}
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
	DynamicText.clear();

	DynamicText.insert(make_pair(string("STATUSBAR"),string(csvQuests.at(quest_obj.active).at(0).c_str())));

	if( csvQuests.at(quest_obj.active).size() > 0 )
		DynamicText.insert(make_pair(string("LIST"),string(csvQuests.at(quest_obj.active).at(0).c_str())));
	if( csvQuests.at(quest_obj.active).size() > 1  )
		DynamicText.insert(make_pair(string("DESC"),string(csvQuests.at(quest_obj.active).at(1).c_str())));
	if( csvQuests.at(quest_obj.active).size() > 3 )
		DynamicText.insert(make_pair(string("REWARD"),string(csvQuests.at(quest_obj.active).at(3).c_str())));

	Blitter.queue_edge(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"TAVERN",DynamicText);
	
	quest_obj.printCSV(BackBuffer,Mouse);

	int tmp_action = GUIAction(GUIObjects, Mouse, "TAVERN");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "CLOSE" )
		{
			menu_state.alter_state(menu_state.m_old_state);
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


void CGraphics::Menu()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[25]);

	StretchBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[25].bmWidth,
		gs_tile.gs_bmp[25].bmHeight,
		SRCCOPY);

		SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);

			TransparentBlt(BackBuffer, 
				400, 
				0, 
				260/2, 
				51/2, 
				Tiles_DC, 
				528, 
				(52*5)/2,
				260/2,
				52/2,
				RGB(255,0,255));

		for(int i = 0; i < 4; i++)
		{
			RECT smallbutton = {260,0,290,30};
		}

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
			GfxDevice.GetAllDisplaySettings(main_hwnd,Mouse);
	}

	//Rendering the text on top of the buttons only
	DynamicText.clear();
	Blitter.queue_menu(BackBuffer,GUIObjects, BDR_RAISEDINNER, BF_RECT,"MENU",DynamicText);

	int tmp_hover = GUIHover(GUIObjects, Mouse, "MENU");
	if( tmp_hover != -1)
	{
		if( GUIObjects.at(tmp_hover).action == "RUN" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "CREATE_CHARACTER" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "OPTIONS" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
		if( GUIObjects.at(tmp_hover).action == "EXIT" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_hover).area.left, 
				GUIObjects.at(tmp_hover).area.top, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
			TextOut(BackBuffer,GUIObjects.at(tmp_hover).area.left,GUIObjects.at(tmp_hover).area.top,GUIObjects.at(tmp_hover).text.c_str(),strlen(GUIObjects.at(tmp_hover).text.c_str()));
		}
	}
	int tmp_action = GUIAction(GUIObjects, Mouse, "MENU");
	if( tmp_action != -1)
	{
		if( GUIObjects.at(tmp_action).action == "RUN" )
		{
			SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);
			TransparentBlt(BackBuffer, 
				GUIObjects.at(tmp_action).area.left, 
				GUIObjects.at(tmp_action).area.top, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51,
				260,
				51,
				RGB(255,0,255));

			game_engine.alter_state(EGameState::RUNNING);
		}
		if( GUIObjects.at(tmp_action).action == "CREATE_CHARACTER" )
		{
			game_engine.alter_state(EGameState::CREATE_CHARACTER);
		}
		if( GUIObjects.at(tmp_action).action == "OPTIONS" )
		{
			if(game_engine.get_state() == EGameState::MENU)
			{
				game_engine.alter_state(EGameState::MENU_OPTIONS);
			}
		}
		if( GUIObjects.at(tmp_action).action == "EXIT" )
		{
			game_engine.alter_state(EGameState::QUIT);
			PostQuitMessage(0);
		}
	}

	Blitter.finish();
}

//movement
bool CGraphics::Input()
{
	CInput Input;
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
		fmap.SaveWorld();
		fmap.create_tiles();
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

void CGraphics::RenderViewport(int i_x, int i_y, HBITMAP i_Tiles)
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

			if(fmap.get_tile(col,row).frame != 0)
			{
				Blitter.tqueue(BackBuffer, 
					(row - ViewportOffsetX)*64, 
					(col - ViewportOffsetY)*64, 
					64, 64, Tiles_DC, (int)fmap.get_tile(col,row).frame*64,
					64, RGB(255,0,255),0);
			}

			// Rendering text about tile information
			if(PtInRect(&rect_run,Mouse))
			{
				DrawFocusRect(BackBuffer,&rect_run);
				RECT TileInformation = {Screen.right/2,Screen.bottom - 64, Screen.right,Screen.bottom};

				_itoa((int)fmap.get_tile(col,row).dnum,buffer,10);
				TextOut(BackBuffer,TileInformation.left,TileInformation.top,buffer, strlen(buffer));
				_itoa((int)col,buffer,10);
				TextOut(BackBuffer,TileInformation.left+16,TileInformation.top,buffer, strlen(buffer));
				_itoa((int)row,buffer,10);
				TextOut(BackBuffer,TileInformation.left+32,TileInformation.top,buffer, strlen(buffer));
				_itoa((int)fmap.get_tile(col,row).encounter_chance,buffer,10);
				TextOut(BackBuffer,TileInformation.left+32,TileInformation.top+48,buffer, strlen(buffer));

				if( fmap.get_tile(col,row).passable )
				{
					TextOut(BackBuffer,TileInformation.left,TileInformation.top + 16,"Walkable", strlen("Walkable"));
				}
				else
				{
					TextOut(BackBuffer,TileInformation.left,TileInformation.top + 16,"Unpassable", strlen("Unpassable"));
				}
				// if player on a tile where there are a NPC active then start battle
				if(fmap.get_tile(col,row).encounter_chance > rand()%100)
				{
					TextOut(BackBuffer,TileInformation.left+32,TileInformation.top + 32,fmap.get_tile(col,row).SNPC.m_name.c_str(), strlen(fmap.get_tile(col,row).SNPC.m_name.c_str()));
					Blitter.tqueue(BackBuffer, 
						rect_run.left, 
						rect_run.top, 
						64, 64, Tiles_DC, 64,64, RGB(255,0,255),0);
				}

				if(WorldEdit)
				{
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
	}

	//Rendering actors on the map
	std::vector<pod_player>::iterator actorit;

	for (actorit=SActors.begin(); actorit!=SActors.end(); ++actorit)
	{
		int tmp_actorx =(actorit->coord.x*64)-ViewportOffsetX*64;
		int tmp_actory =(actorit->coord.y*64)-ViewportOffsetY*64;

		RECT tmp = {tmp_actorx,tmp_actory,tmp_actorx + 64,tmp_actory + 64};
		Blitter.tqueue(BackBuffer, 
		tmp_actorx, 
		tmp_actory, 
		64, 64, Tiles_DC, 0, 64, RGB(255,0,255),0);

		if(PtInRect(&tmp,Mouse))
		{
			TextOut(BackBuffer,Screen.right/2,Screen.top, actorit->m_name.c_str(),strlen(actorit->m_name.c_str()));
		}
	}

}

bool CGraphics::Paint()
{
	SelectObject(Tiles_DC,gs_tile.gs_hbmp[25]);

	StretchBlt(BackBuffer, 
		0, 
		0, 
		Screen.right, 
		Screen.bottom, 
		Tiles_DC, 
		0, 
		0,
		gs_tile.gs_bmp[25].bmWidth,
		gs_tile.gs_bmp[25].bmHeight,
		SRCCOPY);

	SelectObject(Tiles_DC,gs_tile.gs_hbmp[23]);

	StretchBlt(
		BackBuffer, 
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


	// Drawing character icons

	std::list<RECT> RActor_faces,RActor_hp,RActor_curr_hp,RActor_mp;
	std::list<RECT>::iterator it;
	SActors.at(0).m_xp = 6400;

	for(int i = 0; i < SActors.size(); i++)
	{
		int num_actors = SActors.size();
		static int countr = 0;
		countr++;

		RECT SActor_face = { i*64,Screen.bottom - 96,i*64+64,Screen.bottom - 32};
		RECT SActor_hp = { i*64,Screen.bottom - 32,i*64+64,Screen.bottom - 16};
		RECT SActor_curr_hp  = { i*64,Screen.bottom - 32,i*64+((64/SActors.at(i).m_max_hp)*SActors.at(i).m_hp),Screen.bottom - 16};
		RECT SActor_mp = { i*64,Screen.bottom - 16,i*64+64,Screen.bottom};
		RActor_faces.push_back(SActor_face);
		RActor_hp.push_back(SActor_hp);
		RActor_curr_hp.push_back(SActor_curr_hp);
		RActor_mp.push_back(SActor_mp);
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
		std::for_each( RActor_faces.begin(), RActor_faces.end(),SDrawFaceOBj);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(250,25,5));
		std::for_each( RActor_hp.begin(), RActor_hp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(50,25,5));
		std::for_each( RActor_curr_hp.begin(), RActor_curr_hp.end(),RenderHP);

		RenderHP.set_color(RGB(25,250,5));

		std::for_each( RActor_mp.begin(), RActor_mp.end(),RenderHP);
		RenderHP.set_color(RGB(255,255,0));
	
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
			TextOut(BackBuffer,Screen.right - 96,Screen.bottom - 96,buffer, strlen(buffer));
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
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).coord.y,SActors.at(GetActiveActor(SActors)).coord.x).dnum == 17 )
					menu_state.alter_state(AreaType::GENERAL_STORE);
				if( n == AreaType::BANK )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).coord.y,SActors.at(GetActiveActor(SActors)).coord.x).dnum == 17 )
					menu_state.alter_state(AreaType::BANK);
				if( n == AreaType::WORLD_MAP )
					menu_state.alter_state(AreaType::WORLD_MAP);
				if( n == AreaType::TAVERN )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).coord.y,SActors.at(GetActiveActor(SActors)).coord.x).dnum == 17 )
					menu_state.alter_state(AreaType::TAVERN);
				if( n == AreaType::GUILD )
					if( (int)fmap.get_tile(SActors.at(GetActiveActor(SActors)).coord.y,SActors.at(GetActiveActor(SActors)).coord.x).dnum == 17 )
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
		RenderViewport(ViewportTilesX-1,ViewportTilesY,gs_tile.gs_hbmp[MapTileType::STD_TILES]);

		// rendering character faces ontop of the map
		HBRUSH hbr,hbb,hby,oldbrBrush,oldbbBrush,oldbyBrush;
		hbr = CreateSolidBrush(RGB(250, 25, 5));
		hbb = CreateSolidBrush(RGB(25, 5, 255));
		hby = CreateSolidBrush(RGB(255, 255, 0));
		
		
		oldbrBrush  = (HBRUSH)SelectObject(BackBuffer,hbr);
		oldbbBrush  = (HBRUSH)SelectObject(BackBuffer,hbb);
		oldbyBrush  = (HBRUSH)SelectObject(BackBuffer,hby);

		// check this out
		//std::for_each( RActor_faces.begin(), RActor_faces.end(),SDrawFaceOBj);
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[MapTileType::STD_FACE_ICONS]);
		Blitter.tqueue(BackBuffer, 500, 0, 64, 64, Tiles_DC, (SActors.at(GetActiveActor(SActors)).m_Sprite-1)*96, 0, RGB(255,0,255), 0);

		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(250,25,5));

		std::for_each( RActor_hp.begin(), RActor_hp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(50,25,5));
		std::for_each( RActor_curr_hp.begin(), RActor_curr_hp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(25,250,5));
		std::for_each( RActor_mp.begin(), RActor_mp.end(),RenderHP);
		RenderHP.set_mouse(Mouse);
		RenderHP.set_color(RGB(255,255,0));
		std::for_each( SActors.begin(), SActors.end(),RenderHP);

		DeleteObject(hbr);
		DeleteObject(hbb);
		DeleteObject(hby);
	}

	ValidateArea(menu_state.get_state());
	//Page flipping

	// Render complete scene Rendering the inventory
	if(KEY_DOWN(0x42))
	{
		SActors.at(GetActiveActor(SActors)).ShowInventory();
	}
	if(KEY_DOWN(VK_F5))
	{
		WorldEdit = !WorldEdit;
		Sleep(100);
	}

	if( SActors.at(GetActiveActor(SActors)).m_ShowInventory )
	{
		RenderInventory(GetActiveActor(SActors));
		EquipInventory(GetActiveActor(SActors));
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

			if(SActors.at(n-1).m_isactive)
			{
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
				SActors.at(GetActiveActor(SActors)).info(BackBuffer,(HBRUSH)RGB(0,255,0),char_info);
			}
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
				the_battle();
			if( Area == AreaType::GUILD )
				the_guild();
			if( Area == AreaType::GENERAL_STORE )
				the_general_store();
		}
	}
}

void CGraphics::CreateCharacter()
{
	std::list<RECT> RActor_faces,RActor_hp,RActor_curr_hp,RActor_mp;
	std::list<RECT>::iterator it;

	static int face = 1;
	RECT SActor_face = { 400,Screen.top,464,Screen.top+64};
	if(PtInRect(&SActor_face,Mouse))
	{
		if(KEY_DOWN(VK_LBUTTON))
		{
			if( face < 8)
			face++;
		}

		if(KEY_DOWN(VK_RBUTTON))
		{
			if(face > 1)
			face--;
		}
		Sleep(100);
	}

	SActors.at(GetActiveActor(SActors)).m_Sprite = face;
	RActor_faces.clear();
	for(int i = 0; i < face; i++)
		RActor_faces.push_back(SActor_face);

	std::for_each( RActor_faces.begin(), RActor_faces.end(),SDrawFaceOBj);

	static int count = 0;
		SelectObject(Tiles_DC,gs_tile.gs_hbmp[27]);

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
				RGB(255,0,255));
			TextOut(BackBuffer,0,0,"Name", strlen("Name"));
			TransparentBlt(BackBuffer, 
				356, 
				400, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));
			TextOut(BackBuffer,356,400,"Cancel", strlen("Cancel"));
			RECT GENDER = {0,104,260,156};
			if(PtInRect(&GENDER,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Sex = 0;
				}

				if(KEY_DOWN(VK_RBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Sex = 1;
				}
			}
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
				RGB(255,0,255));
			TextOut(BackBuffer,0,104,"Gender", strlen("Gender"));
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
				RGB(255,0,255));
			_itoa(SActors.at(GetActiveActor(SActors)).m_ClassID,buffer,10);

			switch(SActors.at(GetActiveActor(SActors)).m_ClassID)
			{
				case 4: TextOut(BackBuffer,0,156,"Fighter", strlen("Fighter")); break;
				case 5: TextOut(BackBuffer,0,156,"Mage", strlen("Mage")); break;
				case 6: TextOut(BackBuffer,0,156,"Cleric", strlen("Cleric")); break;
				case 7: TextOut(BackBuffer,0,156,"Thief", strlen("Thief")); break;
				default: TextOut(BackBuffer,0,156,"Error", strlen("Error")); break;
			}

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
				RGB(255,0,255));
			TextOut(BackBuffer,0,400,"Save character", strlen("Save character"));

		for(int i = 0; i < 6; i++)
		{
			RECT trigger1 = {0,200,32,232};
			RECT trigger2 = {0,233,32,265};
			RECT trigger3 = {0,266,32,298};
			RECT trigger4 = {0,299,32,331};
			RECT trigger5 = {0,332,32,364};
			RECT trigger6 = {0,365,32,397};

			RECT trigger7 = {64,200,96,232};
			RECT trigger8 = {64,233,96,265};
			RECT trigger9 = {64,266,96,298};
			RECT trigger10 = {64,299,96,331};
			RECT trigger11 = {64,332,96,364};
			RECT trigger12 = {64,365,96,397};

			if(PtInRect(&trigger1,Mouse))
			{
				if(KEY_DOWN(VK_LBUTTON))
				{
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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
					if(SActors.at(GetActiveActor(SActors)).m_StatPoints < 10)
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

			if(KEY_DOWN(VK_LBUTTON))
				{
					SActors.at(GetActiveActor(SActors)).m_Sex = 0;
				}

		TransparentBlt(BackBuffer, 
				0, 
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
				64, 
				200+(32*i), 
				32, 
				32, 
				Tiles_DC, 
				0, 
				457,
				32,
				32,
				RGB(255,0,255));
		}
		for(int i = 0; i < 4; i++)
		{
			RECT button = {0,0,260,51};
			RECT smallbutton = {260,0,290,30};
			RECT smallbutton1 = {290,0,320,30};
			RECT smallbutton2 = {320,0,350,30};
			RECT smallbutton3 = {350,0,380,30};

		if(PtInRect(&smallbutton,Mouse))
		{
			TransparentBlt(BackBuffer, 
				260, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				260, 
				33*5,
				32,
				32,
				RGB(255,0,255));		
		}
		else
		{
			TransparentBlt(BackBuffer, 
				260, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				260, 
				0,
				32,
				32,
				RGB(255,0,255));
		}

		if(PtInRect(&smallbutton1,Mouse))
		{
			TransparentBlt(BackBuffer, 
				292, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				292, 
				33*5,
				32,
				32,
				RGB(255,0,255));		
		}
		else
		{
			TransparentBlt(BackBuffer, 
				292, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				292, 
				0,
				32,
				32,
				RGB(255,0,255));
		}
		if(PtInRect(&smallbutton2,Mouse))
		{
			TransparentBlt(BackBuffer, 
				322, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				322, 
				33*5,
				32,
				32,
				RGB(255,0,255));		
		}
		else
		{
			TransparentBlt(BackBuffer, 
				322, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				322, 
				0,
				32,
				32,
				RGB(255,0,255));
		}
		if(PtInRect(&smallbutton3,Mouse))
		{
			TransparentBlt(BackBuffer, 
				352, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				352, 
				33*5,
				32,
				32,
				RGB(255,0,255));		
		}
		else
		{
			TransparentBlt(BackBuffer, 
				352, 
				0, 
				32, 
				32, 
				Tiles_DC, 
				352, 
				0,
				32,
				32,
				RGB(255,0,255));
		}


		if(PtInRect(&button,Mouse))
		{
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
				RGB(255,0,255));
		}
		else
		{
		}
		_itoa(SActors.at(GetActiveActor(SActors)).m_Sex,buffer,10);
		TextOut(BackBuffer,GENDER.left,GENDER.top,buffer, strlen(buffer));

		_itoa(SActors.at(GetActiveActor(SActors)).m_Charisma,buffer,10);
		TextOut(BackBuffer,32,200,buffer, strlen(buffer));
		_itoa(SActors.at(GetActiveActor(SActors)).m_Constitution,buffer,10);
		TextOut(BackBuffer,32,200+(32),buffer, strlen(buffer));
		_itoa(SActors.at(GetActiveActor(SActors)).m_Dexterity,buffer,10);
		TextOut(BackBuffer,32,200+(64),buffer, strlen(buffer));
		_itoa(SActors.at(GetActiveActor(SActors)).m_Intelligence,buffer,10);
		TextOut(BackBuffer,32,200+(96),buffer, strlen(buffer));
		_itoa(SActors.at(GetActiveActor(SActors)).m_Strength,buffer,10);
		TextOut(BackBuffer,32,200+(128),buffer, strlen(buffer));
		_itoa(SActors.at(GetActiveActor(SActors)).m_Wisdom,buffer,10);
		TextOut(BackBuffer,32,200+(160),buffer, strlen(buffer));


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

void CGraphics::EraseScreen()
{
	GetClientRect(main_hwnd,&Screen);
	ValidateRect(main_hwnd,NULL);
	FillRect(BackBuffer, &Screen, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

CGraphics::CGraphics()
{
}