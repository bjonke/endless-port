#ifndef H_GS_MONSTER_DATABASE
#define H_GS_MONSTER_DATABASE

#include <iostream>
#include <sstream>

struct gs_monster
{
	std::string title;
	std::string description;
};

struct gs_monster_database
{
	vector<gs_monster> m_monster;
	
	gs_monster_database()
	{
	}

	bool add(std::string i_title, std::string i_description)
	{
		gs_monster temp_monster;
		temp_monster.title = i_title;
		temp_monster.description = i_description;
		m_monster.push_back(temp_monster);
		return true;
	}

	bool remove(unsigned int i_id)
	{
		if( i_id < m_monster.size() )
		{
			m_monster.erase( m_monster.begin() + i_id );
			return true;
		}
		return false;
	}

	gs_monster get( int i_id )
	{
		return m_monster.at(i_id);
	}

	bool load_datafile( std::string i_filename )
	{
		ifstream datafile( i_filename.c_str(), ios_base::in);
		std::stringstream buffer;

		gs_monster tmp_monster;

		//buffer << datafile.rdbuf();
		while( datafile >> tmp_monster.title >> tmp_monster.description )
		{
			m_monster.push_back( tmp_monster );
		}
		datafile.clear();
		datafile.close();
		return true;
	}
};

#endif