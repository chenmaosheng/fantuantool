#include "region_player_context.h"
#include "region_server_loop.h"
#include "region_server.h"

#include "master_peer_send.h"
#include "session.h"

RegionServerLoop* RegionPlayerContext::m_pMainLoop = NULL;

RegionPlayerContext::RegionPlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
}

RegionPlayerContext::~RegionPlayerContext()
{
	Clear();
}

void RegionPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_iAvatarId = 0;
	m_strAvatarName[0] = _T('\0');

	m_bFinalizing = false;
	m_pGateServer = NULL;
	m_StateMachine.SetCurrState(PLAYER_STATE_NONE);

	m_iMapId = 0;
}

void RegionPlayerContext::OnRegionAllocReq(uint32 iSessionId, uint64 iAvatarId, const TCHAR* strAvatarName)
{
	int32 iRet = 0;
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONALLOCREQ) != PLAYER_STATE_ONREGIONALLOCREQ)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), strAvatarName, iAvatarId, iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x OnRegionAllocReq"), strAvatarName, iAvatarId, iSessionId);

	m_iSessionId = iSessionId;
	m_iAvatarId = iAvatarId;
	wcscpy_s(m_strAvatarName, _countof(m_strAvatarName), strAvatarName);

	m_pGateServer = g_pServer->GetPeerServer(iServerId);
	
	iRet = MasterPeerSend::OnRegionAllocAck(g_pServer->m_pMasterServer, m_iSessionId, iServerId, 0);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAllocAck failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_REGIONALLOCACK) != PLAYER_STATE_REGIONALLOCACK)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), strAvatarName, iAvatarId, iSessionId, m_StateMachine.GetCurrState());
	}
}
