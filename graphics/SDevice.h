#ifndef SDEVICE_H
#define SDEVICE_H

#include "../map/gs_map.h"
/*
struct SDevice
{
	RECT rect;
	map< string, GS_BITMAPS > ImageMap;
	HDC BackBuffer,Tiles_DC;
	RECT Screen;
	RECT rect_display_setting;
	DISPLAY_DEVICE TemporaryDisplayDevice;
	DEVMODE TemporaryDeviceMode;
	list<DISPLAY_DEVICE> AvailableDisplayDevices;
	list<DEVMODE> AvailableDeviceModes;
	DEVMODE device_mode[20];
	DEVMODE curr_device_mode;
	DEVMODE CurrentDeviceSettings,DeviceRegistrySettings;

	list<DEVMODE> AvailableDeviceSettings;
	int num_modes;
	int DeviceID;


	SDevice()
	{
		num_modes = 0;
	}
	void GetCurrentDisplaySettings();
	void GetAllDisplaySettings(HWND window_handle, POINT pt);

};*/

struct SDevice
{
	GraphicsSettings GfxSettings;
	RECT rect_display_setting;
	
	DISPLAY_DEVICE TemporaryDisplayDevice;
	DEVMODE TemporaryDeviceMode;
	list<DISPLAY_DEVICE> AvailableDisplayDevices;
	list<DEVMODE> AvailableDeviceModes;
	DEVMODE device_mode[20];
	DEVMODE curr_device_mode;
	DEVMODE CurrentDeviceSettings,DeviceRegistrySettings;

	list<DEVMODE> AvailableDeviceSettings;
	int num_modes;
	int DeviceID;

	SDevice()
	{
		GfxSettings.DeviceID = 1;
		GfxSettings.AvailableDeviceModes.clear();
		GfxSettings.AvailableDeviceSettings.clear();
		GfxSettings.AvailableDisplayDevices.clear();
		GfxSettings.CurrentDeviceSettings.dmSize = sizeof(DEVMODE);
		GfxSettings.DeviceRegistrySettings.dmSize = sizeof(DEVMODE);
		GfxSettings.TemporaryDeviceMode.dmSize = sizeof(DEVMODE);
		ZeroMemory (&GfxSettings.TemporaryDisplayDevice, sizeof(GfxSettings.TemporaryDisplayDevice)) ;	
		GfxSettings.TemporaryDisplayDevice.cb = sizeof(GfxSettings.TemporaryDisplayDevice) ;
		GfxSettings.NumDevices = 0;
		GfxSettings.ModeID = 1;
		num_modes = 0;

		GfxSettings.RefreshRate = 0;
		GfxSettings.MultiSampling = 0;
		GfxSettings.VSync = 0;
		GfxSettings.AspectRatio = 0;
		GfxSettings.Gamma = 0;
	}

	void GetCurrentDisplaySettings()
	{
		// new code using a global settings struct
		//EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&curr_device_mode);
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&GfxSettings.CurrentDeviceSettings);
		EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&GfxSettings.DeviceRegistrySettings);
		// old code 
		//DEVMODE curr_device_mode = {0};
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&curr_device_mode);
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&CurrentDeviceSettings);
		EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&DeviceRegistrySettings);

		////////////////////////////////////////
		// Settings the size of dmSize before enumerating
		////////////////////////////////////////

		CurrentDeviceSettings.dmSize = sizeof(DEVMODE);
		DeviceRegistrySettings.dmSize = sizeof(DEVMODE);
	}
	
	void GetAllDisplaySettings(HWND window_handle, POINT pt)
	{
		int iDevNum = 0; // used for iterating the number of devices
		int iModeNum = 0; // used for iterating the number of modes in each device
		// new code - remember to clear every list once each call as it will only grow otherwise
		AvailableDisplayDevices.clear();
		AvailableDeviceModes.clear();
		std::string GfxResolution,GfxRefreshRate,GfxMultisampling,GfxVsync,GfxAspectsRatio,GfxGamma;


		GfxMultisampling.clear();
		GfxMultisampling += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsWidth, std::dec);
		GfxVsync.clear();
		GfxVsync += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsWidth, std::dec);
		GfxAspectsRatio.clear();
		GfxAspectsRatio += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsWidth, std::dec);
		GfxGamma.clear();
		GfxGamma += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsWidth, std::dec);


		while (EnumDisplayDevices (NULL, iDevNum++, &GfxSettings.TemporaryDisplayDevice, 0))
		{
			GfxSettings.NumDevices++;
			AvailableDisplayDevices.push_back(GfxSettings.TemporaryDisplayDevice);
			
			while (EnumDisplaySettings (GfxSettings.TemporaryDisplayDevice.DeviceName, iModeNum++, &GfxSettings.TemporaryDeviceMode))		
			{
				AvailableDeviceModes.push_back(GfxSettings.TemporaryDeviceMode);
				GfxResolution.clear();
				GfxResolution += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsWidth, std::dec);
				GfxResolution += "x";
				GfxResolution += to_string<long>(GfxSettings.TemporaryDeviceMode.dmPelsHeight, std::dec);
				GfxResolution += "x";
				GfxResolution += to_string<long>(GfxSettings.TemporaryDeviceMode.dmBitsPerPel, std::dec);
				GfxResolution += " - ID: ";
				GfxResolution += to_string<long>(GfxSettings.ModeID, std::dec);


				if( iModeNum == GfxSettings.ModeID )
				{
					TextOut(BackBuffer,Screen.left + 128,Screen.top + 16,GfxResolution.c_str(),strlen(GfxResolution.c_str()));
					GfxRefreshRate.clear();
					GfxRefreshRate += to_string<long>(GfxSettings.TemporaryDeviceMode.dmDisplayFrequency, std::dec);
				}
			}
		}

		TextOut(BackBuffer,Screen.left,Screen.top,"Settings",strlen("Settings"));
		TextOut(BackBuffer,Screen.left,Screen.top + 16,"Resolution",strlen("Resolution"));
		TextOut(BackBuffer,Screen.left,Screen.top + 32,"Refresh Rate",strlen("Refresh Rate"));
		TextOut(BackBuffer,Screen.left,Screen.top + 48,"Multisampling",strlen("Multisampling"));
		TextOut(BackBuffer,Screen.left,Screen.top + 64,"Vsync",strlen("Vsync"));
		TextOut(BackBuffer,Screen.left,Screen.top + 80,"Aspect ratio",strlen("Aspect ratio"));
		TextOut(BackBuffer,Screen.left,Screen.top + 96,"Gamma",strlen("Gamma"));

		TextOut(BackBuffer,Screen.left + 128,Screen.top + 32,GfxRefreshRate.c_str(),strlen(GfxRefreshRate.c_str()));
		TextOut(BackBuffer,Screen.left + 128,Screen.top + 48,GfxMultisampling.c_str(),strlen(GfxMultisampling.c_str()));
		TextOut(BackBuffer,Screen.left + 128,Screen.top + 64,GfxVsync.c_str(),strlen(GfxVsync.c_str()));
		TextOut(BackBuffer,Screen.left + 128,Screen.top + 80,GfxAspectsRatio.c_str(),strlen(GfxAspectsRatio.c_str()));
		TextOut(BackBuffer,Screen.left + 128,Screen.top + 96,GfxGamma.c_str(),strlen(GfxGamma.c_str()));

		RECT GameOptionsTab = {0,0,Screen.right/4,16};
		RECT GraphicsOptionsTab = {Screen.right/4,0,Screen.right/4*2,16};
		RECT AudioOptionsTab = {Screen.right/4*2,0,Screen.right/4*3,16};
		RECT KeyMappingTab = {Screen.right/4*3,0,Screen.right,16};

		RECT trigger1 = {96,16,112,32};
		RECT trigger2 = {256,16,272,32};
		RECT trigger3 = {272,16,Screen.right,32};


		RECT LowerGfxRefreshRate = {96,32,112,48};
		RECT IncreaseGfxRefreshRate = {256,32,272,48};

		RECT LowerGfxMultisampling = {96,48,112,64};
		RECT IncreaseGfxMultisampling = {256,48,272,64};


		RECT LowerGfxVsync = {96,64,112,80};
		RECT IncreaseGfxVsync = {256,64,272,80};

		RECT LowerGfxAspectsRatio = {96,80,112,96};
		RECT IncreaseGfxAspectsRatio = {256,80,272,96};

		RECT LowerGfxGamma = {96,96,112,112};
		RECT IncreaseGfxGamma = {256,96,272,112};

		SelectObject(Tiles_DC,ImageMap["data/images/buttonsmisc.bmp"].gs_hbmp);

		DrawFrameControl(
			BackBuffer,
			&GameOptionsTab,
			DFC_BUTTON,
			DFCS_BUTTONPUSH
		);

		DrawFrameControl(
			BackBuffer,
			&GraphicsOptionsTab,
			DFC_BUTTON,
			DFCS_BUTTONPUSH
		);

	DrawFrameControl(
  BackBuffer,
  &AudioOptionsTab,
  DFC_BUTTON,
  DFCS_BUTTONPUSH
);
	DrawFrameControl(
  BackBuffer,
  &KeyMappingTab,
  DFC_BUTTON,
  DFCS_BUTTONPUSH
);

DrawText(
  BackBuffer,
  "Game Options",
  strlen("Game Options"),
  &GameOptionsTab,
  DT_CENTER
);
DrawText(
  BackBuffer,
  "Graphics Options",
  strlen("Graphics Options"),
  &GraphicsOptionsTab,
  DT_CENTER
);
DrawText(
  BackBuffer,
  "Audio Options",
  strlen("Audio Options"),
  &AudioOptionsTab,
  DT_CENTER
);
DrawText(
  BackBuffer,
  "KeyMapping",
  strlen("KeyMapping"),
  &KeyMappingTab,
  DT_CENTER
);

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
  DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &trigger3,
  DFC_BUTTON,
  DFCS_BUTTONPUSH
);

	DrawFrameControl(
  BackBuffer,
  &LowerGfxRefreshRate,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);

	DrawFrameControl(
  BackBuffer,
  &IncreaseGfxRefreshRate,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);

	DrawFrameControl(
  BackBuffer,
  &LowerGfxMultisampling,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);

	DrawFrameControl(
  BackBuffer,
  &IncreaseGfxMultisampling,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &LowerGfxVsync,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &IncreaseGfxVsync,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &LowerGfxAspectsRatio,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);

	DrawFrameControl(
  BackBuffer,
  &IncreaseGfxAspectsRatio,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);
	DrawFrameControl(
  BackBuffer,
  &LowerGfxGamma,
  DFC_SCROLL,
  DFCS_SCROLLLEFT
);
	DrawFrameControl(
  BackBuffer,
  &IncreaseGfxGamma,
  DFC_SCROLL,
  DFCS_SCROLLRIGHT
);

DrawText(BackBuffer,"APPLY",strlen("APPLY"), &trigger3, DT_CENTER);

		TransparentBlt(BackBuffer, 
			16, 
			Screen.bottom - 100, 
			260, 
			50, 
			Tiles_DC, 
			0, 
			51*5,
			260,
			51,
			RGB(255,0,255));

		//DrawFocusRect(BackBuffer,&trigger1);
		//DrawFocusRect(BackBuffer,&trigger2);

		TextOut(BackBuffer,16+50,Screen.bottom - 80,"Back",strlen("Back"));

		if(PtInRect(&GameOptionsTab,pt))
		{
			DrawFrameControl(
			  BackBuffer,
			  &GameOptionsTab,
			  DFC_BUTTON,
			  DFCS_PUSHED | DFCS_BUTTONPUSH
			);
		}
		if(PtInRect(&GraphicsOptionsTab,pt))
		{
			DrawFrameControl(
			  BackBuffer,
			  &GraphicsOptionsTab,
			  DFC_BUTTON,
			  DFCS_PUSHED | DFCS_BUTTONPUSH
			);
		}
		if(PtInRect(&AudioOptionsTab,pt))
		{
			RECT SFXButton = {96,112,112,128};
			RECT MusicButton = {96,128,112,144};
			RECT SFXButtonTitle = {112,112,144,128};
			RECT MusicButtonTitle = {112,128,144,144};

			DrawFrameControl(
			  BackBuffer,
			  &AudioOptionsTab,
			  DFC_BUTTON,
			  DFCS_PUSHED | DFCS_BUTTONPUSH
			);
			DrawFrameControl(
			  BackBuffer,
			  &SFXButton,
			  DFC_BUTTON,
			  DFCS_CHECKED | DFCS_BUTTONCHECK
			);
			DrawFrameControl(
			  BackBuffer,
			  &MusicButton,
			  DFC_BUTTON,
			  DFCS_CHECKED | DFCS_BUTTONCHECK
			);
			DrawText(BackBuffer,"SFX",strlen("SFX"), &SFXButtonTitle, DT_CENTER);
			DrawText(BackBuffer,"Music",strlen("Music"), &MusicButtonTitle, DT_CENTER);
		}
		if(PtInRect(&KeyMappingTab,pt))
		{
			DrawFrameControl(
			  BackBuffer,
			  &KeyMappingTab,
			  DFC_BUTTON,
			  DFCS_PUSHED | DFCS_BUTTONPUSH
			);
		}

		if(PtInRect(&trigger1,pt))
		{
			DrawFrameControl(
			  BackBuffer,
			  &trigger1,
			  DFC_SCROLL,
			  DFCS_HOT | DFCS_SCROLLLEFT
			);
			if(KEY_DOWN(VK_LBUTTON))
			{
				if( GfxSettings.ModeID > 1 )
					GfxSettings.ModeID--;
			}
		}
		if(PtInRect(&trigger2,pt))
		{
			DrawFrameControl(
			  BackBuffer,
			  &trigger2,
			  DFC_SCROLL,
			  DFCS_HOT | DFCS_SCROLLRIGHT
			);
			if(KEY_DOWN(VK_LBUTTON))
			{
				if( GfxSettings.ModeID >= 1 && GfxSettings.ModeID < GfxSettings.NumDevices )
					GfxSettings.ModeID++;
			}
		}
		if(PtInRect(&trigger3,pt))
		{
			if(KEY_DOWN(VK_LBUTTON))
			{
				DrawFrameControl(
				  BackBuffer,
				  &trigger3,
				  DFC_BUTTON,
				  DFCS_BUTTONPUSH | DFCS_PUSHED
				);
				DEVMODE dm;	
				EnumDisplaySettings(NULL,GfxSettings.ModeID, &dm );
					ChangeDisplaySettingsEx(0,&dm,0,CDS_FULLSCREEN,0);
					//ChangeDisplaySettings(0,0);
					
					DWORD			dwExStyle;			// Window Extended Style

					DWORD			dwStyle;
					dwExStyle=WS_EX_TOPMOST;			// Window Extended Style

					dwStyle=WS_POPUPWINDOW;

					RECT WindowRect = {0,0,dm.dmPelsWidth,dm.dmPelsHeight};

					//Get Screen resolution with height - taskbar height
					RECT WorkArea;
					SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &WorkArea,0);

					AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
					rect = WindowRect;
					//MoveWindow(window_handle,0,0,WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,1);	
					SetWindowPos(window_handle, NULL, 0, 0, (int)dm.dmPelsWidth, (int)dm.dmPelsHeight,
                         SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER);
			}
		}
		// old code before global.h struct was implemented
/*
		ZeroMemory (&TemporaryDisplayDevice, sizeof(TemporaryDisplayDevice)) ;	
		TemporaryDisplayDevice.cb = sizeof(TemporaryDisplayDevice) ;

		while (EnumDisplayDevices (NULL, iDevNum++, &TemporaryDisplayDevice, 0))
		{
			TextOut(BackBuffer,Screen.right - (strlen(TemporaryDisplayDevice.DeviceString)*16),Screen.top + (iDevNum*16),TemporaryDisplayDevice.DeviceString,strlen(TemporaryDisplayDevice.DeviceString));
			AvailableDisplayDevices.push_back(TemporaryDisplayDevice);
			ZeroMemory (&TemporaryDeviceMode, sizeof(TemporaryDeviceMode));
			while (EnumDisplaySettings (TemporaryDisplayDevice.DeviceName, iModeNum++, &TemporaryDeviceMode))		
			{
				std::string resolution;
				resolution += to_string<long>(TemporaryDeviceMode.dmPelsWidth, std::dec);
				resolution += "x";
				resolution += to_string<long>(TemporaryDeviceMode.dmPelsHeight, std::dec);
				resolution += "x";
				resolution += to_string<long>(TemporaryDeviceMode.dmBitsPerPel, std::dec);
				resolution += "@";
				resolution += to_string<long>(TemporaryDeviceMode.dmDisplayFrequency, std::dec);
				AvailableDeviceModes.push_back(TemporaryDeviceMode);
				TextOut(BackBuffer,Screen.right - 100,Screen.top + (iModeNum*16),resolution.c_str(),strlen(resolution.c_str()));
		
			}
		}
		GetCurrentDisplaySettings();
		DEVMODE dm;				// The structure needed to hold the settings data
		int i=0;				// Index value that specifies the graphics mode for 
								// which information is to be obtained.
		LONG result;			// Holds the ChangeDisplaySettings() result

		i = 0;
		int YPosition = 0;

		AvailableDeviceSettings.clear();

		while ( EnumDisplaySettings(NULL,i++, &dm ) ) 
		{
			dm.dmSize = sizeof(DEVMODE);
			AvailableDeviceSettings.push_back(dm);
			TextOut(BackBuffer,16+50,Screen.bottom - 80,"Back",strlen("Back"));
			//////////////////////////////////////////////////////////////////////////
			// Print out some information
			//////////////////////////////////////////////////////////////////////////

			
			if(dm.dmBitsPerPel >= 32)
			{
				YPosition++;
			std::string DeviceName(reinterpret_cast<const char *>(dm.dmDeviceName), strlen(reinterpret_cast<const char *>(dm.dmDeviceName)));
			std::stringstream HI,LO;
			HI << (int)HIBYTE(dm.dmDriverVersion);
			LO << (int)LOBYTE(dm.dmDriverVersion);
			std::string DriverVersion = "";
			DriverVersion = LO.str() + "." + HI.str();
			
			std::string resolution;
			resolution += to_string<long>(dm.dmPelsWidth, std::dec);
			resolution += "x";
			resolution += to_string<long>(dm.dmPelsHeight, std::dec);
			resolution += "x";
			resolution += to_string<long>(dm.dmBitsPerPel, std::dec);
			resolution += "@";
			resolution += to_string<long>(dm.dmDisplayFrequency, std::dec);
			resolution += to_string<int>(DeviceID, std::dec);



			SelectObject(Tiles_DC,ImageMap["data/images/buttonsmisc.bmp"].gs_hbmp);
			TransparentBlt(BackBuffer, 
				16, 
				(YPosition*50), 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));

			TransparentBlt(BackBuffer, 
				16, 
				Screen.bottom - 100, 
				260, 
				50, 
				Tiles_DC, 
				0, 
				51*5,
				260,
				51,
				RGB(255,0,255));

			TextOut(BackBuffer,16+50,Screen.bottom - 80,"Back",strlen("Back"));

			TextOut(BackBuffer,16+50,(YPosition*50)+20,resolution.c_str(),strlen(resolution.c_str()));

			//TextOut(BackBuffer,480,(i*32),resolution.c_str(),strlen(resolution.c_str()));
			//////////////////////////////////////////////////////////////////////////
			RECT rect_display_setting = {16,(YPosition*50),276,50+(YPosition*50)};
			
			//DrawEdge(BackBuffer,&rect_display_setting,BDR_RAISEDINNER,BF_RECT);

			if(PtInRect(&rect_display_setting,pt))
			{
				DrawFocusRect(BackBuffer,&rect_display_setting);
				if(KEY_DOWN(VK_LBUTTON))
				{
					curr_device_mode = dm;
					ChangeDisplaySettingsEx(0,&dm,0,CDS_FULLSCREEN,0);
					//ChangeDisplaySettings(0,0);
					
					DWORD			dwExStyle;			// Window Extended Style

					DWORD			dwStyle;
					dwExStyle=WS_EX_TOPMOST;			// Window Extended Style

					dwStyle=WS_POPUPWINDOW;

					RECT WindowRect = {0,0,dm.dmPelsWidth,dm.dmPelsHeight};

					//Get Screen resolution with height - taskbar height
					RECT WorkArea;
					SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &WorkArea,0);

					AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
					rect = WindowRect;
					SetWindowPos(window_handle, NULL, 0, 0, (int)dm.dmPelsWidth, (int)dm.dmPelsHeight,
                         SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER);

					//MoveWindow(window_handle,0,0,WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,1);		
				}
			}
			}
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			// Test the display settings
			//////////////////////////////////////////////////////////////////////////
			// Test if the enumered mode can be set and parse the result.
			// I think it's a good idea to do this when creating a window so you can
			// be sure of its success %)
			//
			result = ChangeDisplaySettings ( &dm, CDS_TEST );
			switch ( result ) {
			
			case DISP_CHANGE_SUCCESSFUL:					// Success
				printf ( "OK\n" ); 
				break;
			
			case DISP_CHANGE_FAILED:						// Failure
				printf ( "Failed\n" ); 
				break;

			case DISP_CHANGE_BADMODE:						// Bad mode
				printf ( "Bad mode\n" ); 
				break;

			case DISP_CHANGE_BADFLAGS:						// The flags are bad
				printf ( "Bad flags\n" ); 
				break;

			case DISP_CHANGE_RESTART:						// Restart is required
				printf ( "Restart required\n" ); 
				break;
			}
		}*/
		
	}

	void WriteResolutionToFile()
	{
			// Writing the resolution to file
			std::fstream cfg;
			cfg.open("conf/cfg.txt", ios::out);
			cfg << device_mode[iModeNum].dmPelsWidth << ",";
			cfg << device_mode[iModeNum].dmPelsHeight << ",";
			cfg << "Forgotten dreams";
			cfg.close();
	}

	bool EnterFullscreen ()
	{
		DEVMODE newSettings;	
		// now fill the DEVMODE with standard settings, mainly monitor frequenzy
		EnumDisplaySettings ( NULL, 0, &newSettings );
		//  set desired screen size/res	
 		newSettings.dmPelsWidth  = 800;		
		newSettings.dmPelsHeight = 600;		
		newSettings.dmBitsPerPel = 16;				
		//set those flags to let the next function know what we want to change
 		newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		// and apply the new settings
		if ( ChangeDisplaySettings ( &newSettings, CDS_FULLSCREEN ) 
			!= DISP_CHANGE_SUCCESSFUL )
		return false; // in case of error
		else return true;
	}

	// and to reset the screen  on program exit:
	void ResetScreen ()
	{
		// this resets the screen to the registry-stored values
  		ChangeDisplaySettings ( NULL, 0 );
	}
};

#endif