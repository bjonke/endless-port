#ifndef GS_GAMEENGINE_H
#define GS_GAMEENGINE_H

#include <vector>
#include "../gs_Callback.h"

struct SStateManager
{
	gs_Callback cbstate;
	int m_state;
	int m_old_state;


	SStateManager() {};
	~SStateManager() {};
	void Init(int i_state) { m_state = i_state; };
	int get_state(){ return m_state; };
	void set_state(){};
	int alter_state(int i_state)
	{
		m_state = i_state;
		return m_state;
	}
};

#endif