#ifndef CMINIMAP_H
#define CMINIMAP_H

class CVector3;
class CMap;
struct STile_data;

class CMiniMap
{
public:

	std::map<int,COLORREF> ColorMap;

	CMiniMap();
    ~CMiniMap(){};

	void Render(HDC BackBuffer, CVector3 coord, RECT Screen, POINT iOffset, CMap* const World);
};

#endif