#ifndef H_POD_PLAYER
#define H_POD_PLAYER

#include <list>
#include <map>
#include <iostream>
#include <string>
#include <unordered_map> // Stores hash table of {key, mapped} pairs.
#include <map>
#include <functional>
#include <fstream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>
#include <deque>
#include <hash_map>
#include <string>
#include <fstream>

#include <stdlib.h>
using namespace std;

#include "../misc/CGameObjects.h"
#include "../misc/CItem.h"
#include "../player/SSoul.h"
#include "../player/gs_CSoul.h"
#include "../graphics/CVector.h"

#define N 10

typedef struct
{
	int move_left,move_right,move_up,move_down,fire,idle;
} AInput;

struct SEquipped
{
	int MAINHAND;
	int OFFHAND;
	int UPPERBODY;
	int HEAD;
};


struct pod_player : public SSoul, gs_CSoul, CItem, SEquipped, CGameObjects
{
	vector<CGameObjects*> VInventory;
	typedef std::tr1::unordered_map<const char * , int> hash;

	struct ltstr
	{
	  bool operator()(const char* s1, const char* s2) const
	  {
		return strcmp(s1, s2) < 0;
	  }
	};

	std::vector<int> m_Inventory;
	bool m_ShowInventory;
	bool m_ShowCharSheet;
	std::vector<int> m_Equipped;
	string m_name;
	map<const char*, int, ltstr> Buffs;
	map<const char*, int, ltstr>::iterator cur;
	map<const char*, int, ltstr>::iterator prev;
	map<const char*, int, ltstr>::iterator next;

	map<const char*, int, ltstr>::const_iterator end;
	map<const char*, int, ltstr>::const_iterator begin;

	typedef map<const char*, int, ltstr> Typen;

	int m_gold;
	CVector3 coord;
	
	int m_max_hp;
	int m_hp;
	int m_max_mp;
	int m_mp;
	int m_xp;
	bool m_isactive;
	AInput i;
	int State;
	int Facing;
	int isMoving;
	int FaceSpriteID;

	pod_player();

	void Init();
	bool Save();
	bool Load();
	void ShowInventory();
	void CharSheet();
	void SetMaxStats();
	std::string ClassName(int ID);
	void info(HDC hdc, HBRUSH color,RECT rect);
	void update();
	void NameGen();
	int state();
	CVector3 TileCoords(int iOffsetX, int iOffsetY);
	float ProcentHP();
	void MoveRandom();
	void Step(){};
};

typedef std::tr1::unordered_map<std::string, pod_player> PlayerTable;



#endif
