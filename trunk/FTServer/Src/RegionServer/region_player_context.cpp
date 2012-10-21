#include "region_player_context.h"
#include "region_server_loop.h"
#include "region_server.h"
#include "region_server_config.h"
#include "avatar.h"

#include "master_peer_send.h"
#include "cache_peer_send.h"
#include "region_server_send.h"
#include "session_peer_send.h"
#include "gate_peer_send.h"
#include "session.h"

RegionServerLoop* RegionPlayerContext::m_pMainLoop = NULL;
uint16 RegionPlayerContext::m_iDelayTypeId = 0;
uint16 RegionPlayerContext::m_iDelayLen = 0;
char RegionPlayerContext::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

RegionPlayerContext::RegionPlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
	_InitStateMachine();
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
	m_pMap = NULL;
	m_pLogicLoop = NULL;
	m_pAvatar = NULL;
}

int32 RegionPlayerContext::DelaySendData(uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
}

void RegionPlayerContext::OnRegionAllocReq(uint32 iSessionId, uint64 iAvatarId, const TCHAR* strAvatarName)
{
	int32 iRet = 0;
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONALLOCREQ) != PLAYER_STATE_ONREGIONALLOCREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), strAvatarName, iAvatarId, iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x OnRegionAllocReq"), strAvatarName, iAvatarId, iSessionId);

	m_iSessionId = iSessionId;
	m_iAvatarId = iAvatarId;
	wcscpy_s(m_strAvatarName, _countof(m_strAvatarName), strAvatarName);

	m_pGateServer = g_pServer->GetPeerServer(iServerId);
	
	iRet = MasterPeerSend::OnRegionAllocAck(g_pServer->m_pMasterServer, m_iSessionId, g_pServerConfig->m_iServerId, 0);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAllocAck failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_REGIONALLOCACK) != PLAYER_STATE_REGIONALLOCACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), strAvatarName, iAvatarId, iSessionId, m_StateMachine.GetCurrState());
	}
}

void RegionPlayerContext::OnRegionReleaseReq()
{
	// check if shutdown
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x receive region release request"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONRELEASEREQ) != PLAYER_STATE_ONREGIONRELEASEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	m_pMainLoop->ShutdownPlayer(this);
}

void RegionPlayerContext::OnRegionEnterReq()
{
	int32 iRet = 0;

	// check if shutdown
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x receive region enter request"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONENTERREQ) != PLAYER_STATE_ONREGIONENTERREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// initialize avatar
	if (!m_pAvatar)
	{
		m_pAvatar = FT_NEW(Avatar);
		if (!m_pAvatar)
		{
			_ASSERT(false);
			LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x allocate avatar failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
			m_pMainLoop->ShutdownPlayer(this);
			return;
		}

		m_pAvatar->m_pPlayerContext = this;
	}
	else
	{
		// todo: resume connection
	}

	iRet = CachePeerSend::OnRegionEnterReq(g_pServer->m_pCacheServer, m_iSessionId, g_pServerConfig->m_iServerId, wcslen(m_strAvatarName)+1, m_strAvatarName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x OnRegionEnterReq failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_REGIONENTERREQ) != PLAYER_STATE_REGIONENTERREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}
}

void RegionPlayerContext::OnRegionEnterAck()
{
	int32 iRet = 0;

	// check if shutdown
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x receive region enter ack"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONENTERACK) != PLAYER_STATE_ONREGIONENTERACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// send region server info to related gate server
	iRet = GatePeerSend::RegionBindReq(m_pGateServer, m_iSessionId, g_pServerConfig->m_iServerId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionBindReq failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// send time synchronization
	iRet = RegionServerSend::ServerTimeNtf(this, m_pMainLoop->GetCurrTime());
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x ServerTimeNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(m_pGateServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x SendData failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_SERVERTIMENTF) != PLAYER_STATE_SERVERTIMENTF)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void RegionPlayerContext::OnRegionLeaveReq()
{
	int32 iRet = 0;

	// check if shutdown
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x receive region leave req"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONLEAVEREQ) != PLAYER_STATE_ONREGIONLEAVEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	m_pMainLoop->ShutdownPlayer(this);

	// notify all clients
	_BroadcastAvatarLeaveNtf();
}

void RegionPlayerContext::OnClientTimeReq(uint32 iClientTime)
{
	int32 iRet = 0;
	uint32 iCurrTime = m_pMainLoop->GetCurrTime();

	// check if shutdown
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x receive client time req"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONCLIENTTIMEREQ) != PLAYER_STATE_ONCLIENTTIMEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// send time synchronization, add RTT
	iRet = RegionServerSend::ServerTimeNtf(this, iCurrTime + abs((iCurrTime - iClientTime) / 2));
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x ServerTimeNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(m_pGateServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x SendData failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_SERVERTIME2NTF) != PLAYER_STATE_SERVERTIME2NTF)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x state=%d state error"), m_strAvatarName, m_iAvatarId, m_iSessionId, m_StateMachine.GetCurrState());
	}

	//_SendInitialAvatarData();
	_BroadcastAvatarEnterNtf();
	_SendRegionAvatars();
}

void RegionPlayerContext::SendAvatarEnterNtf(RegionPlayerContext* pPlayerContext)
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x send avatar enter"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	iRet = WChar2Char(pPlayerContext->m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		_ASSERT( false && "WChar2Char failed" );
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		return;
	}
	strUtf8[iRet] = '\0';

	iRet = RegionServerSend::RegionAvatarEnterNtf(this, pPlayerContext->m_iAvatarId, strUtf8);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAvatarEnterNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(m_pGateServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x SendData failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void RegionPlayerContext::OnRegionChatReq(const char *strMessage)
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};

	iRet = WChar2Char(m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		_ASSERT( false && "WChar2Char failed" );
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		return;
	}
	strUtf8[iRet] = '\0';

	iRet = RegionServerSend::RegionChatNtf(this, m_iAvatarId, strMessage);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionChatNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_pMainLoop->BroadcastData(m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
}

void RegionPlayerContext::_SendInitialAvatarData()
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x send initial avatar data"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	iRet = WChar2Char(m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		_ASSERT( false && "WChar2Char failed" );
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		return;
	}
	strUtf8[iRet] = '\0';

	iRet = RegionServerSend::InitialAvatarDataNtf(this, m_iAvatarId, strUtf8);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x InitialAvatarDataNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(m_pGateServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x SendData failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void RegionPlayerContext::_BroadcastAvatarEnterNtf()
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x BroadcastAvatarEnter"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	iRet = WChar2Char(m_strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		_ASSERT( false && "WChar2Char failed" );
		LOG_ERR(LOG_SERVER, _T("name=%s aid=%llu sid=%08x WChar2Char failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		return;
	}
	strUtf8[iRet] = '\0';

	iRet = RegionServerSend::RegionAvatarEnterNtf(this, m_iAvatarId, strUtf8);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAvatarEnterNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_pMainLoop->BroadcastData(m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
}

void RegionPlayerContext::_SendRegionAvatars()
{
	m_pMainLoop->SendRegionAvatars(this);
}

void RegionPlayerContext::_BroadcastAvatarLeaveNtf()
{
	int32 iRet = 0;
	
	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x BroadcastAvatarLeave"), m_strAvatarName, m_iAvatarId, m_iSessionId);

	iRet = RegionServerSend::RegionAvatarLeaveNtf(this, m_iAvatarId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x RegionAvatarLeaveNtf failed"), m_strAvatarName, m_iAvatarId, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_pMainLoop->BroadcastData(m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
}






















int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((RegionPlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}
