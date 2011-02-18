#ifndef GS_FONT_H
#define GS_FONT_H

HFONT gsFont;

class gs_FONT
{
private:

public:


	gs_FONT()
	{
	};

	void Initialize()
	{
		/*
		gsFont = CreateFont(46, 28, 15, 0,
			FW_NORMAL, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_ROMAN,
			"Times New Roman");
		*/

		SelectObject(BackBuffer, GetStockObject(DEFAULT_GUI_FONT));
		SetBkMode(BackBuffer, TRANSPARENT);
		SetTextColor(BackBuffer,RGB(0,255,0));
	}

	~gs_FONT() 
	{
		DeleteObject(gsFont);
	};
};
gs_FONT Text;
#endif