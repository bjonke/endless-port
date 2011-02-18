#include <windows.h>
#include <list>
#include <map>
using namespace std;
//#include "../graphics/SDevice.h"
//#include "../misc/Library.h"
//#include "../map/gs_map.h"
//
//template <class T>
//std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
//{
//  std::ostringstream oss;
//  oss << f << t;
//  return oss.str();
//}
//
//void SDevice::GetCurrentDisplaySettings()
//{
//	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&curr_device_mode);
//	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&CurrentDeviceSettings);
//	EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&DeviceRegistrySettings);
//
//	////////////////////////////////////////
//	// Settings the size of dmSize before enumerating
//	////////////////////////////////////////
//
//	CurrentDeviceSettings.dmSize = sizeof(DEVMODE);
//	DeviceRegistrySettings.dmSize = sizeof(DEVMODE);
//}
//
//void SDevice::GetAllDisplaySettings(HWND window_handle, POINT pt)
//{
//	int iDevNum = 0;
//	int iModeNum = 0;
//	ZeroMemory (&TemporaryDisplayDevice, sizeof(TemporaryDisplayDevice)) ;	
//	TemporaryDisplayDevice.cb = sizeof(TemporaryDisplayDevice) ;
//
//	while (EnumDisplayDevices (NULL, iDevNum++, &TemporaryDisplayDevice, 0))
//	{
//		TextOut(BackBuffer,Screen.right - (strlen(TemporaryDisplayDevice.DeviceString)*16),Screen.top + (iDevNum*16),TemporaryDisplayDevice.DeviceString,strlen(TemporaryDisplayDevice.DeviceString));
//		AvailableDisplayDevices.push_back(TemporaryDisplayDevice);
//		ZeroMemory (&TemporaryDeviceMode, sizeof(TemporaryDeviceMode));
//		while (EnumDisplaySettings (TemporaryDisplayDevice.DeviceName, iModeNum++, &TemporaryDeviceMode))		
//		{
//			std::string resolution;
//			resolution += to_string<long>(TemporaryDeviceMode.dmPelsWidth, std::dec);
//			resolution += "x";
//			resolution += to_string<long>(TemporaryDeviceMode.dmPelsHeight, std::dec);
//			resolution += "x";
//			resolution += to_string<long>(TemporaryDeviceMode.dmBitsPerPel, std::dec);
//			resolution += "@";
//			resolution += to_string<long>(TemporaryDeviceMode.dmDisplayFrequency, std::dec);
//			AvailableDeviceModes.push_back(TemporaryDeviceMode);
//			TextOut(BackBuffer,Screen.right - 100,Screen.top + (iModeNum*16),resolution.c_str(),strlen(resolution.c_str()));
//	
//		}
//	}
//	GetCurrentDisplaySettings();
//	DEVMODE dm;				// The structure needed to hold the settings data
//	int i=0;				// Index value that specifies the graphics mode for 
//							// which information is to be obtained.
//	LONG result;			// Holds the ChangeDisplaySettings() result
//
//	i = 0;
//	int YPosition = 0;
//
//	AvailableDeviceSettings.clear();
//
//	while ( EnumDisplaySettings(NULL,i++, &dm ) ) 
//	{
//		dm.dmSize = sizeof(DEVMODE);
//		AvailableDeviceSettings.push_back(dm);
//		TextOut(BackBuffer,16+50,Screen.bottom - 80,"Back",strlen("Back"));
//		//////////////////////////////////////////////////////////////////////////
//		// Print out some information
//		//////////////////////////////////////////////////////////////////////////
//
//		
//		if(dm.dmBitsPerPel >= 32)
//		{
//			YPosition++;
//		std::string DeviceName(reinterpret_cast<const char *>(dm.dmDeviceName), strlen(reinterpret_cast<const char *>(dm.dmDeviceName)));
//		std::stringstream HI,LO;
//		HI << (int)HIBYTE(dm.dmDriverVersion);
//		LO << (int)LOBYTE(dm.dmDriverVersion);
//		std::string DriverVersion = "";
//		DriverVersion = LO.str() + "." + HI.str();
//		
//		std::string resolution;
//		resolution += to_string<long>(dm.dmPelsWidth, std::dec);
//		resolution += "x";
//		resolution += to_string<long>(dm.dmPelsHeight, std::dec);
//		resolution += "x";
//		resolution += to_string<long>(dm.dmBitsPerPel, std::dec);
//		resolution += "@";
//		resolution += to_string<long>(dm.dmDisplayFrequency, std::dec);
//		resolution += to_string<int>(DeviceID, std::dec);
//
//		RECT trigger1 = {381,200,381+32,232};
//		RECT trigger2 = {381,233,381+32,265};
//
//		// Left Arrow
//		TransparentBlt(BackBuffer, 
//				381, 
//				200+(32), 
//				32, 
//				32, 
//				Tiles_DC, 
//				0, 
//				424,
//				32,
//				32,
//				RGB(255,0,255));
//		// Right Arrow
//		TransparentBlt(BackBuffer, 
//			381+64, 
//			200+(32), 
//			32, 
//			32, 
//			Tiles_DC, 
//			0, 
//			457,
//			32,
//			32,
//			RGB(255,0,255));
//
//		if(PtInRect(&trigger1,pt))
//		{
//			if(KEY_DOWN(VK_LBUTTON))
//			{
//				DeviceID--;
//			}
//		}
//		if(PtInRect(&trigger2,pt))
//		{
//			if(KEY_DOWN(VK_LBUTTON))
//			{
//				DeviceID++;
//			}
//		}
//
//		SelectObject(Tiles_DC,ImageMap["data/images/buttonsmisc.bmp"].gs_hbmp);
//		TransparentBlt(BackBuffer, 
//			16, 
//			(YPosition*50), 
//			260, 
//			50, 
//			Tiles_DC, 
//			0, 
//			51*5,
//			260,
//			51,
//			RGB(255,0,255));
//
//		TransparentBlt(BackBuffer, 
//			16, 
//			Screen.bottom - 100, 
//			260, 
//			50, 
//			Tiles_DC, 
//			0, 
//			51*5,
//			260,
//			51,
//			RGB(255,0,255));
//
//		TextOut(BackBuffer,16+50,Screen.bottom - 80,"Back",strlen("Back"));
//
//		TextOut(BackBuffer,16+50,(YPosition*50)+20,resolution.c_str(),strlen(resolution.c_str()));
//
//		//TextOut(BackBuffer,480,(i*32),resolution.c_str(),strlen(resolution.c_str()));
//		//////////////////////////////////////////////////////////////////////////
//		RECT rect_display_setting = {16,(YPosition*50),276,50+(YPosition*50)};
//		
//		//DrawEdge(BackBuffer,&rect_display_setting,BDR_RAISEDINNER,BF_RECT);
//
//		if(PtInRect(&rect_display_setting,pt))
//		{
//			DrawFocusRect(BackBuffer,&rect_display_setting);
//			if(KEY_DOWN(VK_LBUTTON))
//			{
//				curr_device_mode = dm;
//				ChangeDisplaySettingsEx(0,&dm,0,CDS_FULLSCREEN,0);
//				//ChangeDisplaySettings(0,0);
//				
//				DWORD			dwExStyle;			// Window Extended Style
//
//				DWORD			dwStyle;
//				dwExStyle=WS_EX_TOPMOST;			// Window Extended Style
//
//				dwStyle=WS_POPUPWINDOW;
//
//				RECT WindowRect = {0,0,dm.dmPelsWidth,dm.dmPelsHeight};
//
//				//Get Screen resolution with height - taskbar height
//				RECT WorkArea;
//				SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &WorkArea,0);
//
//				AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
//				rect = WindowRect;
//				MoveWindow(window_handle,0,0,WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,1);		
//			}
//		}
//		}
//		//////////////////////////////////////////////////////////////////////////
//		//////////////////////////////////////////////////////////////////////////
//		// Test the display settings
//		//////////////////////////////////////////////////////////////////////////
//		// Test if the enumered mode can be set and parse the result.
//		// I think it's a good idea to do this when creating a window so you can
//		// be sure of its success %)
//		//
//		result = ChangeDisplaySettings ( &dm, CDS_TEST );
//		switch ( result ) {
//		
//		case DISP_CHANGE_SUCCESSFUL:					// Success
//			printf ( "OK\n" ); 
//			break;
//		
//		case DISP_CHANGE_FAILED:						// Failure
//			printf ( "Failed\n" ); 
//			break;
//
//		case DISP_CHANGE_BADMODE:						// Bad mode
//			printf ( "Bad mode\n" ); 
//			break;
//
//		case DISP_CHANGE_BADFLAGS:						// The flags are bad
//			printf ( "Bad flags\n" ); 
//			break;
//
//		case DISP_CHANGE_RESTART:						// Restart is required
//			printf ( "Restart required\n" ); 
//			break;
//		}
//	}
//}
