#ifndef CSV_H
#define CSV_H
// typedef to save typing for the following object	
typedef std::vector< std::vector<std::string> > csvVector;

class CSV 
{
public:

	CSV(){};
	bool readCSV(std::istream &input, std::vector< std::vector<std::string> > &output)
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

	void printCSV()
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

	csvVector GetData()
	{
		return csvData;
	}

private:

	csvVector csvData;
	RECT UIObject;
};

CSV reader;
csvVector csvCfg;
csvVector csvData;
csvVector csvText;
#endif
