#include "gs_CSV.h"

GS_CSV::GS_CSV()
{
}

bool GS_CSV::GS_readCSV(std::istream &input, std::vector< std::vector<std::string> > &output)
{
	output.clear();
	csvData.clear();
	std::string csvLine;
	// read every line from the stream
	while( std::getline(input, csvLine) )
	{
		std::istringstream csvStream(csvLine);
		std::vector<std::string> csvColumn;
		std::string csvElement;
		// read every element from the line that is seperated by commas
		// and put it into the vector or strings
		while( std::getline(csvStream, csvElement, ',') )
		{
			csvColumn.push_back(csvElement);
		}		
		output.push_back(csvColumn);
	}
	csvData = output;
	return true;
}

void GS_CSV::GS_printCSV()
{
	for(csvVector::iterator i = csvData.begin(); i != csvData.end(); ++i)
	{
		for(std::vector<std::string>::iterator j = i->begin(); j != i->end(); ++j)
		{
			std::cout << *j << ",";
		}
		std::cout << "\n";
	}
}

csvVector GS_CSV::GS_GetData()
{
	return csvData;
}
