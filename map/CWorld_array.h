#ifndef CWORLD_ARRAY_H
#define CWORLD_ARRAY_H

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

template <typename T>
class CWorld_array{
public:    CWorld_array(){};
		   CWorld_array(int rows, int cols):m_data(rows, std::vector<T>(cols)){}
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

#endif