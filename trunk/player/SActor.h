/**
 * @file SActor.cpp
 *
 * @brief The actor that you play.
 *
 * @author Rickard Skeppström
 *
 * @version 12.0
 *
 */

#ifndef SACTOR_H
#define SACTOR_H

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
	int MoveLeft,MoveRight,MoveUp,MoveDown,Fire,Idle;
} AInput;

struct SEquipped
{
	int MAINHAND;
	int OFFHAND;
	int UPPERBODY;
	int HEAD;
};

struct SActor : public SSoul, gs_CSoul, CItem, SEquipped, CGameObjects
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

	string m_Name;

	map<const char*, int, ltstr> Buffs;
	map<const char*, int, ltstr>::iterator cur;
	map<const char*, int, ltstr>::iterator prev;
	map<const char*, int, ltstr>::iterator next;

	map<const char*, int, ltstr>::const_iterator end;
	map<const char*, int, ltstr>::const_iterator begin;

	typedef map<const char*, int, ltstr> Typen;

	int m_Gold;
	CVector3 m_Coord;
	
	int m_MaxHP;
	int m_HP;
	int m_MaxMP;
	int m_MP;
	int m_XP;
	bool m_IsActive;
	AInput m_Input;
	int m_State;
	int m_Facing;
	int m_IsMoving;
	int m_FaceSpriteID;

	SActor();

	void Init();
	bool Save();
	bool Load();
	void ShowInventory();
	void CharSheet();
	void SetMaxStats();
	std::string ClassName(int ID);
	void Info(HDC hdc, HBRUSH color,RECT rect,HBITMAP hbmp,BITMAP bmp,HDC tile);
	void Update();
	void NameGen();
	int State();
	CVector3 TileCoords(int iOffsetX, int iOffsetY);
	float ProcentHP();
	void MoveRandom();
	void Step(){};
};

typedef std::tr1::unordered_map<std::string, SActor> PlayerTable;

#endif