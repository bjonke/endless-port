#ifndef _S_ANIMATION_
#define _S_ANIMATION_

#include <windows.h>
class SAnimation
{
private:
	HDC SourceImage;
	int Frames;
	POINT FrameSize;
    int id;
public:
    SAnimation(HDC Ihdc, int iFrames, POINT iFrameSize, int i) 
	{
		SourceImage = Ihdc; 
		Frames = iFrames; 
		FrameSize = iFrameSize; 
		id = i;
	};
    int getid()	{ return id; };
};

list<SAnimation> SAnimationlist;
list<SAnimation>::iterator iter;


#endif