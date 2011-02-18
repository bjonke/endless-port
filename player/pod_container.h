#ifndef POD_CONTAINER_H
#define POD_CONTAINER_H

#include "../player/pod_item.h"

struct pod_container
{
	char* name;
	std::list<pod_item> item;

	pod_container()
	{
		name = "empty";
	};
};

#endif;