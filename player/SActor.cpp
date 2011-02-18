/**
 * @file SActor.cpp
 *
 * @brief Brief description goes here.
 *
 * @author Rickard Skeppström
 *
 * @version 1.0
 */

#include<sstream>
using namespace std;
#include "SActor.h"
#include "../misc/constants.h"
SActor::SActor()
{
	m_IsMoving = -1;
	m_Name = "name";
	m_ShowInventory = false;
	m_ShowCharSheet = false;
	m_Gold = rand()%1000;
	m_MP = 10;
	m_HP = 0;
	m_MaxMP = 10;
	m_MaxHP = 0;
	m_Coord.zero();
	m_Coord.x = 1;
	m_Coord.y = 1;
	m_State = ActorState::IDLE;
	m_Facing = ActorFacing::DOWN;
	SetMaxStats();
	Init();
	m_FaceSpriteID = 0;
}

void SActor::Init()
{
	m_HP = m_MaxHP;
	m_MP = m_MaxMP;
	m_XP = 0;
	m_State = ActorState::IDLE;
	NameGen();
}

void SActor::Update()
{
	if( m_HP > m_MaxHP )
	{
		m_HP = m_MaxHP;
	}
	SSoul::m_Attack = SSoul::m_Strength + SSoul::GMods.m_Strength;
 	SSoul::m_Defence = SSoul::m_Dexterity + SSoul::GMods.m_Dexterity;
	IMods.Reset();

	int tmp = 0;
	this->IMods.Reset();
	for( std::vector<int>::const_iterator citer = this->m_Inventory.begin();
		citer != this->m_Inventory.end(); ++citer)
	{
		if( this->m_Equipped.at(tmp) == 1 )
		{
			//this->IMods.ImproveAttack(atoi(Items.m_Vector.at((*citer)).at(4).c_str()));
		}
		if( this->m_Equipped.at(tmp) == 1  )
		{
			//this->IMods.ImproveDefence(atoi(Items.m_Vector.at((*citer)).at(4).c_str()));
		}
		tmp++;
	}
}

void SActor::ShowInventory()
{
	m_ShowInventory = !m_ShowInventory;
	Sleep(100);
}

void SActor::CharSheet()
{
	m_ShowCharSheet = !m_ShowCharSheet;
	Sleep(100);
}

void SActor::SetMaxStats()
{
	m_MaxHP = (SSoul::m_Strength + SSoul::m_Constitution) / 2;
	m_MaxMP = SSoul::m_Wisdom + SSoul::m_Intelligence;
}

void SActor::Info(HDC hdc, HBRUSH color,RECT rect,HBITMAP hbmp, BITMAP bmp, HDC tile)
{
	// 80 height 96 width
	const int FaceSpriteRows = 25;
	const int FaceSpriteColumns = 8;
	const int FaceSpriteHeight = bmp.bmHeight / FaceSpriteRows;
	const int FaceSpriteWidth = bmp.bmWidth / FaceSpriteColumns;
	static int face = 1;
	RECT SActor_face = { 400,0,464,64};

	FillRect(hdc,&rect,color);
	SelectObject(tile,hbmp);
				TransparentBlt(hdc, 
				400, 
				0, 
				FaceSpriteWidth, 
				FaceSpriteHeight, 
				tile, 
				0, 
				FaceSpriteHeight*this->m_FaceSpriteID,
				FaceSpriteWidth,
				FaceSpriteHeight,
				RGB(255,0,255));

	RECT char_info = {rect.left+32,rect.top+64,rect.right,rect.bottom};

	TextOut(hdc,char_info.left,char_info.top+16,"NAME:",strlen("NAME:"));
	TextOut(hdc,char_info.left,char_info.top+32,"CLASS:",strlen("CLASS:"));
	TextOut(hdc,char_info.left,char_info.top+48,"SEX:",strlen("SEX:"));
	TextOut(hdc,char_info.left,char_info.top+64,"ALIGNMENT:",strlen("ALIGNMENT:"));
	TextOut(hdc,char_info.left,char_info.top+80,"GUILD:",strlen("GUILD:"));
	TextOut(hdc,char_info.left,char_info.top+96,"SIZE:",strlen("SIZE:"));

	TextOut(hdc,char_info.left,char_info.top+128,"ATTACK:",strlen("ATTACK:"));
	TextOut(hdc,char_info.left,char_info.top+144,"DEFENCE:",strlen("DEFENCE:"));
	
	TextOut(hdc,char_info.left,char_info.top+112,"LEVEL:",strlen("LEVEL:"));
	TextOut(hdc,char_info.left,char_info.top+192,"GOLD:",strlen("GOLD:"));

	std::stringstream stream;
	stream << SSoul::m_Guild;
	TextOut(hdc,char_info.left + 128,char_info.top+80,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Attack;
	TextOut(hdc,char_info.left + 128,char_info.top+128,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Defence;
	TextOut(hdc,char_info.left + 128,char_info.top+144,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::IMods.m_Attack;
	TextOut(hdc,char_info.left + 160,char_info.top+128,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::IMods.m_Defence;
	TextOut(hdc,char_info.left + 160,char_info.top+144,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << m_Gold;
	TextOut(hdc,char_info.left + 128,char_info.top+192,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::getCharacterLevel(m_XP);
	TextOut(hdc,char_info.left + 128,char_info.top+112,stream.str().c_str(), strlen(stream.str().c_str()));
	
	TextOut(hdc,char_info.left,char_info.top+248,"HP:",strlen("HP:"));
	stream << m_HP;
	TextOut(hdc,char_info.left + 64,char_info.top+248,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << m_MaxHP;
	TextOut(hdc,char_info.left + 96,char_info.top+248,stream.str().c_str(), strlen(stream.str().c_str()));

	TextOut(hdc,char_info.left + 128,char_info.top+248,"MP:",strlen("MP:"));
	stream << m_MP;
	TextOut(hdc,char_info.left + 160,char_info.top+248,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << m_MaxMP;
	TextOut(hdc,char_info.left + 192,char_info.top+248,stream.str().c_str(), strlen(stream.str().c_str()));

	TextOut(hdc,char_info.left + 256,char_info.top+248,"XP:",strlen("XP:"));
	stream << m_XP;
	TextOut(hdc,char_info.left + 320,char_info.top+248,stream.str().c_str(), strlen(stream.str().c_str()));

	TextOut(hdc,char_info.left,char_info.top+272,"STRENGTH:",strlen("STRENGTH:"));
	TextOut(hdc,char_info.left,char_info.top+288,"INTELLIGENCE:",strlen("INTELLIGENCE:"));
	TextOut(hdc,char_info.left,char_info.top+304,"WISDOM:",strlen("WISDOM:"));
	TextOut(hdc,char_info.left,char_info.top+320,"DEXTERITY:",strlen("DEXTERITY:"));
	TextOut(hdc,char_info.left,char_info.top+336,"CONSTITUTION:",strlen("CONSTITUTION:"));
	TextOut(hdc,char_info.left,char_info.top+352,"CHARISMA:",strlen("CHARISMA:"));
	stream << SSoul::m_Strength;
	TextOut(hdc,char_info.left + 128,char_info.top+272,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Intelligence;
	TextOut(hdc,char_info.left + 128,char_info.top+288,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Wisdom;
	TextOut(hdc,char_info.left + 128,char_info.top+304,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Dexterity;
	TextOut(hdc,char_info.left + 128,char_info.top+320,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Constitution;
	TextOut(hdc,char_info.left + 128,char_info.top+336,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::m_Charisma;
	TextOut(hdc,char_info.left + 128,char_info.top+352,stream.str().c_str(), strlen(stream.str().c_str()));

	stream << SSoul::GMods.m_Strength;
	TextOut(hdc,char_info.left + 160,char_info.top+272,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::GMods.m_Intelligence;
	TextOut(hdc,char_info.left + 160,char_info.top+288,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::GMods.m_Wisdom;
	TextOut(hdc,char_info.left + 160,char_info.top+304,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::GMods.m_Dexterity;
	TextOut(hdc,char_info.left + 160,char_info.top+320,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::GMods.m_Constitution;
	TextOut(hdc,char_info.left + 160,char_info.top+336,stream.str().c_str(), strlen(stream.str().c_str()));
	stream << SSoul::GMods.m_Charisma;
	TextOut(hdc,char_info.left + 160,char_info.top+352,stream.str().c_str(), strlen(stream.str().c_str()));

	TextOut(hdc,char_info.left + 128,char_info.top+16,m_Name.c_str(), strlen(m_Name.c_str()));

	TextOut(hdc,char_info.left + 128,char_info.top+32,ClassName(m_ClassID).c_str(), strlen(ClassName(m_ClassID).c_str()));
}

void SActor::NameGen()
{
	typedef std::tr1::unordered_map<std::string, int> AgeTable;
	AgeTable ages;  
	AgeTable::hasher hfunc = ages.hash_function();
	int hash = hfunc("jjjj");
	std::cout << "hash(1)   = " << hfunc("jj")   << std::endl;
	AgeTable::key_equal eqfunc = ages.key_eq();
	int test = eqfunc("jjjj", "jjjj");
	std::cout << "key_eq(1, 1) = "  << std::boolalpha
      << eqfunc("kk", "jjjj") << std::endl;
// http://www.codeguru.com/cpp/cpp/cpp_mfc/stl/article.php/c15303__2/
	//ages["One"].name = "Hello World";
	//ages["One"].value = 11;
	ages["kk"] = 111;
	ages["kkk"] = 222;
	ages["jjjj"] = 333;
	ages["ööää"] = 111;

	//ages.insert( std::make_pair( "Joe", 25 ) ); 
	//ages.insert( std::make_pair( "Sally", 18 ) );  
	//ages.insert( std::make_pair( "Billy", 11 ) );  
	AgeTable::iterator iter;
	AgeTable::hasher;
	for ( iter = ages.begin(); iter != ages.end(); iter++ ) {
		std::cout << iter->first << " is " << iter->second << std::endl;
		//std::cout << iter->first << " is " << iter->second.name << std::endl;  
	}


	typedef map<string, int> mapType;
	mapType data;

	// let's declare some initial values to this map
	data["BobsScore"] = 10;
	data["MartysScore"] = 15;
	data["MehmetsScore"] = 34;
	data["RockysScore"] = 22;
	data["RockysScore"] = 23;  /*overwrites the 22 as keys are unique */
	data.insert(make_pair("crap", 5));

	// Iterate over the map and print out all key/value pairs.
	// Using a const_iterator since we are not going to change the values.
	for(mapType::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		cout << "Who(key = first): " << it->first;
		cout << " Score(value = second): " << it->second << '\n';
	}

	m_Name = "";
	std::string alfabeth;
	alfabeth = "abcdefghijklmnopqrstuvxyzåäö";
	int counter = 0;
	while(counter < 10)
	{		
		m_Name.append(alfabeth.substr(rand()%5,rand()%5));
		counter++;	
	}
}

std::string SActor::ClassName(int ID)
{
	switch(ID)
	{
	case 0: return "Fighter";
	case 1: return "Mage";
	case 2: return "Cleric";
	case 3: return "Thief";
	default: return "Error unknown Class!";
	}
}

bool SActor::Save()
{
	string savegame_name;
	savegame_name = "data/gfx/actors/";
	savegame_name += m_Name.c_str();
	savegame_name += ".sav";
	ofstream savegame_file ( savegame_name.c_str(),ios_base::out);
	savegame_file.close();
	ofstream SActor_data_out ( "data/gfx/logs/SActor_savefile.txt",ios_base::out);
	if( SActor_data_out.is_open() )
	{
		//Parse the savefile
		SActor_data_out << "# begin" << endl;
		SActor_data_out << "# m_name" << endl;
		SActor_data_out << m_Name << endl;
		SActor_data_out << "# m_ShowInventory" << endl;
		SActor_data_out << m_ShowInventory << endl;
		SActor_data_out << "# m_gold" << endl;
		SActor_data_out << m_Gold << endl;
		SActor_data_out << "# m_mp" << endl;
		SActor_data_out << m_MP << endl;
		SActor_data_out << "# m_HP" << endl;
		SActor_data_out << m_HP << endl;
		SActor_data_out << "# m_max_mp" << endl;
		SActor_data_out << m_MaxMP << endl;
		SActor_data_out << "# m_max_hp" << endl;
		SActor_data_out << m_MaxHP << endl;
		SActor_data_out << "# m_xp" << endl;
		SActor_data_out << m_XP << endl;

		std::vector<int>::iterator IInventory;
		std::vector<int>::iterator IEquipped;

		SActor_data_out << "# m_Inventory.size()" << endl;
		SActor_data_out << m_Inventory.size() << endl;

		for (IInventory=m_Inventory.begin(); IInventory!=m_Inventory.end(); ++IInventory)
		{
			size_t index = IInventory - m_Inventory.begin();
			SActor_data_out << "# IInventory" << index << endl;
			SActor_data_out << *IInventory << endl;
		}

		SActor_data_out << "# m_Inventory.size()" << endl;
		SActor_data_out << m_Inventory.size() << endl;

		for (IEquipped=m_Equipped.begin(); IEquipped!=m_Equipped.end(); ++IEquipped)
		{
			size_t index = IEquipped - m_Equipped.begin();
			SActor_data_out << "# IEquipped" << index << endl;
			SActor_data_out << *IEquipped << endl;
		}

		SActor_data_out << "# m_Attack" << endl;
		SActor_data_out << SSoul::m_Attack << endl;
		SActor_data_out << "# m_Charisma" << endl;
		SActor_data_out << SSoul::m_Charisma << endl;
		SActor_data_out << "# m_ClassID" << endl;
		SActor_data_out << SSoul::m_ClassID << endl;
		SActor_data_out << "# m_Constitution" << endl;
		SActor_data_out << SSoul::m_Constitution << endl;
		SActor_data_out << "# m_Defence" << endl;
		SActor_data_out << SSoul::m_Defence << endl;
		SActor_data_out << "# m_Dexterity" << endl;
		SActor_data_out << SSoul::m_Dexterity << endl;
		SActor_data_out << "# m_Guild" << endl;
		SActor_data_out << SSoul::m_Guild << endl;
		SActor_data_out << "# m_Intelligence" << endl;
		SActor_data_out << SSoul::m_Intelligence << endl;
		SActor_data_out << "# m_Sprite" << endl;
		SActor_data_out << SSoul::m_Sprite << endl;
		SActor_data_out << "# m_StatPoints" << endl;
		SActor_data_out << SSoul::m_StatPoints << endl;
		SActor_data_out << "# m_Strength" << endl;
		SActor_data_out << SSoul::m_Strength << endl;
		SActor_data_out << "# m_Wisdom" << endl;
		SActor_data_out << SSoul::m_Wisdom << endl;
		SActor_data_out << "# m_Charisma" << endl;
		SActor_data_out << SSoul::GMods.m_Charisma << endl;
		SActor_data_out << "# gmod m_Constitution" << endl;
		SActor_data_out << SSoul::GMods.m_Constitution << endl;
		SActor_data_out << "# gmod m_Dexterity" << endl;
		SActor_data_out << SSoul::GMods.m_Dexterity << endl;
		SActor_data_out << "# gmod m_Intelligence" << endl;
		SActor_data_out << SSoul::GMods.m_Intelligence << endl;
		SActor_data_out << "# gmod m_Strength" << endl;
		SActor_data_out << SSoul::GMods.m_Strength << endl;
		SActor_data_out << "# gmod m_Wisdom" << endl;
		SActor_data_out << SSoul::GMods.m_Wisdom << endl;
		SActor_data_out << "# imod m_Attack" << endl;
		SActor_data_out << SSoul::IMods.m_Attack << endl;
		SActor_data_out << "# imod m_Defence" << endl;
		SActor_data_out << SSoul::IMods.m_Defence << endl;

		SActor_data_out << "# end" << endl;
	}
	SActor_data_out.close();

	//ifstream actor_data ( "data/gfx/ActorTest.txt",ios_base::in);
	ofstream actor_data_out ( "data/gfx/logs/Actor.txt",ios_base::out);

	actor_data_out << m_Name << endl;
	actor_data_out << m_ShowInventory << endl;
	actor_data_out << m_Gold << endl;
	actor_data_out << m_MP << endl;
	actor_data_out << m_HP << endl;
	actor_data_out << m_MaxMP << endl;
	actor_data_out << m_MaxHP << endl;
	actor_data_out << m_XP << endl;

	std::vector<int>::iterator IInventory;
	std::vector<int>::iterator IEquipped;

	actor_data_out << m_Inventory.size() << endl;

	for (IInventory=m_Inventory.begin(); IInventory!=m_Inventory.end(); ++IInventory)
	{
		actor_data_out << *IInventory << endl;
	}

	actor_data_out << m_Inventory.size() << endl;

	for (IEquipped=m_Equipped.begin(); IEquipped!=m_Equipped.end(); ++IEquipped)
	{
		actor_data_out << *IEquipped << endl;
	}

	actor_data_out << SSoul::m_Attack << endl;
	actor_data_out << SSoul::m_Charisma << endl;
	actor_data_out << SSoul::m_ClassID << endl;
	actor_data_out << SSoul::m_Constitution << endl;
	actor_data_out << SSoul::m_Defence << endl;
	actor_data_out << SSoul::m_Dexterity << endl;
	actor_data_out << SSoul::m_Guild << endl;
	actor_data_out << SSoul::m_Intelligence << endl;
	actor_data_out << SSoul::m_Sprite << endl;
	actor_data_out << SSoul::m_StatPoints << endl;
	actor_data_out << SSoul::m_Strength << endl;
	actor_data_out << SSoul::m_Wisdom << endl;
	actor_data_out << SSoul::GMods.m_Charisma << endl;
	actor_data_out << SSoul::GMods.m_Constitution << endl;
	actor_data_out << SSoul::GMods.m_Dexterity << endl;
	actor_data_out << SSoul::GMods.m_Intelligence << endl;
	actor_data_out << SSoul::GMods.m_Strength << endl;
	actor_data_out << SSoul::GMods.m_Wisdom << endl;
	actor_data_out << SSoul::IMods.m_Attack << endl;
	actor_data_out << SSoul::IMods.m_Defence << endl;

	actor_data_out.close();
	return false;
}

bool SActor::Load()
{
	ifstream actor_data_in ( "data/gfx/logs/Actor.txt",ios_base::in);

	ifstream in ( "data/gfx/logs/SActor_savefile.txt" );
	ofstream out ( "data/gfx/logs/SActor_savefile_out.txt" );

	if ( in.is_open() ) {
		string line;
		string data = "";
		while ( getline ( in, line ) ) {
			string::size_type i = line.find_first_not_of ( " \t\n\v" );
			if ( i != string::npos && line[i] == '#' )
			{
				size_t found;
				if ( found = line.find("m_name") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_Name;
				}
				else if ( found = line.find("m_ShowInventory") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_ShowInventory;
				}
				else if ( found = line.find("m_gold") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_Gold;
				}
				else if ( found = line.find("m_mp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_MP;
				}
				else if ( found = line.find("m_HP") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_HP;
				}
				else if ( found = line.find("m_max_mp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_MaxMP;
				}
				else if ( found = line.find("m_max_hp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_MaxHP;
				}
				else if ( found = line.find("m_xp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_XP;
				}
				else if ( found = line.find("m_Inventory.size()") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("IInventory") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("IEquipped") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Attack") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Charisma") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_ClassID") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Constitution") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Defence") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Dexterity") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Guild") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Intelligence") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Sprite") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_StatPoints") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("m_Strength") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_Charisma") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_onstitution") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_Dexterity") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_Intelligence") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_Strength") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("gmod m_Wisdom") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("imod m_Attack") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				else if ( found = line.find("imod m_Defence") != string::npos )
				{
					data.assign(line, found, line.size());
				}
				continue;
			}
			else
			{
					out << data << " - "<< line << endl;
			}
		}
	}
	out.close();
	
	int val,InventorySize,EquippedSize =0;
	actor_data_in >> InventorySize;
	for (int IInventory=0; IInventory!=InventorySize; ++IInventory)
	{
		actor_data_in >> val;
		m_Inventory.push_back(val);
	}

	actor_data_in >> EquippedSize;
	for (int IEquipped=0; IEquipped!=EquippedSize; ++IEquipped)
	{
		actor_data_in >> val;
		m_Equipped.push_back(val);
	}

	actor_data_in >> SSoul::m_Attack;
	actor_data_in >> SSoul::m_Charisma;
	actor_data_in >> SSoul::m_ClassID;
	actor_data_in >> SSoul::m_Constitution;
	actor_data_in >> SSoul::m_Defence;
	actor_data_in >> SSoul::m_Dexterity;
	actor_data_in >> SSoul::m_Guild;
	actor_data_in >> SSoul::m_Intelligence;
	actor_data_in >> SSoul::m_Sprite;
	actor_data_in >> SSoul::m_StatPoints;
	actor_data_in >> SSoul::m_Strength;
	actor_data_in >> SSoul::m_Wisdom;
	actor_data_in >> SSoul::GMods.m_Charisma;
	actor_data_in >> SSoul::GMods.m_Constitution;
	actor_data_in >> SSoul::GMods.m_Dexterity;
	actor_data_in >> SSoul::GMods.m_Intelligence;
	actor_data_in >> SSoul::GMods.m_Strength;
	actor_data_in >> SSoul::GMods.m_Wisdom;
	actor_data_in >> SSoul::IMods.m_Attack;
	actor_data_in >> SSoul::IMods.m_Defence;

	actor_data_in.close();
	return false;
}

int SActor::State()
{
	return m_State;
}

CVector3 SActor::TileCoords(int iOffsetX, int iOffsetY)
{
	CVector3 tmpVector;
	tmpVector.x = (this->m_Coord.x*64)-iOffsetX*64;
	tmpVector.y = (this->m_Coord.y*64)-iOffsetY*64;
	return tmpVector;
}

float SActor::ProcentHP()
{
	return 100.0f / (float)this->m_MaxHP * (float)this->m_HP;
}

void SActor::MoveRandom()
{
	if( m_Coord.x > 0 && m_Coord.x < 49)
	m_Coord.x += rand() % 3 -1;
	if( m_Coord.y > 0 && m_Coord.y < 49)
	m_Coord.y += rand() % 3  -1;
}