#include <iostream>
using namespace std;
#include "CFileManager.h"

void CFileManager::FileLoader(std::fstream& i_stream,csvVector& i_csvVector,std::string i_filepath)
{
	i_stream.clear();
	if(i_stream.is_open())
	{
		i_stream.close();
	}

	i_stream.open(i_filepath.c_str(), ios::in);
	if(i_stream.good() )
	{
		FMFile.GS_readCSV(i_stream,i_csvVector);
	}
	i_stream.close();
}
