#ifndef SACTIONS_H
#define SACTIONS_H

#include "../misc/macros.h"
#include "../player/pod_item.h"
#include "../player/pod_container.h"

struct SActions
{
	pod_skill learn_skill(char* name)
	{
		Log( __FILE__ , __LINE__ , name );
		pod_skill temp;
		temp.name = name;
		return temp;
	};

	void put_obj(pod_item& obj, pod_container& con)
	{
		Log( __FILE__ , __LINE__ , obj.name);
		Log( __FILE__ , __LINE__ , " put into ");
		Log( __FILE__ , __LINE__ , con.name );
		con.item.push_front(obj);
	};

	void get_obj(pod_container& con)
	{
		Log( __FILE__ , __LINE__ , con.item.front().name);
		Log( __FILE__ , __LINE__ , " removed from ");
		Log( __FILE__ , __LINE__ , con.name );
		con.item.pop_front();
	};

	void tackle(char* victim)
	{
		Log( __FILE__ , __LINE__ , "You tackle ");
		Log( __FILE__ , __LINE__ , victim);
	};

	void move(float x, float y)
	{
	};
};

#endif;
