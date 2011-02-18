#include <crtdbg.h>
#include <windows.h>
#include <wininet.h>
#include <ctime>
#include <stack>
#include <fmod.h>
#include <winsock.h>
#include "../misc/main.h"
#include "../graphics/CGraphics.h"
#include "../misc/macros.h"

#include <urlmon.h> //Must add the urlmon.h to project or URLDownloadToFile will not work.

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "urlmon.lib") //Must add the lib file to project or URLDownloadToFile will not work.	srand ( (unsigned int)time(NULL) );

#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib,"fmod.lib")

extern bool Focused = true;
// The STL stack can't take a function pointer as a type //
// so we encapsulate a function pointer within a struct. //
struct StateStruct 
{
	void (*StatePointer)();
};

stack<StateStruct> g_StateStack;     // Our state stack

static int frames = 0;

std::string convertWCharArrayToString(const WCHAR * const wcharArray) {
    std::stringstream ss;

    int i = 0;
    char c = (char) wcharArray[i];
    while(c != '\0') {
        ss <<c;
        i++;
        c = (char) wcharArray[i];
    }
 
    std::string convert = ss.str();
    return convert;
}



/**********************************************************************
* Globaler
/*********************************************************************/
CGraphics* gs_Graphics = 0;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);



class BadConversion : public std::runtime_error {
 public:
   BadConversion(const std::string& s)
     : std::runtime_error(s)
     { }
 };

inline int convertToInt(const std::string& s)
{
	std::istringstream i(s);
	int x;
	if (!(i >> x))
		throw BadConversion("convertToDouble(\"" + s + "\")");
	return x;
}

inline double convertToDouble(const std::string& s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		throw BadConversion("convertToDouble(\"" + s + "\")");
	return x;
}
 void Test1()
{
	_RPT0(_CRT_WARN,"Reporting that Test == 1");
}
void Test2()
{
	_RPT0(_CRT_WARN,"Reporting that Test == 2");
}

/***************************************************************************************
	Main window entry point
****************************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR szCmdLine, int iCmdShow) 
{
	xmllog.AddTagTree("Project");
	xmllog.AddTag("Name", "XMLlogger");

	xmllog.AddTag("Type", "Project");
	xmllog.AddTag("FileName", __FILE__);
	xmllog.AddTag("Compiled", COMPILE_TIMESTAMP);

	xmllog.AddTagTree("CommentLines");
	xmllog.AddTag("CommentLine", "This is the programs entry point");
	xmllog.AddTag("CommentLine", "of multiple lines");
	xmllog.CloseTagTree();	// CommentLines

	xmllog.AddTagTree("LogEntries");
	xmllog.WriteLog("Main", "main", "Info", "Starting up", FILE_INFO);

	srand ( (unsigned int)time(NULL) );

	std::fstream cfg;
	cfg.open("conf/cfg.txt", ios::in);

	reader.readCSV(cfg,csvCfg);
	cfg.close();

	InitLog();
	
	TCHAR szAppName[] = "Forgotten dreams";
	HWND hwnd;
	MSG			msg = {0};
	WNDCLASS	wndclass ;

	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= hInstance ;
	wndclass.hIcon			= LoadIcon (hInstance, IDI_APPLICATION) ;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= szAppName ;

	if (!RegisterClass (&wndclass))
	{
		xmllog.WriteLog("Main", "main", "Error", "RegisterClass failed", FILE_INFO);
		return 0;
	}
	else
	{
		xmllog.WriteLog("Main", "main", "Info", "RegisterClass initialized...", FILE_INFO);
	}
	
	gs_Graphics = new CGraphics();

	hwnd = CreateWindow (szAppName,		// Window class name
						 TEXT ("Endless port"),	//window caption
						 WS_POPUPWINDOW,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 1024,
						 768,
						 NULL,
						 NULL,
						 hInstance,
						 NULL) ;

	ShowWindow(hwnd, iCmdShow) ;
	UpdateWindow(hwnd);

	DEVMODE dm;	
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS, &dm );
	ChangeDisplaySettingsEx(0,&dm,0,CDS_FULLSCREEN,0);

					DWORD			dwExStyle;			// Window Extended Style

					DWORD			dwStyle;
					dwExStyle=WS_EX_TOPMOST;			// Window Extended Style

					dwStyle=WS_POPUPWINDOW;

					RECT WindowRect = {0,0,dm.dmPelsWidth,dm.dmPelsHeight};

					//Get Screen resolution with height - taskbar height
					RECT WorkArea;
					SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &WorkArea,0);

					AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
					RECT rect = WindowRect;
					//MoveWindow(window_handle,0,0,WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,1);	
					SetWindowPos(hwnd, NULL, 0, 0, (int)dm.dmPelsWidth, (int)dm.dmPelsHeight,
                         SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER);


	gs_Graphics->Init(hwnd);
	gs_Graphics->game_engine.Init(EGameState::MENU);

/*
	const int CLIENT_PORT=25001; //the client port number

   struct sockaddr_in serverAddr; //server address
   struct sockaddr_in clientAddr; //client address

   int clientSock; //client sock
   char buf[100]; //buffer the message send and receive
   int serverPort; //protocol port
   int len=sizeof(struct sockaddr);

   WSADATA wsaData;
   WSAStartup(0x0202,&wsaData); //windows socket startup

   memset((char*)&clientAddr,0,sizeof(clientAddr));
   clientAddr.sin_family=AF_INET; //set client address protocol family
   clientAddr.sin_addr.s_addr=INADDR_ANY;
   clientAddr.sin_port=htons((u_short)CLIENT_PORT); //set client port

	serverAddr.sin_family=AF_INET;
	serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serverAddr.sin_port=htons((u_short)15001);
	clientSock=socket(PF_INET,SOCK_DGRAM,0);
	bind(clientSock,(LPSOCKADDR)&clientAddr,len);
	sendto(clientSock,"test",strlen("test"),0,(LPSOCKADDR)&serverAddr,len);
	memset(buf,0,sizeof(buf));
	recvfrom(clientSock,buf,sizeof(buf),0,(LPSOCKADDR)&serverAddr,&len);

   WSACleanup();
  */
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage (&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gs_Graphics->Execute(Focused);
		}
	}
	xmllog.CloseTagTree(); // xmllogEntries
	return(msg.wParam) ;
}

/*******************************************************************************
* LRESULT CALLBACK WndProc() sköter grovjobbet
/*******************************************************************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_TIMER:
		break;
	case WM_CREATE:
		break;
	case WM_CHAR:
		if(gs_Graphics->game_engine.get_state() == EGameState::CREATE_CHARACTER)
			if( wParam == 0x08)
			{
				if(gs_Graphics->actor_name.size() - 1 >= 1 )
				{
					int lastCharIndex = gs_Graphics->actor_name.size() - 1;
					gs_Graphics->actor_name.erase(lastCharIndex);
				}
			}
			else
				if(gs_Graphics->EditName == true )
					gs_Graphics->actor_name += wParam;
		break;
	case WM_SETFOCUS:
		Focused = true;
		break;
	case WM_KILLFOCUS:
		Focused = false;
		break;
    case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		break;
	case WM_MOUSEMOVE:
		gs_Graphics->Mouse.x = GET_X_LPARAM(lParam);
		gs_Graphics->Mouse.y = GET_Y_LPARAM(lParam);
		break;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}