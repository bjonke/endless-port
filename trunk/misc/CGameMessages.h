#ifndef CGAMEMESSAGES_H
#define CGAMEMESSAGES_H

//The base class
class CGameMessages
{
public:
	int UID;
	string name;
	string description;
	vector<int> scripts;
	vector<int> valid_actions;
	list<string> Messages;
	// Virtual functions and so on
    CGameMessages();
    CGameMessages(const CGameMessages &);
    ~CGameMessages(){};
    CGameMessages &operator=(const CGameMessages &rhs);
    int operator==(const CGameMessages &rhs) const;
    int operator<(const CGameMessages &rhs) const;
	void Render(HDC BackBuffer, RECT Screen, POINT iOffset);
	void Post(std::string iMessage);
};



#endif
