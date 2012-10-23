#include "avatar.h"
#include "map.h"
#include "region_player_context.h"

Avatar::Avatar() : m_StateMachine(0)
{
	m_iActorType = ACTOR_TYPE_AVATAR;
	m_pPlayerContext = NULL;
	m_bFinalizing = false;
	
	m_strAvatarName[0] = _T('\0');
	m_iAvatarId = 0;

	m_iTeleportMapId = 0;

	_InitStateMachine();
}

Avatar::~Avatar()
{
}

void Avatar::OnMapEnterReq(Map *pMap)
{
	int32 iRet = 0;

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);

	iRet = _OnMapEnterReq(pMap);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d _OnMapEnterReq failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);
	}
}

int32 Avatar::_OnMapEnterReq(Map* pMap)
{
	int32 iRet = 0;
	MapGrid* pMapGrid = NULL;

	// bind actor to map
	iRet = pMap->BindActor(this);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d BindActor failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);
		return iRet;
	}

	// bind actor to map grid
	pMapGrid = pMap->GetGridByPosition(m_vPosition);
	if (!pMapGrid)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d GetGridByPosition failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);
		return -1;
	}

	pMapGrid->BindActor(this);


	return 0;
}


