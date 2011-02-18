#ifndef CMAP_H
#define CMAP_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <windows.h>
using namespace std;

#include "../map/CWorld_array.h"
#include "../map/STile_data.h"
// Todo: calculate how many tiles can be view at the same time
// by dividing the the resolution with STile_size
class CMap
{
private:
	const int MIN_OFFSET;
	const int MAX_OFFSET;
	int *width, *height, *STile_size;
	int *offset_x;
	int *offset_y;
	POINT m_pt;
	CWorld_array<STile_data> tiles;
	bool mWorldEdit;
	int mLayer;

public:
	CMap (int,int,int);
    ~CMap ();
    int area () {return (*width * *height);}
	int get_width () {return (*width);}
	int get_height () {return (*height);}
	int get_STile_size () {return (*STile_size);}
	int get_offsetX() {return (*offset_x);}
	int get_offsetY() {return (*offset_y);}

	//void Passable(POINT pt);
	//int CMap::Walkable(int nCol, int nRow);
	void MiniMap(HDC BackBuffer, CVector3 coord, RECT Screen, POINT iOffset);
	void OffsetX(int o_v);
	void OffsetY(int o_v);
	void CreateTiles();
	bool SaveWorld();
	STile_data get_tile(int nCol, int nRow);
	void SetFrame(int nCol, int nRow,int value);
	void SetTile(int nCol, int nRow,double value, int iLayer);
	void SetWorldEdit(int iLayer);
	bool GetWorldEdit(int iLayer);
	void LayerUp();
	void LayerDown();
	int GetLayer();
	STile_data CMap::GetTriggers(SActor ipod);
	STile_data CMap::SetTriggers(SActor ipod, DWORD iConditions);

};
#endif