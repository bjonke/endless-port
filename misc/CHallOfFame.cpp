#include "../misc/CHallOfFame.h"
#include "../player/SActor.h"

SActor pHallOfFame;

typedef std::map<string,string> mHallOfFame;
typedef std::pair<string,string> pairLeaders;

typedef std::map<string,int> mapHallOfFame;
typedef std::pair<string,int> pairStats;

mHallOfFame mHOFList;
mHallOfFame::iterator HOFp;

mapHallOfFame mHOF;
mapHallOfFame::iterator p;


CHallOfFame::CHallOfFame()
{
}

void CHallOfFame::Refresh(SActor ipod_player)
{
	mHOFList.insert(pairLeaders("Strongest","Crashland"));
	mHOFList.insert(pairLeaders("Smartest","Crashland"));
	mHOFList.insert(pairLeaders("Wisest","Crashland"));
	mHOFList.insert(pairLeaders("Healtiest","Crashland"));
	mHOFList.insert(pairLeaders("Most attractive","Crashland"));
	mHOFList.insert(pairLeaders("Quickest","Crashland"));
	mHOFList.insert(pairLeaders("Deadliest creature defeated","Crashland"));
	mHOFList.insert(pairLeaders("Most experienced explorer","Crashland"));
	mHOFList.insert(pairLeaders("Master of fighting","Crashland"));
	mHOFList.insert(pairLeaders("Master of magic","Crashland"));
	mHOFList.insert(pairLeaders("Master of thieving","Crashland"));


	mHOF.insert(pairStats("Strength",ipod_player.m_Strength));
	mHOF.insert(pairStats("Charisma",ipod_player.m_Charisma));
	mHOF.insert(pairStats("Dexterity",ipod_player.m_Dexterity));
	mHOF.insert(pairStats("Constitution",ipod_player.m_Constitution));
	mHOF.insert(pairStats("Intelligence",ipod_player.m_Intelligence));
	mHOF.insert(pairStats("Wisdom",ipod_player.m_Wisdom));

	if(ipod_player.m_Strength +1> pHallOfFame.m_Strength)
	{
		mHallOfFame::iterator it = mHOFList.find("Strongest");
		it->second = ipod_player.m_Name.c_str();
		pHallOfFame.m_Strength = ipod_player.m_Strength;
	}
	if(ipod_player.m_Charisma +1> pHallOfFame.m_Charisma)
	{
		mHallOfFame::iterator it = mHOFList.find("Most attractive");
		it->second = ipod_player.m_Name.c_str();

		pHallOfFame.m_Charisma = ipod_player.m_Charisma;
	}
	if(ipod_player.m_Dexterity +1> pHallOfFame.m_Dexterity)
	{
		mHallOfFame::iterator it = mHOFList.find("Quickest");
		it->second = ipod_player.m_Name.c_str();

		pHallOfFame.m_Dexterity = ipod_player.m_Dexterity;
	}
	if(ipod_player.m_Constitution +1> pHallOfFame.m_Constitution)
	{
		mHallOfFame::iterator it = mHOFList.find("Healtiest");
		it->second = ipod_player.m_Name.c_str();

		pHallOfFame.m_Constitution = ipod_player.m_Constitution;
	}
	if(ipod_player.m_Intelligence +1> pHallOfFame.m_Intelligence)
	{
		mHallOfFame::iterator it = mHOFList.find("Smartest");
		it->second = ipod_player.m_Name.c_str();

		pHallOfFame.m_Intelligence = ipod_player.m_Intelligence;
	}
	if(ipod_player.m_Wisdom +1> pHallOfFame.m_Wisdom)
	{
		mHallOfFame::iterator it = mHOFList.find("Wisest");
		it->second = ipod_player.m_Name.c_str();

		pHallOfFame.m_Wisdom = ipod_player.m_Wisdom;
	}
}

void CHallOfFame::Render(HDC* iBackBuffer,RECT* Location)
{
	//int Index = 0;
	//char buffer[100];

	for(HOFp = mHOFList.begin(); HOFp != mHOFList.end(); ++HOFp)
	{
		Location->bottom += 16;
			Location->top += 16;
		DrawText(*iBackBuffer,
		HOFp->first.c_str(),
		strlen(HOFp->first.c_str()),
		Location,
		DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		DrawText(*iBackBuffer,
			HOFp->second.c_str(),
			strlen(HOFp->second.c_str()),
		Location,
		DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		//TextOut(*iBackBuffer,16,iy + (Index*16),HOFp->first.c_str(), strlen(HOFp->first.c_str()));
		//TextOut(*iBackBuffer,150,iy + (Index*16),HOFp->second.c_str(), strlen(HOFp->second.c_str()));
		//Index++;
	}
}