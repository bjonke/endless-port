#ifndef HP_Render
#define HP_Render

struct HP_Render 
{
  int current;
  POINT pt;
  COLORREF color;
  float units;
  HP_Render();
  void set_mouse(POINT p){ pt=p; };

  void set_color(COLORREF c)
  {
	  //_RPT0(_CRT_WARN,"Reporting color == 0\n");
	  color = c;
  };

  void operator() (SActor p)
	{
			_itoa(current,buffer,10);
			TextOut(BackBuffer,rect.right - 96,rect.bottom - 96,buffer, strlen(buffer));
	}

  void operator() (RECT i)
	{
		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		InflateRect(&i, -1, -1);
		HBRUSH brush;
		brush = CreateSolidBrush(color);

		FillRect(BackBuffer, &i, brush);
		DeleteObject(brush);

		if(PtInRect(&i,pt))
		{
			_itoa(current,buffer,10);
			TextOut(BackBuffer,rect.right - 96,rect.bottom - 96,buffer, strlen(buffer));
			DrawFocusRect(BackBuffer,&i);
			if(KEY_DOWN(VK_LBUTTON))
			{
			}
		}

		++current;
	}
} RenderHP;

#endif