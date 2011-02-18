#include "cmapstuff.h"

template<typename T>
void writeVector(ofstream &out, const vector<T> &vec)
{
    out << vec.size();

    for(vector<T>::const_iterator i = vec.begin(); i != vec.end(); i++)
    {
        out << *i;
    }
}


std::ostream &
operator << (std::ostream &out, const InventoryItem &i)
{
    out << i.Item << ' ' << i.Description << ' ';
    out << i.Quantity << ' ';
    out << i.wholesaleCost << ' ' << i.retailCost << ' ';
    out << i.dateAdded << ' ';
    return out;
}

std::istream &operator>>(istream &in, InventoryItem &i)
{
    in >> i.Item >> i.Description;
    in >> i.Quantity;
    in >> i.wholesaleCost >> i.retailCost;
    in >> i.dateAdded;
    return in;
}



template<typename T>
vector<T> readVector(ifstream &in)
{
    size_t size;
    in >> size;

    vector<T> vec;
    vec.reserve(size);

    for(unsigned int i = 0; i < size; i++)
    {
        T tmp;
        in >> tmp;
        vec.push_back(tmp);
    }

    return vec;
}

void tester()
{
	std::vector<InventoryItem> tmp;
	InventoryItem dummy;
	dummy.dateAdded = 1;
	dummy.Description =" description";
	dummy.Item = "item";
	dummy.Quantity = 10;
	dummy.wholesaleCost =100;
	dummy.retailCost = 40;
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	tmp.push_back(dummy);
	ofstream out("data.dat");
	writeVector(out,tmp);
	out.close();
		ifstream in("data.dat");

		std::vector<InventoryItem> structList = readVector<InventoryItem>( in );
}
// define how to read a user_rec
std::istream &
operator >> (std::istream &istr, user_rec &ur)
{
 istr >> ur.fname >> ur.lname >> ur.telno;
 return istr;
} 
// define how to write a user_rec
std::ostream &
operator << (std::ostream &ostr, const user_rec &ur)
{
 ostr << "first name: " << ur.fname << '\n';
 ostr << "last name: " << ur.lname << '\n';
 ostr << "telephone number: " << ur.telno << '\n';
 return ostr;
}
void test_user_rec()
{
	ifstream in("data.dat");

 std::istringstream iss("aaa bbbb 123-678-9900\nzzzx xxxx 999-999-9999\n");
 std::vector<user_rec> v;
 user_rec dummy;
 while (true)
 {
  iss >> dummy;
  if (!iss)
   break; // presumably out of data
  v.push_back(dummy);
 }
 for (std::vector<user_rec>::const_iterator i = v.begin(); i != v.end();
++i)
  std::cout << *i;
} 
Cmap::Cmap (int a, int b, int c) {
  width = new int;
  height = new int;
  STile_size = new int;
  *width = a;
  *height = b;
  *STile_size = c;
  offset_x = new int;
  offset_y = new int;
  *offset_x = 0;
  *offset_y = 0;
  tiles.resize(a,b);
}

Cmap::~Cmap () {
  delete width;
  delete height;
  delete STile_size;
  delete offset_x;
  delete offset_y;
}