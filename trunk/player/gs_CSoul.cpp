//-------------------------------------------------------------------------
// Namn: Rickard Skeppström
// E-mail: rickard_skeppstrom@msn.com
//-------------------------------------------------------------------------

#include <iostream>
using namespace std;
#include "gs_CSoul.h"

gs_CSoul::gs_CSoul() 
{ 
	++s_ID;
	//cout << "The creator made a new soul with s_ID (" << s_ID << ")" << endl;
};
gs_CSoul::~gs_CSoul() 
{
	//cout << "Reaper reposessed soul with s_ID (" << s_ID << ")" << endl;
	--s_ID;
};

int gs_CSoul::s_ID = 0;