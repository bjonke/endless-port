#ifndef GS_NPC_DATABASE_H
#define GS_NPC_DATABASE_H

#include <iostream>
#include <sstream>

struct gs_npc
{
	std::string title;
	std::string description;
};

struct gs_npc_database
{
	vector<gs_npc> m_npc;
	
	gs_npc_database()
	{
	}

	bool add(std::string i_title, std::string i_description)
	{
		gs_npc temp_npc;
		temp_npc.title = i_title;
		temp_npc.description = i_description;
		m_npc.push_back(temp_npc);
		return true;
	}

	bool remove(unsigned int i_id)
	{
		if( i_id < m_npc.size() )
		{
			m_npc.erase( m_npc.begin() + i_id );
			return true;
		}
		return false;
	}

	gs_npc get( int i_id )
	{
		return m_npc.at(i_id);
	}

	bool load_datafile( std::string i_filename )
	{
		ifstream datafile( i_filename.c_str(), ios_base::in);
		std::stringstream buffer;

		gs_npc tmp_npc;

		//buffer << datafile.rdbuf();
		while( datafile >> tmp_npc.title >> tmp_npc.description )
		{
			m_npc.push_back( tmp_npc );
		}
		datafile.clear();
		datafile.close();
		return true;
	}
};

#endif