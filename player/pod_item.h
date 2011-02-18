#ifndef POD_ITEM_H
#define POD_ITEM_H

#include <vector>
using namespace std;

struct pod_item
{
	char* name;
	string m_name;
	bool equipped;
	
	pod_item()
	{
		name = "generic soulname";
		m_name = "generic soulname";
	};
};

#endif;