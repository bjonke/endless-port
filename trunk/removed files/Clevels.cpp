#include <iostream>
#include <ctime>
#include <cstdio>
#include <fstream>
using namespace std;
#include <windows.h>
#include "graphics.h"
#include "Clevels.h"

bool CLevels::LoadLevel(HDC BackBuffer, RECT rect, char *LevelFile)
{
	DrawText(BackBuffer,"CLEVEL TEST",-1,&rect,0);
	return true;
}

CLevels::CLevels()
{
}