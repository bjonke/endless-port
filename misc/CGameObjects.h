#ifndef CGAMEOBJECTS_H
#define CGAMEOBJECTS_H

//The base class
class CGameObjects
{
public:
	int UID;
	string name;
	string description;
	vector<int> scripts;
	vector<int> valid_actions;
	// Virtual functions and so on
     CGameObjects();
     CGameObjects(const CGameObjects &);
     ~CGameObjects(){};
     CGameObjects &operator=(const CGameObjects &rhs);
     int operator==(const CGameObjects &rhs) const;
     int operator<(const CGameObjects &rhs) const;
};

class CObjAxe : public CGameObjects
{
   string myType() {return "Axe";};
   void Step(){};

};

class CObjSword : public CGameObjects
{
   string myType() {return "Sword";};
   void Step(){};
};

class CObjBow : public CGameObjects
{
	string myType() {return "Bow";};
	void Step(){};
};

class CObjDagger : public CGameObjects
{
	string myType() {return "Dagger";};
	void Step(){};
};

class CObjPotion : public CGameObjects
{
	string myType() {return "Potion";};
	void Step(){};
};

#endif
