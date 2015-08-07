#ifndef GS_CSV_H
#define GS_CSV_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>
using namespace std;
// typedef to save typing for the following object	
typedef std::vector< std::vector<std::string> > csvVector;

class GS_CSV 
{
public:

	GS_CSV();
	bool GS_readCSV(std::istream &input, std::vector< std::vector<std::string> > &output);

	void GS_printCSV();

	csvVector GS_GetData();

private:

	csvVector csvData;
	RECT UIObject;
};

#endif
