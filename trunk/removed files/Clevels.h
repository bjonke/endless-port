#ifndef LEVELS_H
#define LEVELS_H

class CLevels
{
private:

	int LevelFigure[20][15];

public:

	bool LoadLevel(HDC BackBuffer, RECT rect, char *LevelFile);
	CLevels();
	~CLevels() {};
};
#endif