#ifndef __MACROS_H__
#define __MACROS_H__
#include <windows.h>
#include <windowsx.h>

// Some useful macro's
#define SAFEDXREL(x) if( (x) )	\
	{							\
		(x)->Release();			\
		(x) = NULL;				\
	}

#define DXSTRUCTINIT(x) ZeroMemory( &x, sizeof( (x) ) ); \
						(x).dwSize = sizeof( (x) );

#define KEYDOWN(name,key) (name[key] & 0x80)

void Log( char *fl, long ln, char *text );
void Log1( char *fl, long ln, char *text, char *txt1 );
void Log1n( char *fl, long ln, char *text, int txt1 );
void Log2( char *fl, long ln, char *text, char *txt1, char *txt2 );
void Log2n( char *fl, long ln, char *text, char *txt1, char *txt2 );
void Log3( char *fl, long ln, char *text, char *txt1, char *txt2, char *txt3 );
void Log3n( char *fl, long ln, char *text, int txt1, int txt2, int txt3 );

void MLog( char *fl, long ln, char *text );
void MLog1( char *fl, long ln, char *text, char *txt1 );

void InitLog( void );
//char* DDErrorString( HRESULT hr );
extern unsigned int giMouseX, giMouseY, giMouseButtons;
//extern HANDLE ghMouse;

typedef struct _PCX_HEADER
{
	char Manufacturer;
	char Version;
	char Encoding;
	char BitsPerPixel;
	short XMin;
	short YMin;
	short XMax;
	short YMax;
	short HDpi;
	short VDPI;
	char Colormap[48];
	char Reserved;
	char NPlanes;
	short BytesPerLine;
	short PaletteInfo;
	short HScreenSize;
	short VScreenSize;
	char Filler[54];
} PCXHeader, *pPCXHeader;

typedef struct _TGA_HEADER
{
	char IDLength;
	char ColorMap;
	char ImageType;
	short ColorSpec_FirstEntry;
	short ColorSpec_Length;
	char ColorSpec_EntrySize;
	short ImageSpec_XOrig;
	short ImageSpec_YOrig;
	short ImageSpec_Width;
	short ImageSpec_Height;
	char ImageSpec_Depth;
	char ImageSpec_Desc;
} TGAHeader, *pTGAHeader;

#endif