#ifndef CINPUT_H
#define CINPUT_H

#define KEYBOARD 1
#define MOUSE 2
#define JOYSTICK 3

class cCallback
{
    public:
        virtual void Execute(int Param) const =0;
};


template <class cInstance>
class TCallback : public cCallback
{
    public:
        TCallback()    // constructor
        {
            pFunction = 0;
        }

        typedef void (cInstance::*tFunction)(int Param);
        
        virtual void Execute(int Param) const 
        {
            if (pFunction) (cInst->*pFunction)(Param);
            else printf("ERROR : the callback function has not been defined !!!!");
        }

        void SetCallback (cInstance  *cInstancePointer, 
                          tFunction   pFunctionPointer)
        {
            cInst     = cInstancePointer;
            pFunction = pFunctionPointer;
        }

    private:
        cInstance  *cInst;
        tFunction  pFunction;
};

typedef struct
{
	int MoveLeft,MoveRight,MoveUp,MoveDown,Fire,Idle;
	bool Inventory_state;
	bool CharSheetState;
} Sinput;

class CInput
{
private:
public:
	CInput() {}
	void Keyboard( Sinput *i )
	{
		if(KEY_DOWN(VK_LEFT))
		{
			i->MoveLeft = 1;
		}
		if(KEY_UP(VK_LEFT))
		{
			i->MoveLeft = 0;
		}
		if(KEY_DOWN(VK_RIGHT))
		{
			i->MoveRight = 1;
		}
		if(KEY_UP(VK_RIGHT))
		{
			i->MoveRight = 0;
		}
		if(KEY_DOWN(VK_UP))
		{
			i->MoveUp = 1;
		}
		if(KEY_UP(VK_UP))
		{
			i->MoveUp = 0;
		}
		if(KEY_DOWN(VK_DOWN))
		{
			i->MoveDown = 1;
		}
		if(KEY_UP(VK_DOWN))
		{
			i->MoveDown = 0;
		}
		if(KEY_UP(VK_SPACE))
		{
			i->Fire = 1;
		}
		if(KEY_UP(VK_SPACE))
		{
			i->Fire = 0;
		}
		if(!i->MoveDown && !i->MoveLeft && !i->MoveRight && !i->MoveUp)
		{
			i->Idle = 0;
		}
		if(KEY_DOWN(VK_B))
		{
			i->Inventory_state = !i->Inventory_state;
		}
		if(KEY_DOWN(VK_C))
		{
			i->CharSheetState = !i->CharSheetState;
		}
		if(KEY_DOWN(VK_F5))
		{
			//fmap.SetWorldEdit(0);
		}
		if(KEY_DOWN(VK_PRIOR))
		{
			//fmap.LayerUp();
		}
		if(KEY_DOWN(VK_NEXT))
		{
			//fmap.LayerDown();
		}
		else
		{
			i->Idle = 0;
		}
	}

	bool find_passable_tile(SActor *p, CMap *map)
	{
		for( int i_height = 0; i_height < map->get_height(); i_height++ )
		{
			for( int i_width = 0; i_width < map->get_width(); i_width++ )
			{
				if(fmap.get_tile(i_height,i_width).Layer[MapTile::BASE] < 64)
				{
					POINT p;
					p.x = i_width;
					p.y = i_height;
					//fmap.Passable(p);
					return true;
				}
			}
		}
		return false;
	}
	bool check_collision(SActor *p, CMap *map)
	{
		static bool done = false;
		find_passable_tile(p, map);
		
		for(int i_height = (int)p->m_Coord.y - 1; i_height <= (int)p->m_Coord.y + 1; i_height++ )
		{
			for(int i_width = (int)p->m_Coord.x - 1; i_width <= (int)p->m_Coord.x + 1; i_width++ )
			{
				if(i_height >= 0 && i_width >= 0)
					if(i_height < fmap.get_height() && i_width < fmap.get_width())
					{
						cout << (int)fmap.get_tile(i_height,i_width).Layer[0] << " | ";
					}
			}
			cout << endl;
		}
		
		// Bug where the position of the map gets altered after change between actors
		//if(p->m_IsNPC)
		//{
		//p->coord.x = fmap.get_passable().x;
		//fmap.OffsetX(p->coord.x*64);
		//p->coord.y = fmap.get_passable().y;
		//fmap.OffsetY(p->coord.y*64);
		//p->m_IsNPC = false;
		//done = true;
		//}
		return true;
	}

	void get_input(SActor *p)
	{
		Sinput i;

		Keyboard((Sinput *)&i);
		p->m_Input.Fire = i.Fire;
		p->m_Input.MoveDown = i.MoveDown;
		p->m_Input.MoveLeft = i.MoveLeft;
		p->m_Input.MoveRight = i.MoveRight;
		p->m_Input.MoveUp = i.MoveUp;
		//p->m_ShowInventory = i.inventory_state;

		fmap.SetTriggers(*p,NULL);
		check_collision(p,&fmap);
		//Sleep(100);

		if(i.Fire)
		{
			TextOut(BackBuffer,Screen.right/2,Screen.top, p->m_Name.c_str(),strlen(p->m_Name.c_str()));		
			p->m_IsActive = true;
			p->m_State = ActorState::ATTACKING;
		}
		else
		{
			p->m_IsActive = false;
			p->m_State = ActorState::IDLE;
		}

		if(i.MoveLeft)
		{
			if(p->m_Coord.x > 0 && p->m_Coord.x <= fmap.get_width()-7)
			{
				if(fmap.get_tile((int)p->m_Coord.y,(int)p->m_Coord.x-1).Layer[MapTile::BASE] < 64)
				{
					
					p->m_Coord = p->m_Coord.sub(CVector3(1,0,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::LEFT;
					p->m_State = ActorState::MOVING;
					
				}
			}
			else if(p->m_Coord.x > fmap.get_width()-7)
			{
				if(fmap.get_tile((int)p->m_Coord.y,(int)p->m_Coord.x-1).Layer[MapTile::BASE] < 64)
				{
					p->m_Coord = p->m_Coord.sub(CVector3(1,0,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::LEFT;
					p->m_State = ActorState::MOVING;
				}
			}
		}

		if(i.MoveRight)
		{
			if(p->m_Coord.x < fmap.get_width()-1)
			{
				if(fmap.get_tile((int)p->m_Coord.y,(int)p->m_Coord.x+1).Layer[MapTile::BASE] < 64)
				{
					p->m_Coord = p->m_Coord.add(CVector3(1,0,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::RIGHT;
					p->m_State = ActorState::MOVING;
				}
			}
		}

		if(i.MoveUp)
		{
			if(p->m_Coord.y > 0 && p->m_Coord.y <= fmap.get_height()-5)
			{
				if(fmap.get_tile((int)p->m_Coord.y-1,(int)p->m_Coord.x).Layer[MapTile::BASE] < 64)
				{
					p->m_Coord = p->m_Coord.sub(CVector3(0,1,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::UP;
					p->m_State = ActorState::MOVING;
				}
			}
			else if(p->m_Coord.y > fmap.get_height()-5 && p->m_Coord.y > 0)
			{
				if(fmap.get_tile((int)p->m_Coord.y-1,(int)p->m_Coord.x).Layer[MapTile::BASE] < 64)
				{
					p->m_Coord = p->m_Coord.sub(CVector3(0,1,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::UP;
					p->m_State = ActorState::MOVING;
				}
			}
		}

		if(i.MoveDown)
		{
			if(p->m_Coord.y < fmap.get_height()-1)
			{
				if(fmap.get_tile((int)p->m_Coord.y+1,(int)p->m_Coord.x).Layer[MapTile::BASE] < 64)
				{
					p->m_Coord = p->m_Coord.add(CVector3(0,1,0));
					p->m_IsMoving = 1;
					p->m_Facing = ActorFacing::DOWN;
					p->m_State = ActorState::MOVING;
				}
			}
		}

		fmap.SetTriggers(*p,CONDITION_PLAYER);
		if(p->m_Coord.y > 4 && p->m_Coord.y < fmap.get_height()-5 )
		fmap.OffsetY((int)p->m_Coord.y*64);
		else if(p->m_Coord.y < 5)
			fmap.OffsetY(0);
		if(p->m_Coord.x > 6 && p->m_Coord.x < fmap.get_width()-6)
		fmap.OffsetX((int)p->m_Coord.x*64);
				else if(p->m_Coord.x < 7)
			fmap.OffsetX(0);
	
	}
};

#endif