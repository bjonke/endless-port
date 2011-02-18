#ifndef CHALLOFFAME_H
#define CHALLOFFAME_H

#include "../player/SActor.h"
struct SHallOfFame
{

};

class CHallOfFame
{
private:
public:
	CHallOfFame();
	void Refresh(SActor ipod_player);
	void Render(HDC* iBackBuffer,RECT* Location);
};

#endif