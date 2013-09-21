#include "map.h"
#include "map_desc.h"
#include "avatar.h"
#include "region_player_context.h"
#include "region_server.h"
#include "region_server_config.h"
#include "data_center.h"
#include "region_logic_loop.h"
#include <algorithm>
#include <cmath>

#include "region_server_send.h"

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
			m_array2dMapGrid[i][j] = FT_NEW(MapGrid);
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

uint32 Map::GetNearbyGrids(MapGrid* pGrid, MapGrid** ppNearbyGrids)
{
	uint32 iCount = 0;

	for (uint32 i = pGrid->m_iGridY - 1; i <= pGrid->m_iGridY + 1; ++i)
	{
		for (uint32 j = pGrid->m_iGridX - 1; j <= pGrid->m_iGridX + 1; ++j)
		{
			if (i >= 0 && i < m_iVerticalGridCount && j >= 0 && j < m_iHorizonGridCount)
			{
				ppNearbyGrids[iCount] = m_array2dMapGrid[i][j];
				++iCount;
			}
		}
	}

	return iCount;
}

int32 Map::UpdateAOI(Avatar* pAvatar, uint32 iGridCount, MapGrid** arrayGrid)
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};
	uint32 iAddAvatarCount = 0;
	
	iRet = WChar2Char(pAvatar->m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), pAvatar->m_strAvatarName, pAvatar->m_iAvatarId, pAvatar->m_pPlayerContext->m_iSessionId);
		_ASSERT( false && "WChar2Char failed" );
		return -1;
	}
	strUtf8[iRet] = '\0';

	// todo: check avatar leave

	// check avatar enter
	for (uint32 i = 0; i < iGridCount; ++i)
	{
		for (std::list<Actor*>::iterator it = arrayGrid[i]->m_ActorList.begin(); it != arrayGrid[i]->m_ActorList.end(); ++it)
		{
			Avatar* pSrcAvatar = (Avatar*)(*it);
			pAvatar->AddAvatarToInterestList(pSrcAvatar, true);
			pSrcAvatar->AddAvatarToInterestList(pAvatar);

			// put avatar to temp array
			iAddAvatarCount = m_pRegionLogicLoop->AddAvatarToTemp(pSrcAvatar);
		}
	}

	if (iAddAvatarCount)
	{
		iRet = RegionServerSend::RegionAvatarEnterNtf(this, pAvatar->m_iAvatarId, strUtf8);
		if (iRet != 0)
		{
			LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAvatarEnterNtf failed"), pAvatar->m_strAvatarName, pAvatar->m_iAvatarId, pAvatar->m_pPlayerContext->m_iSessionId);
			return -1;
		}
		else
		{
			// notify all players in temp array
			m_pRegionLogicLoop->BroadcastToTemp();
		}
	}

	return 0;
}

Map* Map::Create(uint16 iMapId)
{
	Map* pMap = FT_NEW(Map);
	if (pMap)
	{
		if (pMap->Init(iMapId) == 0)
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