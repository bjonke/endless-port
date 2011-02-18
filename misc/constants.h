#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>

using namespace std;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

const int MOVEMENT_FRAMES = 10;
const int ATTACK_VERTICAL_FRAMES = 8;
const int ATTACK_HORIZONTAL_FRAMES = 7;
const int CITY_TILE = 17;
const int TILE_SIZE = 64;
const int MAX_COMPANIONS = 4;

namespace MoveDirection
{
	enum Enum
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
	};
}

namespace EGameState
{
	enum Enum
	{
		SPLASH,
		MENU,
		MENU_OPTIONS,
		QUIT,
		RUNNING,
		CREATE_CHARACTER,
		BATTLE,
		OUTRO,
		HALL_OF_FAME,
		CHARACTER_LIST,
		COMPANION_LIST,
		SPELL_LIST,
		CITY,
		GUILD,
	};
}
namespace AreaType
{
	enum Enum
	{
		GENERAL_STORE = 1,
		BANK = 2,
		WORLD_MAP = 3,
		TAVERN = 4,
		GUILD = 5,
		SPLASH = 7,
		STREET = 8,
		MENU = 9,
		MENU_OPTIONS = 10,
		MENU_CREATE = 11,
		BATTLE = 12,
	};
}

namespace MapTileType
{
	enum Enum
	{
		PLAYER,
		BORDER2,
		SPLASH3,
		TILEMAP,
		TILEMAP2,
		TILE1,
		SPLASHEN,
		SPLASH2,
		CARDINAL,
		NETHACK,
		CITY_ICONS,
		STD_BUTTON,
		STD_EQ,
		STD_SHEET,
		STD_OPTION_ICONS,
		STD_FACE_ICONS,
		STD_WORLD_MAP,
		STD_640x480,
		STD_SPELLBOOK,
		STD_TILES,
		STD_CREATE,
		STD_HAWK,
		STD_MENU,
	};
}

// BASE = bottom layer
// FRINGE = trees and such
// OBJECT = Actors,items
namespace MapTile
{
	enum Enum
	{
		BASE,
		FRINGE,
		OBJECT,
	};
};

namespace MapWalkability
{
	enum Enum
	{
		NORMAL_WALKABLE,
		NORMAL_UNWALKABLE,
		SPECIAL_WALKABLE,
		SPECIAL_UNWALKABLE,
	};
};

namespace ActorState
{
	enum Enum
	{
		IDLE,
		MOVING,
		ATTACKING,
	};
};
namespace ActorFacing
{
	enum Enum
	{
		RIGHT = 6,
		LEFT = 10,
		DOWN = 4,
		UP = 8,
	};
};

const int ATTACK_ACTION = 0, DEFEND_ACTION = 1;


const DWORD CONDITION_PLAYER = 1;

const DWORD EVENT_POISON = 1;
const DWORD EVENT_DROWNING = 2;
const DWORD EVENT_SLOW = 4;
const DWORD EVENT_TELEPORT = 8;
const DWORD EVENT_UNSET1 = 16;
const DWORD EVENT_UNSET2 = 32;
const DWORD EVENT_UNSET3 = 64;
const DWORD EVENT_UNSET4 = 128;


const DWORD EX_1 = 1;  //00000001 in binary
const DWORD EX_2 = 2;  //00000010 in binary
const DWORD EX_3 = 4;  //00000100 in binary
const DWORD EX_4 = 8;  //00001000 in binary
const DWORD EX_5 = 16; //00010000 in binary


const DWORD TRIGGER_ENABLED = 1;  //00000001 in binary
const DWORD TRIGGER_INITIALLY_ON = 2;  //00000010 in binary
const DWORD TRIGGER_COMMENT = 4;  //00000100 in binary

const DWORD TRIGGER_INITIALLY_ENABLED = 3;

/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

#define VK_A		0x41
#define VK_B		0x42
#define VK_C		0x43
#define VK_D		0x44
#define VK_E		0x45
#define VK_F		0x46
#define VK_G		0x47
#define VK_H		0x48
#define VK_I		0x49
#define VK_J		0x50
#define VK_K		0x51
#define VK_L		0x52
#define VK_M		0x53
#define VK_N		0x54
#define VK_O		0x55
#define VK_P		0x56
#define VK_Q		0x57
#define VK_R		0x58
#define VK_S		0x59
#define VK_T		0x60
#define VK_U		0x61
#define VK_V		0x62
#define VK_W		0x63
#define VK_X		0x64
#define VK_Y		0x65
#define VK_Z		0x66
#endif