//-------------------------------------------------------------------------
// Namn: Rickard Skeppström
// E-mail: rickard_skeppstrom@msn.com
//-------------------------------------------------------------------------

#include "pod_player.h"
#include "../misc/constants.h"
pod_player::pod_player()
{
	isMoving = -1;
	m_name = "name";
	m_ShowInventory = false;
	m_ShowCharSheet = false;
	m_gold = rand()%1000;
	m_mp = 10;
	m_hp = 0;
	m_max_mp = 10;
	m_max_hp = 0;
	coord.zero();
	coord.x = 1;
	coord.y = 1;
	State = ActorState::IDLE;
	Facing = ActorFacing::DOWN;
	SetMaxStats();
	Init();
	FaceSpriteID = 0;
}

void pod_player::Init()
{
	m_hp = m_max_hp;
	m_mp = m_max_mp;
	m_xp = 0;
	State = ActorState::IDLE;
	NameGen();
	//if(rand()%100 > 78)
	//{
	//	m_isactive = true;
	//}
}

void pod_player::update()
{
	if( m_hp > m_max_hp )
	{
		m_hp = m_max_hp;
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

void pod_player::ShowInventory()
{
	m_ShowInventory = !m_ShowInventory;
	Sleep(100);
}

void pod_player::CharSheet()
{
	m_ShowCharSheet = !m_ShowCharSheet;
	Sleep(100);
}

void pod_player::SetMaxStats()
{
	m_max_hp = (SSoul::m_Strength + SSoul::m_Constitution) / 2;
	m_max_mp = SSoul::m_Wisdom + SSoul::m_Intelligence;
}

void pod_player::info(HDC hdc, HBRUSH color,RECT rect)
{
	FillRect(hdc,&rect,color);
	char buffer[345];
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
	TextOut(hdc,char_info.left + 128,char_info.top+80,_itoa(SSoul::m_Guild,buffer,10), strlen(_itoa(SSoul::m_Guild,buffer,10)));

	TextOut(hdc,char_info.left + 128,char_info.top+128,_itoa(SSoul::m_Attack,buffer,10), strlen(_itoa(SSoul::m_Attack,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+144,_itoa(SSoul::m_Defence,buffer,10), strlen(_itoa(SSoul::m_Defence,buffer,10)));

	TextOut(hdc,char_info.left + 160,char_info.top+128,_itoa(SSoul::IMods.m_Attack,buffer,10), strlen(_itoa(SSoul::IMods.m_Attack,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+144,_itoa(SSoul::IMods.m_Defence,buffer,10), strlen(_itoa(SSoul::IMods.m_Defence,buffer,10)));


	TextOut(hdc,char_info.left + 128,char_info.top+192,_itoa(m_gold,buffer,10), strlen(_itoa(m_gold,buffer,10)));

	TextOut(hdc,char_info.left + 128,char_info.top+112,_itoa(SSoul::getCharacterLevel(m_xp),buffer,10), strlen(_itoa(SSoul::getCharacterLevel(m_xp),buffer,10)));
	
	TextOut(hdc,char_info.left,char_info.top+248,"HP:",strlen("HP:"));
	TextOut(hdc,char_info.left + 64,char_info.top+248,_itoa((int)m_hp,buffer,10), strlen(_itoa((int)m_hp,buffer,10)));
	TextOut(hdc,char_info.left + 96,char_info.top+248,_itoa((int)m_max_hp,buffer,10), strlen(_itoa((int)m_max_hp,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+248,"MP:",strlen("MP:"));
	TextOut(hdc,char_info.left + 160,char_info.top+248,_itoa((int)m_mp,buffer,10), strlen(_itoa(m_mp,buffer,10)));
	TextOut(hdc,char_info.left + 192,char_info.top+248,_itoa((int)m_max_mp,buffer,10), strlen(_itoa(m_max_mp,buffer,10)));
	TextOut(hdc,char_info.left + 256,char_info.top+248,"XP:",strlen("XP:"));
	TextOut(hdc,char_info.left + 320,char_info.top+248,_itoa((int)m_xp,buffer,10), strlen(_itoa(m_xp,buffer,10)));

	TextOut(hdc,char_info.left,char_info.top+272,"STRENGTH:",strlen("STRENGTH:"));
	TextOut(hdc,char_info.left,char_info.top+288,"INTELLIGENCE:",strlen("INTELLIGENCE:"));
	TextOut(hdc,char_info.left,char_info.top+304,"WISDOM:",strlen("WISDOM:"));
	TextOut(hdc,char_info.left,char_info.top+320,"DEXTERITY:",strlen("DEXTERITY:"));
	TextOut(hdc,char_info.left,char_info.top+336,"CONSTITUTION:",strlen("CONSTITUTION:"));
	TextOut(hdc,char_info.left,char_info.top+352,"CHARISMA:",strlen("CHARISMA:"));
	TextOut(hdc,char_info.left + 128,char_info.top+272,_itoa(SSoul::m_Strength,buffer,10), strlen(_itoa(SSoul::m_Strength,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+288,_itoa(SSoul::m_Intelligence,buffer,10), strlen(_itoa(SSoul::m_Intelligence,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+304,_itoa(SSoul::m_Wisdom,buffer,10), strlen(_itoa(SSoul::m_Wisdom,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+320,_itoa(SSoul::m_Dexterity,buffer,10), strlen(_itoa(SSoul::m_Dexterity,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+336,_itoa(SSoul::m_Constitution,buffer,10), strlen(_itoa(SSoul::m_Constitution,buffer,10)));
	TextOut(hdc,char_info.left + 128,char_info.top+352,_itoa(SSoul::m_Charisma,buffer,10), strlen(_itoa(SSoul::m_Charisma,buffer,10)));

	TextOut(hdc,char_info.left + 160,char_info.top+272,_itoa(SSoul::GMods.m_Strength,buffer,10), strlen(_itoa(SSoul::GMods.m_Strength,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+288,_itoa(SSoul::GMods.m_Intelligence,buffer,10), strlen(_itoa(SSoul::GMods.m_Intelligence,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+304,_itoa(SSoul::GMods.m_Wisdom,buffer,10), strlen(_itoa(SSoul::GMods.m_Wisdom,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+320,_itoa(SSoul::GMods.m_Dexterity,buffer,10), strlen(_itoa(SSoul::GMods.m_Dexterity,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+336,_itoa(SSoul::GMods.m_Constitution,buffer,10), strlen(_itoa(SSoul::GMods.m_Constitution,buffer,10)));
	TextOut(hdc,char_info.left + 160,char_info.top+352,_itoa(SSoul::GMods.m_Charisma,buffer,10), strlen(_itoa(SSoul::GMods.m_Charisma,buffer,10)));

	TextOut(hdc,char_info.left + 128,char_info.top+16,m_name.c_str(), strlen(m_name.c_str()));

	TextOut(hdc,char_info.left + 128,char_info.top+32,ClassName(m_ClassID).c_str(), strlen(ClassName(m_ClassID).c_str()));
}

void pod_player::NameGen()
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

	m_name = "";
	std::string alfabeth;
	alfabeth = "abcdefghijklmnopqrstuvxyzåäö";
	int counter = 0;
	while(counter < 10)
	{		
		m_name.append(alfabeth.substr(rand()%5,rand()%5));
		counter++;	
	}
}

std::string pod_player::ClassName(int ID)
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

bool pod_player::Save()
{
	string savegame_name;
	savegame_name = "data/actors/";
	savegame_name += m_name.c_str();
	savegame_name += ".sav";
	ofstream savegame_file ( savegame_name.c_str(),ios_base::out);
	savegame_file.close();
	ofstream pod_player_data_out ( "data/logs/pod_player_savefile.txt",ios_base::out);
	if( pod_player_data_out.is_open() )
	{
		//Parse the savefile
		pod_player_data_out << "# begin" << endl;
		pod_player_data_out << "# m_name" << endl;
		pod_player_data_out << m_name << endl;
		pod_player_data_out << "# m_ShowInventory" << endl;
		pod_player_data_out << m_ShowInventory << endl;
		pod_player_data_out << "# m_gold" << endl;
		pod_player_data_out << m_gold << endl;
		pod_player_data_out << "# m_mp" << endl;
		pod_player_data_out << m_mp << endl;
		pod_player_data_out << "# m_hp" << endl;
		pod_player_data_out << m_hp << endl;
		pod_player_data_out << "# m_max_mp" << endl;
		pod_player_data_out << m_max_mp << endl;
		pod_player_data_out << "# m_max_hp" << endl;
		pod_player_data_out << m_max_hp << endl;
		pod_player_data_out << "# m_xp" << endl;
		pod_player_data_out << m_xp << endl;

		std::vector<int>::iterator IInventory;
		std::vector<int>::iterator IEquipped;

		pod_player_data_out << "# m_Inventory.size()" << endl;
		pod_player_data_out << m_Inventory.size() << endl;

		for (IInventory=m_Inventory.begin(); IInventory!=m_Inventory.end(); ++IInventory)
		{
			size_t index = IInventory - m_Inventory.begin();
			pod_player_data_out << "# IInventory" << index << endl;
			pod_player_data_out << *IInventory << endl;
		}

		pod_player_data_out << "# m_Inventory.size()" << endl;
		pod_player_data_out << m_Inventory.size() << endl;

		for (IEquipped=m_Equipped.begin(); IEquipped!=m_Equipped.end(); ++IEquipped)
		{
			size_t index = IEquipped - m_Equipped.begin();
			pod_player_data_out << "# IEquipped" << index << endl;
			pod_player_data_out << *IEquipped << endl;
		}

		pod_player_data_out << "# m_Attack" << endl;
		pod_player_data_out << SSoul::m_Attack << endl;
		pod_player_data_out << "# m_Charisma" << endl;
		pod_player_data_out << SSoul::m_Charisma << endl;
		pod_player_data_out << "# m_ClassID" << endl;
		pod_player_data_out << SSoul::m_ClassID << endl;
		pod_player_data_out << "# m_Constitution" << endl;
		pod_player_data_out << SSoul::m_Constitution << endl;
		pod_player_data_out << "# m_Defence" << endl;
		pod_player_data_out << SSoul::m_Defence << endl;
		pod_player_data_out << "# m_Dexterity" << endl;
		pod_player_data_out << SSoul::m_Dexterity << endl;
		pod_player_data_out << "# m_Guild" << endl;
		pod_player_data_out << SSoul::m_Guild << endl;
		pod_player_data_out << "# m_Intelligence" << endl;
		pod_player_data_out << SSoul::m_Intelligence << endl;
		pod_player_data_out << "# m_Sprite" << endl;
		pod_player_data_out << SSoul::m_Sprite << endl;
		pod_player_data_out << "# m_StatPoints" << endl;
		pod_player_data_out << SSoul::m_StatPoints << endl;
		pod_player_data_out << "# m_Strength" << endl;
		pod_player_data_out << SSoul::m_Strength << endl;
		pod_player_data_out << "# m_Wisdom" << endl;
		pod_player_data_out << SSoul::m_Wisdom << endl;
		pod_player_data_out << "# m_Charisma" << endl;
		pod_player_data_out << SSoul::GMods.m_Charisma << endl;
		pod_player_data_out << "# gmod m_Constitution" << endl;
		pod_player_data_out << SSoul::GMods.m_Constitution << endl;
		pod_player_data_out << "# gmod m_Dexterity" << endl;
		pod_player_data_out << SSoul::GMods.m_Dexterity << endl;
		pod_player_data_out << "# gmod m_Intelligence" << endl;
		pod_player_data_out << SSoul::GMods.m_Intelligence << endl;
		pod_player_data_out << "# gmod m_Strength" << endl;
		pod_player_data_out << SSoul::GMods.m_Strength << endl;
		pod_player_data_out << "# gmod m_Wisdom" << endl;
		pod_player_data_out << SSoul::GMods.m_Wisdom << endl;
		pod_player_data_out << "# imod m_Attack" << endl;
		pod_player_data_out << SSoul::IMods.m_Attack << endl;
		pod_player_data_out << "# imod m_Defence" << endl;
		pod_player_data_out << SSoul::IMods.m_Defence << endl;

		pod_player_data_out << "# end" << endl;
	}
	pod_player_data_out.close();

	//ifstream actor_data ( "data/ActorTest.txt",ios_base::in);
	ofstream actor_data_out ( "data/logs/Actor.txt",ios_base::out);

	actor_data_out << m_name << endl;
	actor_data_out << m_ShowInventory << endl;
	actor_data_out << m_gold << endl;
	actor_data_out << m_mp << endl;
	actor_data_out << m_hp << endl;
	actor_data_out << m_max_mp << endl;
	actor_data_out << m_max_hp << endl;
	actor_data_out << m_xp << endl;

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

bool pod_player::Load()
{
	ifstream actor_data_in ( "data/logs/Actor.txt",ios_base::in);

	ifstream in ( "data/logs/pod_player_savefile.txt" );
	ofstream out ( "data/logs/pod_player_savefile_out.txt" );

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
					actor_data_in >> m_name;
				}
				else if ( found = line.find("m_ShowInventory") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_ShowInventory;
				}
				else if ( found = line.find("m_gold") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_gold;
				}
				else if ( found = line.find("m_mp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_mp;
				}
				else if ( found = line.find("m_hp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_hp;
				}
				else if ( found = line.find("m_max_mp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_max_mp;
				}
				else if ( found = line.find("m_max_hp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_max_hp;
				}
				else if ( found = line.find("m_xp") != string::npos )
				{
					data.assign(line, found, line.size());
					actor_data_in >> m_xp;
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
	
	//ofstream actor_data_in ( "data/ActorTest.txt",ios_base::in);

	//actor_data_in >> m_name;
	//actor_data_in >> m_ShowInventory;
	//actor_data_in >> m_gold;
	//actor_data_in >> m_mp;
	//actor_data_in >> m_hp;
	//actor_data_in >> m_max_mp;
	//actor_data_in >> m_max_hp;
	//actor_data_in >> m_isactive;
	//actor_data_in >> m_xp;

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

int pod_player::state()
{
	return State;
}

CVector3 pod_player::TileCoords(int iOffsetX, int iOffsetY)
{
	CVector3 tmpVector;
	tmpVector.x = (this->coord.x*64)-iOffsetX*64;
	tmpVector.y = (this->coord.y*64)-iOffsetY*64;
	return tmpVector;
}

float pod_player::ProcentHP()
{
	return 100.0f / (float)this->m_max_hp * (float)this->m_hp;
}

void pod_player::MoveRandom()
{
	if( coord.x > 0 && coord.x < 49)
	coord.x += rand() % 3 -1;
	if( coord.y > 0 && coord.y < 49)
	coord.y += rand() % 3  -1;
}