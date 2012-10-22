#include "map.h"
#include <algorithm>

float32 MapGrid::m_fGridSize = 0.0f;

MapGrid::MapGrid() : m_iGridX(0), m_iGridY(0), m_fMinPositionX(0.0f), m_fMaxPositionX(0.0f), m_fMinPositionY(0.0f), m_fMaxPositionY(0.0f)
{

}

MapGrid::~MapGrid()
{
	m_ActorList.clear();
}

void MapGrid::SetGridPosition(uint32 iGridX, uint32 iGridY)
{
	m_iGridX = iGridX;
	m_iGridY = iGridY;
}

void MapGrid::AddActor(Actor *pActor)
{
	m_ActorList.push_back(pActor);
}

void MapGrid::RemoveActor(Actor *pActor)
{
	std::list<Actor*>::iterator it = std::find(m_ActorList.begin(), m_ActorList.end(), pActor);
	if (it != m_ActorList.end())
	{
		m_ActorList.erase(it);
	}
}

Map::Map() : m_iMapId(0), m_pRegionLogicLoop(NULL), m_iActorMax(0),
			m_arrayActor(NULL), m_iAvatarCount(0), m_iHorizonGridCount(0),
			m_iVerticalGridCount(0), m_array2dMapGrid(NULL)
{
}

Map::~Map()
{
}

int32 Map::Init(uint16 iMapId)
{
	m_iMapId = iMapId;
	return 0;
}

void Map::Destroy()
{
}

Map* Map::Create(uint16 iMapId)
{
	Map* pMap = FT_NEW(Map);
	if (pMap)
	{
		if (pMap->Init(iMapId))
		{
			return pMap;
		}
	}

	return NULL;
}

void Map::Delete(Map* pMap)
{
	pMap->Destroy();
	FT_DELETE(pMap);
}