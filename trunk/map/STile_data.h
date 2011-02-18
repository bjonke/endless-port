#ifndef STILE_DATA_H
#define STILE_DATA_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <windows.h>
using namespace std;

#include "../misc/constants.h"
#include "../player/SActor.h"

const int iLayers = 100;

struct STrigger
{
	DWORD mState;
	DWORD mEvents;
	DWORD mConditions;
	DWORD mActions;
	int mTickTimer;
	int mValue;
	int mX;
	int mY;
};

struct STile_data
{
	STrigger STrigger;
	DWORD mTriggers;
	int Layer[iLayers];
	int MiniMapColor;

	STile_data()
	{
		for(int cLayer = 0; cLayer < iLayers; cLayer++)
			Layer[cLayer] = -1;
		MiniMapColor = 0;
		mTriggers = EX_1;
		mTriggers = randomDWORD();

		STrigger.mState = TRIGGER_ENABLED | TRIGGER_INITIALLY_ENABLED;
		STrigger.mConditions = NULL;
		STrigger.mEvents = NULL;
			STrigger.mTickTimer = rand()%100;
			STrigger.mValue = rand()%100;
			STrigger.mX = 0;
			STrigger.mY = 0;
	}
unsigned random(unsigned n) {
/* n must be < RAND_MAX 
 * returns a random number in range [0, n) */
  const int min_reject=RAND_MAX-RAND_MAX%n;
  int r;
  unsigned i=0;
  while ((r=rand()) >= min_reject && i<128) ++i;
  return r%n;
}
DWORD randomDWORD() {
  return DWORD(random(256))     | DWORD(random(256)<<8)
        |DWORD(random(256)<<16) | DWORD(random(256)<<24);
}



	int GetLayers()
	{
		return iLayers;
	}
};

#endif