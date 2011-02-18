#ifndef CCOMPANION_H
#define CCOMPANION_H

//The base class
class Companion
{
public:
  string name;
  string description;
  int hp;
  vector<int> scripts;
  vector<int> valid_actions;
  GS_BITMAPS Image;
  // Virtual functions and so on
  Companion()
  {
	  valid_actions.push_back(ATTACK_ACTION);
	  valid_actions.push_back(DEFEND_ACTION);
  };
  virtual string myType (void) =0;
  virtual void SetImage (GS_BITMAPS iImage) =0;
};

class Amphisbaena : public Companion
{
   string myType() {return "Amphisbaena";};
   void SetImage (GS_BITMAPS iImage){};
};

class Alerion : public Companion
{
   string myType() {return "Alerion";};
   void SetImage (GS_BITMAPS iImage){};
};

class Ant : public Companion
{
public:
	string myType() {return "Ant";};
	void SetImage (GS_BITMAPS iImage){Image = iImage;};
};

class Antelope : public Companion
{
	string myType() {return "Antelope";};
	void SetImage (GS_BITMAPS iImage){};
};

#endif