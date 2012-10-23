#include "map.h"
#include "map_desc.h"
#include "actor.h"
#include "region_server.h"
#include "region_server_config.h"
#include "data_center.h"
#include <algorithm>
#include <cmath>

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

void MapGrid::BindActor(Actor *pActor)
{
	m_ActorList.push_back(pActor);
}

void MapGrid::UnbindActor(Actor *pActor)
{
	std::list<Actor*>::iterator it = std::find(m_ActorList.begin(), m_ActorList.end(), pActor);
	if (it != m_ActorList.end())
	{
		m_ActorList.erase(it);
	}
}

Map::Map() : m_iMapId(0), m_pRegionLogicLoop(NULL), m_iActorMax(0), m_iActorIndexMax(0),
			m_arrayActor(NULL), m_iAvatarCount(0), m_pMapDesc(NULL), m_iHorizonGridCount(0),
			m_iVerticalGridCount(0), m_array2dMapGrid(NULL)
{
}

Map::~Map()
{
}

int32 Map::Init(uint16 iMapId)
{
	ActorId actorId;
	m_iMapId = iMapId;

	// read map desc from data center
	m_pMapDesc = g_pDataCenter->GetMapDesc(iMapId);
	if (!m_pMapDesc)
	{
		LOG_ERR(LOG_SERVER, _T("failed to read map desc, id=%d"), iMapId);
		return -1;
	}
	// estimate actor max count, todo:
	m_iActorMax = g_pServerConfig->m_iPlayerMax * 10;

	// initialize actor id pool
	for (uint32 i = 0; i < m_iActorMax; ++i)
	{
		actorId.m_sValue.m_iActorIndex = i;
		m_ActorIdPool.push_back(actorId.m_iValue);
	}

	// initialize all of map grids
	MapGrid** ppMapGrid = NULL;
	MapGrid::m_fGridSize = 50.0f;
	m_iHorizonGridCount = (uint32)ceil((m_pMapDesc->m_fMaxX - m_pMapDesc->m_fMinX) / MapGrid::m_fGridSize);
	m_iVerticalGridCount = (uint32)ceil((m_pMapDesc->m_fMaxY - m_pMapDesc->m_fMinY) / MapGrid::m_fGridSize);

	m_array2dMapGrid = new MapGrid**[m_iVerticalGridCount];
	ppMapGrid = new MapGrid*[m_iHorizonGridCount*m_iVerticalGridCount];
	memset(ppMapGrid, 0, sizeof(MapGrid*)*m_iHorizonGridCount*m_iVerticalGridCount);

	for (uint32 i = 0; i < m_iVerticalGridCount; ++i)
	{
		m_array2dMapGrid[i] = ppMapGrid + i * m_iHorizonGridCount;
		for (uint32 j = 0; j < m_iHorizonGridCount; ++j)
		{
			m_array2dMapGrid[i][j] = new MapGrid;
			m_array2dMapGrid[i][j]->SetGridPosition(j, i);
			m_array2dMapGrid[i][j]->m_fMinPositionX = m_pMapDesc->m_fMinX + j * MapGrid::m_fGridSize;
			m_array2dMapGrid[i][j]->m_fMaxPositionX = m_array2dMapGrid[i][j]->m_fMinPositionX + MapGrid::m_fGridSize;
			m_array2dMapGrid[i][j]->m_fMinPositionY = m_pMapDesc->m_fMinY + j * MapGrid::m_fGridSize;
			m_array2dMapGrid[i][j]->m_fMaxPositionY = m_array2dMapGrid[i][j]->m_fMinPositionY + MapGrid::m_fGridSize;
		}
	}

	// bind area to grid if overlapped
	for (uint32 i = 0; i < m_iVerticalGridCount; ++i)
	{
		for (uint32 j = 0; j < m_iHorizonGridCount; ++j)
		{
			MapGrid* pGrid = m_array2dMapGrid[i][j];
			for (std::vector<AreaDesc>::iterator it = m_pMapDesc->m_vAreaDesc.begin(); it != m_pMapDesc->m_vAreaDesc.end(); ++it)
			{
				AreaDesc* pArea = &(*it);
				if (_CheckIfAreaOverlap(pGrid->m_fMinPositionX, pGrid->m_fMaxPositionX,
					pGrid->m_fMinPositionY, pGrid->m_fMaxPositionY, pArea->m_fMinX,
					pArea->m_fMaxX, pArea->m_fMinY, pArea->m_fMaxY))
				{
					pGrid->m_arrayArea.push_back(pArea);
				}
			}
		}
	}

	// initialize all actors
	m_arrayActor = new Actor*[m_iActorMax];
	memset(m_arrayActor, 0, sizeof(Actor*)*m_iActorMax);

	return 0;
}

void Map::Destroy()
{
	// delete all undeleted actors
	for (uint32 i = 0; i < m_iActorMax; ++i)
	{
		if (m_arrayActor[i])
		{
			FT_DELETE(m_arrayActor[i]);
			m_arrayActor[i] = NULL;
		}
	}
	SAFE_DELETE_ARRAY(m_arrayActor);

	m_ActorIdPool.clear();

	// clear all map grid
	for (uint32 i = 0; i < m_iVerticalGridCount; ++i)
	{
		for (uint32 j = 0; j < m_iHorizonGridCount; ++j)
		{
			SAFE_DELETE(m_array2dMapGrid[i][j]);
		}
	}
	SAFE_DELETE_ARRAY(m_array2dMapGrid[0]);
	SAFE_DELETE_ARRAY(m_array2dMapGrid);
}

int32 Map::BindActor(Actor* pActor)
{
	int32 iRet = 0;
	uint32 iActorIndex = 0;

	// set actor id by actor type
	pActor->m_iActorId = _AllocActorId(pActor->m_iActorType);
	pActor->m_pMap = this;

	// bind actor to map
	iActorIndex = ((ActorId*)&pActor->m_iActorId)->m_sValue.m_iActorIndex;
	m_arrayActor[iActorIndex] = pActor;
	m_iActorIndexMax = m_iActorIndexMax < iActorIndex ? iActorIndex : m_iActorIndexMax;

	// set avatar count
	if (pActor->IsAvatar())
	{
		m_iAvatarCount++;
	}

	return 0;
}

void Map::UnbindActor(Actor* pActor)
{
	uint32 iActorIndex = ((ActorId*)&pActor->m_iActorId)->m_sValue.m_iActorIndex;
	m_arrayActor[iActorIndex] = NULL;
	pActor->m_pMap = NULL;

	_FreeActorId(pActor->m_iActorId);
	pActor->m_iActorId = 0;

	// set avatar count
	if (pActor->IsAvatar())
	{
		m_iAvatarCount--;
	}
}

MapGrid* Map::GetGridByPosition(const Vector3& pos)
{
	uint32 iGridX = (uint32)floor((pos.m_fX - m_pMapDesc->m_fMinX) / MapGrid::m_fGridSize);
	uint32 iGridY = (uint32)floor((pos.m_fY - m_pMapDesc->m_fMinY) / MapGrid::m_fGridSize);

	return m_array2dMapGrid[iGridX][iGridY];
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

uint32 Map::_AllocActorId(uint8 iActorType)
{
	uint32 iActorId = m_ActorIdPool.front();
	m_ActorIdPool.pop_front();

	((ActorId*)&iActorId)->m_sValue.m_iSequence++;
	((ActorId*)&iActorId)->m_sValue.m_iActorType = iActorType;

	return iActorId;
}

void Map::_FreeActorId(uint32 iActorId)
{
	m_ActorIdPool.push_back(iActorId);
}

bool Map::_CheckIfAreaOverlap(float fMinX1, float fMaxX1, float fMinY1, float fMaxY1, float fMinX2, float fMaxX2, float fMinY2, float fMaxY2)
{
	float left = max(fMinX1, fMinX2);
	float right = min(fMaxX1, fMaxX2);
	float top = max(fMinY1, fMinY2);
	float bottom = min(fMaxY1, fMaxY2);

	return (left <= right) && (top <= bottom);
}