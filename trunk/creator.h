#ifndef CREATOR_H
#define CREATOR_H

#include "misc/SRoom.h"

struct creator
{
	creator()
	{
	};

	SRoom create_room()
	{
		SRoom room;
		return room;
	};
};

#endif;