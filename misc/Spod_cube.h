#ifndef SPOD_CUBE_H
#define SPOD_CUBE_H

#include <vector>
using namespace std;
#include "../misc/Spod_entity.h"

struct Spod_cube
{
   int x, y, z;
   bool up,down,left,right,top,bottom;
   std::vector<Spod_entity> npc_actor;
   int tile_num;
};

#endif;