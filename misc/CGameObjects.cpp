#include<string>
#include<list>
#include<vector>
using namespace std;
#include "CGameObjects.h"

CGameObjects::CGameObjects()   // Constructor
{
}
 
CGameObjects::CGameObjects(const CGameObjects &copyin)   // Copy constructor to handle pass by value.
{
}
 
ostream &operator<<(ostream &output, const CGameObjects &aaa)
{
   //output << aaa.x << ' ' << aaa.y << ' ' << aaa.z << endl;
   return output;
}
 
CGameObjects& CGameObjects::operator=(const CGameObjects &rhs)
{
   //this->x = rhs.x;
   //this->y = rhs.y;
   //this->z = rhs.z;
   return *this;
}
 
int CGameObjects::operator==(const CGameObjects &rhs) const
{
   //if( this->x != rhs.x) return 0;
   //if( this->y != rhs.y) return 0;
   //if( this->z != rhs.z) return 0;
   return 1;
}
 
// This function is required for built-in STL list functions like sort
int CGameObjects::operator<(const CGameObjects &rhs) const
{
   //if( this->x == rhs.x && this->y == rhs.y && this->z < rhs.z) return 1;
   //if( this->x == rhs.x && this->y < rhs.y) return 1;
   //if( this->x < rhs.x ) return 1;
   return 0;
}