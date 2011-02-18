#ifndef CINVENTORY_H
#define CINVENTORY_H

#include <list>
#include <vector>

class CItemSlot
{
private:
public:
	virtual void Activate(){};
};

class CharacterItemSlot : public CItemSlot
{
private:
public:
};

class BagItemSlot : public CItemSlot
{
private:
	int Position;
public:
};

class CInventory
{
private:
	list<LPCSTR> item_name;
	list<int> item_value;
	list<LPCSTR>::iterator iter_name;

	vector<string> m_name;
	vector<string>::iterator m_iter_name;
	const unsigned int MAX_ITEMS;

public:
	CInventory():MAX_ITEMS(10){};
	~CInventory(){};

	bool Vadd(string i_name, int i_value)
	{
		if( i_name.c_str() != "" || i_value != 0 )
		{
			m_name.push_back( i_name );
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Vremove( string i_name, unsigned int i_at )
	{
		if( m_name.size() > 0 && i_at < m_name.size() )
		{
			m_name.at(i_at).clear();
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string VTransfer(std::string, int i_at, int i_dir)
	{
		return m_name.at(i_at).c_str();
	}

	std::string Vexchange(int i_at)
	{
		return m_name.at(i_at).c_str();
	}

	bool vRecieve(std::string i_name)
	{
		m_name.push_back( i_name.c_str() );
		return true;
	}

	int Vsize()
	{
		return m_name.size();
	}

	int add(LPCSTR i_name, int i_value)
	{
		if( item_name.size() < MAX_ITEMS )
		{
			item_name.push_front(i_name);
			item_value.push_front(i_value);
			return item_value.front();
		}
		else
		{
			return 0;
		}
		

	}

	bool remove(std::string i_name)
	{
		if( !item_name.empty() )
		for(iter_name=item_name.begin(); iter_name != item_name.end(); ++iter_name)
		{
			const char * pItem = *iter_name;
			if( pItem == "shop item" )
			{
				iter_name = item_name.erase(iter_name);
			}
		}

		if( !item_name.empty() )
		{
			//item_name.pop_front();
			//item_value.pop_front();
			return true;
		}
		else
		{
			MessageBox(NULL,"No items","Inventory",MB_OK);
			return false;
		}
	}

	void list(HDC BackBuffer, int i_x, int i_y)
	{
		int yloc = 0;
		int counter = 0;
		for(iter_name=item_name.begin(); iter_name != item_name.end(); ++iter_name)
		{
			if(counter > 5)
			yloc = abs(counter/5);

			Rectangle(BackBuffer,i_x+(128*counter),i_y + (40*yloc),i_x+(128*counter) + 128,i_y + (40*yloc)+40);
			TextOut(BackBuffer,i_x+(128*counter),i_y + (40*yloc),*iter_name, strlen(*iter_name));
			counter++;
		}
	}
};

typedef std::tr1::unordered_map<int, CInventory> InventoryTable;

#endif