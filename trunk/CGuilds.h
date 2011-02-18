#include <string>

using namespace std;

#ifndef CGUILDS_H
#define CGUILDS_H

class CGuilds
{
public:
	std::string GuildMaster;
	std::string GuildName;
	int Strength;
	int intelligence;
	int Wisdom;
	int Constitution;
	int Charisma;
	int Dexterity;
	int XpToLevel;


	CGuilds();
	~CGuilds();
	void ChangeGuild(int NewGuildId);
	void GainLevel(int Exp);
	void ListGuilds();
};

#endif