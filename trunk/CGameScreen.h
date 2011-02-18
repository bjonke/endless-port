#ifndef _GAME_SCREEN
#define _GAME_SCREEN 1

class CGameScreen
{
public:
	CGameScreen(){};
	~CGameScreen(){};
	virtual void Draw() = 0;
	virtual void Update() = 0;
	virtual void Draw(HDC ihdc,HDC iSurface, HBITMAP hBitmap, BITMAP iBitmap) = 0;
private:
};

class CMenuScreen : public CGameScreen
{
public:
	CMenuScreen() {};
	void Draw() {};
	void Draw(HDC ihdc,HDC iSurface, HBITMAP hBitmap, BITMAP iBitmap)
	{
		SelectObject(ihdc,hBitmap);

		StretchBlt(
			iSurface, 
			0, 
			0, 
			640, 
			480, 
			ihdc, 
			0, 
			0, 
			iBitmap.bmWidth,
			iBitmap.bmHeight,
			SRCCOPY);
	};
	void Update() {};

private:
};

class CPauseScreen : public CGameScreen
{
public:
	CPauseScreen() {};
	void Draw()
	{
	};
	void Update()
	{
		cout << "EDEDE";
	};
	void Draw(HDC ihdc,HDC iSurface, HBITMAP hBitmap, BITMAP iBitmap)
	{
		SelectObject(ihdc,hBitmap);

		StretchBlt(
			iSurface, 
			0, 
			0, 
			640, 
			480, 
			ihdc, 
			0, 
			0, 
			iBitmap.bmWidth,
			iBitmap.bmHeight,
			SRCCOPY);
	};

private:
};
#endif
