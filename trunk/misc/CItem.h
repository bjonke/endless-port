#ifndef CITEM_H
#define CITEM_H

//The base class
class CItem
{
public:
	int UID;
	string name;
	string description;
	vector<int> scripts;
	vector<int> valid_actions;
	// Virtual functions and so on
	CItem(){};
	virtual void Step() = 0;
};

class CAxe : public CItem
{
   string myType() {return "Axe";};
   void Step(){};

};

class CSword : public CItem
{
   string myType() {return "Sword";};
   void Step(){};
};

class CBow : public CItem
{
	string myType() {return "Bow";};
	void Step(){};
};

class CDagger : public CItem
{
	string myType() {return "Dagger";};
	void Step(){};
};

class CPotion : public CItem
{
	string myType() {return "Potion";};
	void Step(){};
};

#endif
