#include "CMap.h"
#include "CMiniMap.h"

CMap::CMap (int a, int b, int c) : MIN_OFFSET(0),MAX_OFFSET(2880)
{
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
  mWorldEdit = false;
}

CMap::~CMap () 
{
  delete width;
  delete height;
  delete STile_size;
  delete offset_x;
  delete offset_y;
}

/*
int CMap::Walkable(int nCol, int nRow)
{
	int TileIndex = tiles[nCol][nRow].Layer[MapTile::BASE] % 128; // 01234 255 values into there correct index
	int TileWalkability = tiles[nCol][nRow].Layer[MapTile::BASE] / TILE_SIZE;
	return TileWalkability;
}*/

// Renders a minimap to the screen using setpixel
// change reference point to upper left corner
void CMap::MiniMap(HDC BackBuffer, CVector3 m_Coord, RECT Screen, POINT iOffset)
{
	char buffer[100];
	CMiniMap MiniMap;
	MiniMap.Render(BackBuffer,m_Coord,Screen,iOffset,this);

	string ActorLocation;

	ActorLocation += _itoa(m_Coord.x,buffer,10);
	ActorLocation += ",";
	ActorLocation += _itoa(m_Coord.y,buffer,10);

	TextOut(BackBuffer,Screen.right - this->get_width()+iOffset.x,this->get_height()+iOffset.y,ActorLocation.c_str(), strlen(ActorLocation.c_str()));
	/*
	static CVector3 NPC(0,0,0);
	static CVector3 Velocity(0,0,0);
	static int Times = 0;
	if( Times <= 0 )
	{
		Times = rand()% 10;
		Velocity.x = rand()%3-1;
		Velocity.y = rand()%3-1;
	}

	if( (NPC.x + Velocity.x) < 0.0f)
		Times--;
	else if( (NPC.y + Velocity.y) < 0.0f)
		Times--;
	else if( (NPC.x + Velocity.x) >= get_width() )
		Times--;
	else if( (NPC.y + Velocity.y) >= get_height() )
		Times--;
	else
	{
	NPC.x += Velocity.x;
	NPC.y += Velocity.y;
	Times--;
	}


	for(int nCol = 0; nCol < *height; nCol++)
	{
		for(int nRow = 0; nRow < *width; nRow++)
		{
			int TileIndex = tiles[nCol][nRow].Layer[MapTile::BASE] % 256; // 01234
			TileIndex = 256 - TileIndex;
			if( nRow == 0 || nRow == *width - 1)
				SetPixelV(BackBuffer,Screen.right - get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,RGB(255,255,255));
			else if( nCol == 0 || nCol == *height - 1)
				SetPixelV(BackBuffer,Screen.right - get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,RGB(255,255,255));
			else
			{
				SetPixelV(BackBuffer,Screen.right - get_width() + nRow + iOffset.x,Screen.top + nCol + iOffset.y,RGB(TileIndex,TileIndex,TileIndex));
				//SetPixelV(BackBuffer,Screen.right - get_width() + (int)m_Coord.x  + iOffset.x,Screen.top + (int)m_Coord.y  + iOffset.y,RGB(0,255,0));
				//SetPixelV(BackBuffer,Screen.right - get_width() + (int)NPC.x  + iOffset.x,Screen.top + (int)NPC.y  + iOffset.y,RGB(255,0,0));
			}
		}
	}*/
}

void CMap::OffsetX(int o_v) 
{ 
	if( *offset_x < MIN_OFFSET )
		*offset_x = MIN_OFFSET;
	else
		*offset_x = o_v;
}

void CMap::OffsetY(int o_v)
{
	if( *offset_y > MAX_OFFSET )
		*offset_y = MAX_OFFSET;
	else
		*offset_y = o_v;
}

void CMap::CreateTiles()
{
	ifstream BaseTiles("data/maps/BASE.txt",ios_base::in);
	ifstream FringeTiles("data/maps/FRINGE.txt",ios_base::in);
	ifstream ObjectTiles("data/maps/OBJECT.txt",ios_base::in);

	for(int nCol = 0; nCol < *height; nCol++)
	{
		for(int nRow = 0; nRow < *width; nRow++)
		{
			BaseTiles >> tiles[nCol][nRow].Layer[MapTile::BASE];
			FringeTiles >> tiles[nCol][nRow].Layer[MapTile::FRINGE];
			ObjectTiles >> tiles[nCol][nRow].Layer[MapTile::OBJECT];
			if(tiles[nCol][nRow].Layer[MapTile::BASE] == EVENT_TELEPORT)
			{
				tiles[nCol][nRow].STrigger.mEvents = EVENT_TELEPORT;
				tiles[nCol][nRow].STrigger.mTickTimer = 0;
				tiles[nCol][nRow].STrigger.mX = rand()%30;
				tiles[nCol][nRow].STrigger.mY = rand()%30;
			}
			if(tiles[nCol][nRow].Layer[MapTile::BASE] == EVENT_DROWNING)
			{
				tiles[nCol][nRow].STrigger.mEvents = EVENT_DROWNING;
				tiles[nCol][nRow].STrigger.mTickTimer = 100;
				tiles[nCol][nRow].STrigger.mX = rand()%30;
				tiles[nCol][nRow].STrigger.mY = rand()%30;
			}
		}
	}
	BaseTiles.close();
	FringeTiles.close();
	ObjectTiles.close();
};

bool CMap::SaveWorld()
{
	ofstream BaseTiles("data/maps/BASE.txt",ios_base::out);
	ofstream FringeTiles("data/maps/FRINGE.txt",ios_base::out);
	ofstream ObjectTiles("data/maps/OBJECT.txt",ios_base::out);

	for(int nCol = 0; nCol < *height; nCol++)
	{
		for(int nRow = 0; nRow < *width; nRow++)
		{
			BaseTiles << tiles[nCol][nRow].Layer[MapTile::BASE] << " ";
			FringeTiles << tiles[nCol][nRow].Layer[MapTile::FRINGE] << " ";
			ObjectTiles << tiles[nCol][nRow].Layer[MapTile::OBJECT] << " ";
		}
		BaseTiles << "\n";
		FringeTiles << "\n";
		ObjectTiles << "\n";
	}

	return true;
}

STile_data CMap::GetTriggers(SActor ipod)
{
	return tiles[ipod.m_Coord.x][ipod.m_Coord.y];
}

STile_data CMap::SetTriggers(SActor ipod, DWORD iConditions)
{
	tiles[ipod.m_Coord.x][ipod.m_Coord.y].STrigger.mConditions = iConditions;
	return tiles[ipod.m_Coord.x][ipod.m_Coord.y];
}

STile_data CMap::get_tile(int nCol, int nRow)
{
	return tiles[nCol][nRow];
}

void CMap::SetFrame(int nCol, int nRow,int value)
{
	tiles[nCol][nRow].mTriggers = EX_1 | EX_2;
	tiles[nCol][nRow].Layer[MapTile::OBJECT] = value;
}

void CMap::SetTile(int nCol, int nRow,double value,int iLayer)
{
	int Transition = 0;
	Transition = tiles[nCol][nRow].Layer[iLayer] + (int)value;

	if( Transition > 255 )
	{
		tiles[nCol][nRow].Layer[iLayer] = 0;
	}
	else if( Transition < 0)
	{
		tiles[nCol][nRow].Layer[iLayer] = 255;
	}
	else
	{
		tiles[nCol][nRow].Layer[iLayer] += (int)value;
	}
}

// iLayer is the number of layers to change from current layer
bool CMap::GetWorldEdit(int iLayer)
{
	return mWorldEdit;
}
// iLayer is the number of layers to change from current layer
void CMap::SetWorldEdit(int iLayer)
{
	mWorldEdit = !mWorldEdit;
}

void CMap::LayerUp()
{
	if(mLayer > 0)
		mLayer--;
	//Sleep(100);
}

// Change the if statement to something better
void CMap::LayerDown()
{
	if(mLayer < tiles[0][0].GetLayers())
		mLayer++;
	//Sleep(100);
}

int CMap::GetLayer()
{
	return mLayer;
}