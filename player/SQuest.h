#ifndef SQUEST_H
#define SQUEST_H

struct SQuest
{
	std::vector< std::vector<std::string> > questList;
	unsigned int active;

	void printCSV(HDC hdc,POINT p)
	{
		int counter = 0;
		int i_y = 0;
		for(std::vector< std::vector<std::string> >::iterator i = questList.begin(); i != questList.end(); ++i)
		{
			for(std::vector<std::string>::iterator j = i->begin(); j != i->end(); ++j)
			{
				TextOut(hdc,p.x,p.y + 32*i_y,i->at(0).c_str(), strlen(i->at(0).c_str()));
				counter++;
			}
			counter = 0;
			i_y++;
			std::cout << "\n";
		}
	}

	void info(HDC hdc, HBRUSH color,RECT rect)
	{
	}

};

#endif;