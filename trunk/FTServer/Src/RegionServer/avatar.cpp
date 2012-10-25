#include "avatar.h"
#include "map.h"
#include "region_player_context.h"
#include "region_logic_loop.h"

#include "region_server_send.h"
#include "session_peer_send.h"

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

void Avatar::AddAvatarToInterestList(Avatar* pAvatar, bool bSendNtf)
{
	m_AvatarInterestList.push_back(pAvatar);
	if (bSendNtf)
	{
		SendAvatarEnterNtf(pAvatar);
	}
}

int32 Avatar::SendAvatarEnterNtf(Avatar* pAvatar)
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x send avatar enter"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId);

	iRet = WChar2Char(pAvatar->m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId);
		_ASSERT( false && "WChar2Char failed" );
		return -1;
	}
	strUtf8[iRet] = '\0';

	iRet = RegionServerSend::RegionAvatarEnterNtf(&m_pPlayerContext->m_pLogicLoop->GetDelaySendData(), pAvatar->m_iAvatarId, strUtf8);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAvatarEnterNtf failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId);
		return -1;
	}

	iRet = SessionPeerSend::SendData(pAvatar->m_pPlayerContext->m_pGateServer, pAvatar->m_pPlayerContext->m_iSessionId, 
		m_pPlayerContext->m_pLogicLoop->GetDelaySendData().m_iDelayTypeId, 
		m_pPlayerContext->m_pLogicLoop->GetDelaySendData().m_iDelayLen, 
		m_pPlayerContext->m_pLogicLoop->GetDelaySendData().m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x SendData failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId);
		return -1;
	}
}

int32 Avatar::_OnMapEnterReq(Map* pMap)
{
	int32 iRet = 0;
	MapGrid* pMapGrid = NULL;
	uint32 iGridCount = 0;
	MapGrid* arrayGrid[9] = {0};

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
	m_pMapGrid = pMapGrid;

	// notify map enter to client
	iRet = SendAvatarEnterNtf(this);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d SendAvatarEnterNtf failed"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);
		return -1;
	}

	// find nearby grid
	iGridCount = pMap->GetNearbyGrids(m_pMapGrid, arrayGrid);

	// update AOI
	pMap->UpdateAOI(this, iGridCount, arrayGrid);

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x mapId=%d avatar enter map"), m_strAvatarName, m_iAvatarId, m_pPlayerContext->m_iSessionId, pMap->m_iMapId);

	return 0;
}


