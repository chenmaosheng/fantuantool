#include "master_player_context.h"
#include "master_server_loop.h"
#include "master_server.h"
#include "master_server_config.h"

#include "gate_peer_send.h"
#include "session_peer_send.h"
#include "cache_peer_send.h"
#include "login_server_send.h"

#include "packet.h"

MasterServerLoop* MasterPlayerContext::m_pMainLoop = NULL;
uint16 MasterPlayerContext::m_iDelayTypeId = 0;
uint16 MasterPlayerContext::m_iDelayLen = 0;
char MasterPlayerContext::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

MasterPlayerContext::MasterPlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
	_InitStateMachine();
}

MasterPlayerContext::~MasterPlayerContext()
{
	Clear();
}

void MasterPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_strAccountName[0] = _T('\0');
	m_iGateServerId = 0;
	m_bFinalizing = false;
	m_StateMachine.SetCurrState(PLAYER_STATE_NONE);
	m_iAvatarCount = 0;
	memset(m_arrayAvatar, 0, sizeof(m_arrayAvatar));
}

int32 MasterPlayerContext::DelaySendData(uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
}

void MasterPlayerContext::OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGINREQ) != PLAYER_STATE_ONLOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
	
	m_iSessionId = iSessionId;
	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);

	// notify gate
	iRet = m_pMainLoop->GateAllocReq();
	if (iRet < 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x No more gate session to allocate"), strAccountName, iSessionId);
		return;
	}

	m_iGateServerId = (uint8)iRet;
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate a gate session on gate server id=%d"), strAccountName, iSessionId, iRet);

	iRet = GatePeerSend::GateAllocReq(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, (uint16)wcslen(strAccountName)+1, strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Send gate allocate request failed"), strAccountName, iSessionId);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCREQ) != PLAYER_STATE_GATEALLOCREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void MasterPlayerContext::GateAllocAck(uint8 iGateServerId, uint32 iGateSessionId)
{
	int32 iRet = 0;
	GateConfigItem* pConfigItem = NULL;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate gate success"), m_strAccountName, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCACK) != PLAYER_STATE_GATEALLOCACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_iGateServerId = iGateServerId;
	pConfigItem = g_pServerConfig->GetGateConfigItem(iGateServerId);
	if (!pConfigItem)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Get gate server's config failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// send gate server's ip and port to client
	iRet = LoginServerSend::LoginNtf(this, pConfigItem->m_iServerIP, pConfigItem->m_iServerPort);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Send login notification failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pLoginServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Forward packet to login server failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// set state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCNTF) != PLAYER_STATE_GATEALLOCNTF)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_pMainLoop->LoginSession2GateSession(this, m_iSessionId, iGateSessionId);
}

void MasterPlayerContext::OnGateLoginReq()
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONGATELOGINREQ) != PLAYER_STATE_ONGATELOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = CachePeerSend::OnLoginReq(g_pServer->m_pCacheServer, m_iSessionId, wcslen(m_strAccountName)+1, m_strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginReq failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x account logged in finished"), m_strAccountName, m_iSessionId);

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_CACHELOGINREQ) != PLAYER_STATE_CACHELOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}

}

void MasterPlayerContext::OnSessionDisconnect()
{
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect"), m_strAccountName, m_iSessionId);

	m_pMainLoop->ShutdownPlayer(this);
}

void MasterPlayerContext::OnAvatarListReq()
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARLISTREQ) != PLAYER_STATE_ONAVATARLISTREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x send avatar list request to cache server"), m_strAccountName, m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pCacheServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTREQ) != PLAYER_STATE_AVATARLISTREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void MasterPlayerContext::OnAvatarListAck(int32 iReturn, uint8 iAvatarCount, const prdAvatar* pAvatar)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARLISTACK) != PLAYER_STATE_ONAVATARLISTACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive avatar list from cache server"), m_strAccountName, m_iSessionId);

	// check return value
	if (iReturn != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x return value is nonzero"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_iAvatarCount = iAvatarCount;
	memcpy(m_arrayAvatar, pAvatar, sizeof(prdAvatar)*iAvatarCount);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x send avatar list to gate server"), m_strAccountName, m_iSessionId);
	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
	}
	
	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTACK) != PLAYER_STATE_AVATARLISTACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarCreateReq(prdAvatarCreateData &data)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARCREATEREQ) != PLAYER_STATE_ONAVATARCREATEREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive avatar create, name=%s"), m_strAccountName, m_iSessionId, data.m_strAvatarName);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pCacheServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARCREATEREQ) != PLAYER_STATE_AVATARCREATEREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarCreateAck(int32 iReturn, prdAvatar& newAvatar)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARCREATEACK) != PLAYER_STATE_ONAVATARCREATEACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive new avatar from cache server"), m_strAccountName, m_iSessionId);

	// check return value
	if (iReturn != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x return value is nonzero"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	if (m_iAvatarCount >= AVATARCOUNT_MAX)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x avatar is full"), m_strAccountName, m_iSessionId);
		return;
	}

	memcpy(&m_arrayAvatar[m_iAvatarCount], &newAvatar, sizeof(prdAvatar));

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x name=%s send new avatar to gate server"), m_strAccountName, m_iSessionId, newAvatar.m_strAvatarName);
	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
	}
	
	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARCREATEACK) != PLAYER_STATE_AVATARLISTACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((MasterPlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}

