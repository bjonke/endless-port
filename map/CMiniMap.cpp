#include<windows.h>
#include<map>

#include "CMiniMap.h"
#include "../misc/library.h"
#include "../graphics/CVector.h"
#include "../map/CMap.h"
#include "../map/STile_data.h"

CMiniMap::CMiniMap()
{
	ColorMap[0] = RGB(127,255,0); // Green
	ColorMap[1] = RGB(255,140,0); // Orange
	ColorMap[2] = RGB(0,0,139); // DarkBlue
	ColorMap[3] = RGB(119,136,153); // LightGray
	ColorMap[4] = RGB(169,169,169); // DarkGray
	ColorMap[5] = RGB(255,215,0); // Yellow
	ColorMap[6] = RGB(169,169,169); // DarkGray
	ColorMap[55] = RGB(208,32,144); // Cave
	ColorMap[56] = RGB(173,255,47); // house
	ColorMap[58] = RGB(255,128,255); // tower
	ColorMap[70] = RGB(255,105,180); // City
	ColorMap[7] = RGB(0,0,0); // Black
}

void CMiniMap::Render(HDC BackBuffer, CVector3 coord, RECT Screen, POINT iOffset, CMap* const World)
{
	for(int nCol = 0; nCol < World->get_height(); nCol++)
	{
		for(int nRow = 0; nRow < World->get_width(); nRow++)
		{
			STile_data aTile = World->get_tile(nCol,nRow);
			int TileIndex = aTile.Layer[MapTile::BASE] % 128; // 01234
			//TileIndex = 256 - TileIndex;
			if( nRow == 0 || nRow == World->get_width() - 1)
				SetPixelV(BackBuffer,Screen.right - World->get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,ColorMap[7]);
			else if( nCol == 0 || nCol == World->get_height() - 1)
				SetPixelV(BackBuffer,Screen.right - World->get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,ColorMap[7]);
			else
			{
				SetPixelV(BackBuffer,Screen.right - World->get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,ColorMap[TileIndex]);
			}
		}
	}
}

