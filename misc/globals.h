#ifndef GLOBALS_H
#define GLOBALS_H

#include "../misc/Library.h"

const double DegToRad ( double deg ) {
  const double PI = 3.1415926;
  return deg * PI / 180.0;
}

clock_t ClockInit, ClockFinal;

vector<string> MenuLoadActor,MenuSaveActor;

int CurrentMovementFrame = 0;
int CurrentAttackFrame = 0;
int SelectedItemId = -1;
int SelectedSpellId = 0;


struct GUIObject
{
	RECT area;
	int x,y,z;
	std::string action;
	std::string location;
	std::string text;

};

bool WorldEdit;


HWND main_hwnd;
RECT ViewPort,Screen,rect,SActor_active_face,client_window_rect;
HDC Tiles_DC = 0, gDC = 0, BackBuffer = 0;
HBITMAP BackBuffer_bmp = 0, Old_bmp = 0;
SQuest quest_obj;
std::list<RECT> city_options;
const int TileSize = 64;

CMap fmap(50,50,64);

char buffer[100];

int EnemyID = 0;
gs_banks bank; 

CInventory CActor_inventory, Shop_inventory;

std::vector<SActor> SActors,SNPC;
int active_form, active_area,active_subarea,last_active_area,window_active;
int glob_anim_frame_x,glob_anim_frame_y;

std::vector<RECT> SActors_hp,SActors_mp,SActors_xp,SActors_faces;
std::vector<GUIObject> GUIObjects;

DWORD iModeNum;

int SActor_active,top_item;

//map related
int scroll_x_map,scroll_y_map;

unsigned int store_item_page;

int QuestID = 0;


struct Enemy
{
	string EnemyName;
	string EnemyLevel;
	string EnemyMembersOnly;
	string EnemyHP;
	string EnemyMaxHP;
	string EnemyXP;
	string EnemyKillXP;

	void Init(csvVector Enemy)
	{
		EnemyName = Enemy.at(EnemyID).at(0).c_str();
		EnemyLevel = Enemy.at(EnemyID).at(1).c_str();
		EnemyMembersOnly = Enemy.at(EnemyID).at(2).c_str();
		EnemyHP = Enemy.at(EnemyID).at(3).c_str();
		EnemyMaxHP = Enemy.at(EnemyID).at(3).c_str();
		EnemyXP = Enemy.at(EnemyID).at(4).c_str();
		EnemyKillXP = Enemy.at(EnemyID).at(5).c_str();

	}

	void Reset()
	{
		EnemyName = "";
		EnemyLevel = "";
		EnemyMembersOnly = "";
		EnemyHP = "";
		EnemyXP = "";
		EnemyMaxHP = "";
		EnemyKillXP = "";
	}
} SEnemy;

std::map<int,int> city_visited;

int GUIAction(std::vector<GUIObject> i_obj, POINT i_pt, std::string i_type)
{
	for(int obj_index = i_obj.size()-1; obj_index >= 0; obj_index--)
	{
		if( PtInRect(&i_obj.at(obj_index).area,i_pt) )
		{
			if( i_obj.at(obj_index).location == i_type )
			if(KEY_DOWN(VK_LBUTTON))
			{
				return obj_index;
			}
		}
	}
	return -1;
}

int GUIHover(std::vector<GUIObject> i_obj, POINT i_pt, std::string i_type)
{
	for(int obj_index = i_obj.size()-1; obj_index >= 0; obj_index--)
	{
		if( PtInRect(&i_obj.at(obj_index).area,i_pt) )
		{
			if( i_obj.at(obj_index).location == i_type )
			return obj_index;
		}
	}
	return -1;
}

bool zDataSortPredicate(const GUIObject& d1, const GUIObject& d2)
{
	return d1.z < d2.z;
}
DWORD CALLBACK CopyProgressRoutine(
LARGE_INTEGER TotalFileSize,
                 LARGE_INTEGER TotalBytesTransferred,
                 LARGE_INTEGER StreamSize,
                LARGE_INTEGER StreamBytesTransferred,
                 DWORD dwStreamNumber,
                 DWORD dwCallbackReason,
                 HANDLE hSourceFile,
                 HANDLE hDestinationFile,
                 LPVOID lpData
);

struct argp_state
{
	int something;
	string text;
};
void  silly( void *arg )
{
	while(1)
	{
		//struct argp_state* arguments = (struct argp_state *)arg;
		
		string text = "finkel";
		int loops = 100;
		for(int i = 0; i < loops; i++)
		{
			TextOut(BackBuffer,50,16*i,text.c_str(), strlen(text.c_str()));
			//Sleep(100);
		}
	}
}


class MyThread : public CThread
{
public:
	virtual void run()
	{
		TextOut(BackBuffer,50,16,"Programming", strlen("Programming"));  
	}

};

class CDemoThread : public CThread
{
  virtual DWORD Run( LPVOID arg )
  { 
    while(1)
    {
		TextOut(BackBuffer,50,16,"Programming", strlen("Programming"));      
		//SleepEx(1 * 1000, FALSE);
    }
  }
};

DWORD WINAPI Threaded( LPVOID lpData )
{
  while(1)
  {
	GetClientRect(main_hwnd,&Screen);
	ValidateRect(main_hwnd,NULL);
	FillRect(BackBuffer, &Screen, (HBRUSH)GetStockObject(BLACK_BRUSH));
	TextOut(BackBuffer,50,16,"Programming", strlen("Programming"));
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

	//CopyFileEx((LPCSTR)"data/logs/data2.pck", (LPCSTR)"data/logs/data1.pck", (LPPROGRESS_ROUTINE)&CopyProgressRoutine, NULL, false, COPY_FILE_FAIL_IF_EXISTS | COPY_FILE_RESTARTABLE);
	//Sleep(10000);
  }
}

DWORD CALLBACK MyUpdateProgress(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData)
{
return true;
}
// Filehandling - TransmitFile for network

DWORD CALLBACK CopyProgressRoutine(
LARGE_INTEGER TotalFileSize,
                 LARGE_INTEGER TotalBytesTransferred,
                 LARGE_INTEGER StreamSize,
                LARGE_INTEGER StreamBytesTransferred,
                 DWORD dwStreamNumber,
                 DWORD dwCallbackReason,
                 HANDLE hSourceFile,
                 HANDLE hDestinationFile,
                 LPVOID lpData
)
{
double nPercentage = (double(TotalBytesTransferred.QuadPart*100))/((double)TotalFileSize.QuadPart);//tcount += TotalBytesTransferred;

if(nPercentage >= 100)

{
	return PROGRESS_STOP;
}
else
{
	/*
	GetClientRect(main_hwnd,&Screen);
	ValidateRect(main_hwnd,NULL);
	FillRect(BackBuffer, &Screen, (HBRUSH)GetStockObject(BLACK_BRUSH));
*/
	RECT progress = {0,0,0,50};
		stringstream output;
		output << nPercentage;
		string finalString = output.str();
		DrawText(
		  BackBuffer,
		  finalString.c_str(),
		  strlen(finalString.c_str()),
		  &progress,
		  DT_SINGLELINE | DT_CENTER | DT_VCENTER
		);
/*
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
			*/
}

return PROGRESS_CONTINUE;
};

HRESULT CopyItem(__in PCWSTR pszSrcItem, __in PCWSTR pszDest, PCWSTR pszNewName)
{
    //
    // Initialize COM as STA.
    //
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); 
    if (SUCCEEDED(hr))
    {
        IFileOperation *pfo;
  
        //
        // Create the IFileOperation interface 
        //
        hr = CoCreateInstance(CLSID_FileOperation, 
                              NULL, 
                              CLSCTX_ALL, 
                              IID_PPV_ARGS(&pfo));
        if (SUCCEEDED(hr))
        {
            //
            // Set the operation flags. Turn off all UI from being shown to the
            // user during the operation. This includes error, confirmation,
            // and progress dialogs.
            //
            hr = pfo->SetOperationFlags(NULL);
            if (SUCCEEDED(hr))
            {
                //
                // Create an IShellItem from the supplied source path.
                //
                IShellItem *psiFrom = NULL;
                hr = SHCreateItemFromParsingName(pszSrcItem, 
                                                 NULL, 
                                                 IID_PPV_ARGS(&psiFrom));
                if (SUCCEEDED(hr))
                {
                    IShellItem *psiTo = NULL;
  
                    if (NULL != pszDest)
                    {
                        //
                        // Create an IShellItem from the supplied 
                        // destination path.
                        //
                        hr = SHCreateItemFromParsingName(pszDest, 
                                                         NULL, 
                                                         IID_PPV_ARGS(&psiTo));
                    }
                    
                    if (SUCCEEDED(hr))
                    {
                        //
                        // Add the operation
                        //
                        hr = pfo->CopyItem(psiFrom, psiTo, pszNewName, NULL);

                        if (NULL != psiTo)
                        {
                            psiTo->Release();
                        }
                    }
                    
                    psiFrom->Release();
                }
                
                if (SUCCEEDED(hr))
                {
                    //
                    // Perform the operation to copy the file.
                    //
                    hr = pfo->PerformOperations();
                }        
            }
            
            //
            // Release the IFileOperation interface.
            //
            pfo->Release();
        }
  
        CoUninitialize();
    }
    return hr;
}


bool FileData(std::string iPath,std::string out_filename )
{
	WIN32_FIND_DATA findFileData;
	LARGE_INTEGER filesize;
	HANDLE hFind = FindFirstFile((LPCSTR)iPath.c_str(), &findFileData);
    
    if(hFind  == INVALID_HANDLE_VALUE) 
	{
        std::cout <<"No files found." <<std::endl;
		return false;
    } 
	else 
	{
        std::cout <<"Files found." <<std::endl;
    }
    
    int fileNumber = 0;
	char buffer[100];

    while(FindNextFile(hFind, &findFileData)) 
	{
		DWORD dw = GetLastError();

		//CopyFileEx("P:\forgotten-dreams\data\logs\copied.txt","P:\forgotten-dreams\data\logs\images.txt", NULL, NULL, false, COPY_FILE_FAIL_IF_EXISTS | COPY_FILE_RESTARTABLE);
        fileNumber++;
		fstream file_op(out_filename.c_str(),ios::out | ios::app);
		  if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		  {
			  file_op << findFileData.cFileName << "<DIR>"<<endl;
		  }
		  else
		  {
			filesize.LowPart = findFileData.nFileSizeLow;
			filesize.HighPart = findFileData.nFileSizeHigh;
			file_op << findFileData.cFileName << filesize.QuadPart<< "bytes"<<endl;
		  }
		file_op.close();
    }
 
	_itoa(fileNumber,buffer,10);
    FindClose(hFind);
	return true;
}

// Filehandling

// Purpose: Checking to see that all the savegames are intact
// and where they should be
// 1. Count the number of files in the directory
// 2. Check there names, should be saveslot<num>.sav 1-9
// 3. Validate content
bool CheckSaveGames(std::string iPath,std::string OutFileName)
{
	WIN32_FIND_DATA FindFileData;
	LARGE_INTEGER FileSize;
	int FileNumber = 0;
	int NumSaveSlots = 9;

	HANDLE hFind = FindFirstFile((LPCSTR)iPath.c_str(), &FindFileData);

	fstream FileOp(OutFileName.c_str(),ios::out);

    if(hFind  == INVALID_HANDLE_VALUE) 
	{
		std::cout <<"No savegame files found." << std::endl;
		return false;
    } 
	else 
	{
		FileSize.LowPart = FindFileData.nFileSizeLow;
		FileSize.HighPart = FindFileData.nFileSizeHigh;
		FileOp << FindFileData.cFileName << FileSize.QuadPart<< " bytes" <<endl;
    }
    


    while(FindNextFile(hFind, &FindFileData)) 
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			FileOp << FindFileData.cFileName << "<DIR>"<<endl;
		}
		else
		{
			FileSize.LowPart = FindFileData.nFileSizeLow;
			FileSize.HighPart = FindFileData.nFileSizeHigh;
			FileOp << FindFileData.cFileName << FileSize.QuadPart<< "bytes"<<endl;
			
			FileNumber++;
		}
    }

	if( FileNumber == NumSaveSlots )
	{
		FileOp << "Correct number of savegame slots!" <<endl;
		return true;
	}
	else
	{
		FileOp << "Wrong number of savegame slots!" <<endl;
		return false;
	}
 
	_itoa(FileNumber,buffer,10);
	FileOp.close();
    FindClose(hFind);

	return true;
}

// Purpose: Return a list of character filenames for rendering
bool ListCharacters(std::string iPath,std::string out_filename )
{
	MenuLoadActor.clear();
	WIN32_FIND_DATA findFileData;
	LARGE_INTEGER filesize;
	int fileNumber = 0;
	HANDLE hFind = FindFirstFile((LPCSTR)iPath.c_str(), &findFileData);

	fstream file_op(out_filename.c_str(),ios::out);

    if(hFind  == INVALID_HANDLE_VALUE) 
	{
        std::cout <<"No files found." <<std::endl;
		return false;
    } 
	else 
	{
		filesize.LowPart = findFileData.nFileSizeLow;
		filesize.HighPart = findFileData.nFileSizeHigh;
		file_op << findFileData.cFileName << filesize.QuadPart<< "bytes"<<endl;
		MenuLoadActor.push_back(findFileData.cFileName);
    }
    


    while(FindNextFile(hFind, &findFileData)) 
	{
        fileNumber++;
		  if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		  {
			  file_op << findFileData.cFileName << "<DIR>"<<endl;
		  }
		  else
		  {
			filesize.LowPart = findFileData.nFileSizeLow;
			filesize.HighPart = findFileData.nFileSizeHigh;
			file_op << findFileData.cFileName << filesize.QuadPart<< "bytes"<<endl;
			MenuLoadActor.push_back(findFileData.cFileName);
		  }
    }
 
	_itoa(fileNumber,buffer,10);
	file_op.close();
    FindClose(hFind);

	return true;
}

bool InitConfig(std::string i_cfgfile,std::string o_cfgfile)
{
	std::fstream cfg;
	cfg.open(i_cfgfile.c_str(), ios::in);

	//reader.readCSV(cfg,csvCfg);
	cfg.close();
	return true;
}

// Usage:
// std::list<unit*> units;
// unit* u = *get_random(units,random);

/*
template<typename T, typename F>
typename T::iterator& get_random(T & list,const F & random)
{
  return std::advance(list.begin(),random(0,list.size()));
};

int random(int min, int max)
{
  assert (max > min);
  return std::rand() % (max - min) + min;;
};
*/

// map<string, jItem> Items;
// Items["shortsword"]  shortsword is the filename o load from

template <class T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
{
  std::ostringstream oss;
  oss << f << t;
  return oss.str();
}

// Get random object from a list should be used for the quests
// call this everytime there should be a new quest with the CSVQuest item

template<typename T, typename F>
typename T::iterator& get_random(T & list,const F & random)
{
  return std::advance(list.begin(),random(0,list.size()-1));
}

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


struct myclass2 {
  int current;
  myclass2() {current=0;}
	void operator() (RECT i) 
	{
		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		InflateRect(&i, -1, -1);
		HBRUSH hbr,hbb,hby,oldbrBrush,oldbbBrush,oldbyBrush;
		hbr = CreateSolidBrush(RGB(255, 0, 0));
		hbb = CreateSolidBrush(RGB(0, 0, 255));
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

struct S_CSV
{
	csvVector m_Vector;
	std::fstream m_File;
};

S_CSV Spells, Items, Quests, NPC, Guilds, Images, Actors, Monsters, Gui;

CFileManager FileManager;
map< string, GS_BITMAPS > ImageMap;
GS_TILES gs_tile;

std::vector<CGameObjects*> backpack;
std::vector<CGameObjects*> WorldItems;
vector<Companion*> Beastiary;
vector<Companion*> ActorCompanions;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct GraphicsSettings
{
	int DeviceID; // ID of the device currently used
	int ModeID; // ID of the current resolution used
	int NumDevices; // Number of devices enumrated
	DISPLAY_DEVICE TemporaryDisplayDevice;
	DEVMODE TemporaryDeviceMode;
	list<DISPLAY_DEVICE> AvailableDisplayDevices;
	list<DEVMODE> AvailableDeviceModes;
	DEVMODE CurrentDeviceSettings,DeviceRegistrySettings;
	list<DEVMODE> AvailableDeviceSettings;

	int RefreshRate;
	int MultiSampling;
	int VSync;
	int AspectRatio;
	int Gamma;
};

struct SDevice;
CGuilds GameGuilds;
CGameMessages GameMessageWindow;
#endif