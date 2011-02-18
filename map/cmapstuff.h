#ifndef CMAPSTUFF_H
#define CMAPSTUFF_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <windows.h>
using namespace std;
#include "../player/SActor.h"
#include "../map/STile_data.h"

void test_user_rec();
void tester();
// walkable(true,false) - Layer[2](animation)
template <typename T>
class dynamic_2d_array{
public:    dynamic_2d_array(){};
		   dynamic_2d_array(int rows, int cols):m_data(rows, std::vector<T>(cols)){}
		   inline std::vector<T> & operator[](int i) { return m_data[i];}
		   inline const std::vector<T> & operator[] (int i) const { return m_data[i];}
		   void resize(int rows, int cols)
		   {
			   m_data.resize(rows);       
			   for(int i = 0;i < rows;++i)
			   {
				   m_data[i].resize(cols);
			   }
		   }
private:    
	std::vector<std::vector<T> > m_data;  
};

struct world_struct
{/* map cell */
   unsigned char reg;
   long ore,crops,wood,stone;   /* raw materials */
   char structure;              /* bdg is a: 0 city, 1 facility, 2 artifact STR_* */
};
/*
struct STile_data
{
	double dnum;
	bool passable;
	int inum, Layer[2], encounter_chance;
	SActor SNPC;
	STile_data()
	{
		dnum = 0;
		inum = 0;
		Layer[2] = 0;
		encounter_chance = rand()%100;
		SNPC.Init();
		SNPC.m_IsActive = (bool)((rand() % 2) != 0);
	}
};
*/
struct MAPFILEHDR{
    //Map file header
    char EngName[16];   //Map engine name.
    int EngVersion;     //Map engine version.
    char MapName[16];   //Map short name.
    char MapDesc[198];  //Map long description.
    char Tileset[16];   //Name of tileset used for map.
    long MapWidth;      //Width of the map.
    long MapHeight;     //Height of the map.
};

struct s
{
// Your POD data here
};

// use std::string rather than char[]
struct user_rec
{
 std::string fname;
 std::string lname;
 std::string telno;
};

template<typename T>
void writeVector(ofstream &out, const vector<T> &vec);

template<typename T>
vector<T> readVector(ifstream &in);

struct InventoryItem {
    string Item;
    string Description;
    int Quantity;
    int wholesaleCost;
    int retailCost;
    int dateAdded;
};

// Todo: calculate how many tiles can be view at the same time
// by dividing the the resolution with STile_size
class Cmap
{
private:
	int *width, *height, *STile_size;
	int *offset_x;
	int *offset_y;
	POINT m_pt;
	dynamic_2d_array<STile_data> tiles;
	MAPFILEHDR CMapHeader;
public:
    Cmap (int,int,int);
    ~Cmap ();
    int area () {return (*width * *height);}
	int get_width () {return (*width);}
	int get_height () {return (*height);}
	int get_STile_size () {return (*STile_size);}
	int get_offsetX() {return (*offset_x);}
	int get_offsetY() {return (*offset_y);}
	POINT get_passable() {return m_pt;}

	void passable(POINT pt)
	{
		m_pt = pt;
	}

	void offsetX(int o_v) 
	{ 
		if( *offset_x < 0 )
			*offset_x = 0;
		else
			*offset_x = o_v;
	}

	void offsetY(int o_v)
	{
		if( *offset_y > 2880 )
			*offset_y = 2880;
		else
			*offset_y = o_v;
	}

	void write(const std::string& file_name, s& data)
	{
		std::ofstream out(file_name.c_str());
		out.write(reinterpret_cast<char*>(&data), sizeof(data));
	}

	void read(const std::string file_name, s& data)
	{
		std::ifstream in(file_name.c_str());
		in.read(reinterpret_cast<char*>(&data), sizeof(data));
	}

	void create_tiles()
	{
		strcpy(CMapHeader.EngName,"EngName");
		CMapHeader.EngVersion = 1;
		strcpy(CMapHeader.MapDesc,"MapDesc");
		CMapHeader.MapHeight = 1;
		strcpy(CMapHeader.MapName,"MapName");
		CMapHeader.MapWidth = 1;
		strcpy(CMapHeader.Tileset,"Tileset");

		s bu;

		// Fill vector here
		read("test.dat", bu);
		write("test.dat", bu);

		tester();
		
		//test_user_rec();
		ifstream tile_data ( "data/worldmap.txt",ios_base::in);
		ifstream monster_data ( "data/worldmapActors.txt",ios_base::in);
		
		for(int nCol = 0; nCol < *height; nCol++)
		{
			for(int nRow = 0; nRow < *width; nRow++)
			{
				tile_data >> tiles[nCol][nRow].Layer[MapTile::BASE];
				monster_data >> tiles[nCol][nRow].Layer[MapTile::OBJECT];
			}
		}
		tile_data.close();
		monster_data.close();
	};

	bool SaveWorld()
	{
		std::ofstream MapFile("data/worldmap.txt",ios_base::out);
		std::ofstream ActorFile("data/worldmapActors.txt",ios_base::out);
		
		for(int nCol = 0; nCol < *height; nCol++)
		{
			for(int nRow = 0; nRow < *width; nRow++)
			{
				MapFile << tiles[nCol][nRow].Layer[MapTile::BASE] << " ";
				ActorFile << tiles[nCol][nRow].Layer[MapTile::OBJECT] << " ";
			}
			MapFile << "\n";
			ActorFile << "\n";
		}
		MapFile.close();
		return true;
	}

	STile_data get_tile(int nCol, int nRow)
	{
		return tiles[nCol][nRow];
	}

	void set_tile(int nCol, int nRow,double value)
	{
		if( tiles[nCol][nRow].Layer[MapTile::BASE] + value > 255 ||
			tiles[nCol][nRow].Layer[MapTile::BASE] + value < 0)
		{
			return;
		}
		else
		{
			tiles[nCol][nRow].Layer[MapTile::BASE] += (int)value;
		}
	}
};
#endif