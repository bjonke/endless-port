#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "misc/Library.h"
//#include "filehandling/gs_CSV.h"

// typedef to save typing for the following object	
typedef std::vector< std::vector<std::string> > csvVector;

class CFileManager
{
public:

	CFileManager(){};
	void FileLoader(std::fstream& i_stream,csvVector& i_csvVector,std::string i_filepath);

private:
	GS_CSV FMFile;
};
#endif