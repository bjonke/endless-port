#ifndef GS_ITEM_DATABASE_H
#define GS_ITEM_DATABASE_H

#include <iostream>
#include <sstream>

struct gs_item
{
	std::string title;
	std::string description;
	int image_id;
	int type;
};

struct gs_item_database
{
	RECT category;
	vector<gs_item> m_item;
	
	gs_item_database()
	{
	}

	bool add(std::string i_title, std::string i_description)
	{
		gs_item temp_item;
		temp_item.title = i_title;
		temp_item.description = i_description;
		m_item.push_back(temp_item);
		return true;
	}

	bool remove(unsigned int i_id)
	{
		if( i_id < m_item.size() )
		{
			m_item.erase( m_item.begin() + i_id );
			return true;
		}
		return false;
	}

	gs_item get( int i_id )
	{
		return m_item.at(i_id);
	}

	bool load_datafile( std::string i_filename )
	{
		ifstream datafile( i_filename.c_str(), ios_base::in);
		std::stringstream buffer;

		gs_item tmp_item;

		//buffer << datafile.rdbuf();
		while( datafile >> tmp_item.title >> tmp_item.description >> tmp_item.image_id)
		{
			m_item.push_back( tmp_item );
		}
		datafile.clear();
		datafile.close();
		return true;
	}
};

#endif