#ifndef GS_MAP_H
#define GS_MAP_H

const int numImages = 200;

struct GS_TILES
{
	BITMAP gs_bmp[numImages];
	HBITMAP gs_hbmp[numImages];
};

struct GS_BITMAPS
{
	BITMAP gs_bmp;
	HBITMAP gs_hbmp;
};
#endif