#ifndef GS_CSOUL_H
#define GS_CSOUL_H

class gs_CSoul
{
public:
	static int s_ID;
	gs_CSoul();
	~gs_CSoul();


	int GetID() { return s_ID; }
};

#endif