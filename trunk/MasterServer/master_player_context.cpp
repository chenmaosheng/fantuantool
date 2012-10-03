#include "master_player_context.h"
#include "master_server_loop.h"
#include "gate_peer_send.h"
#include "master_server.h"
#include "master_server_config.h"
#include "login_server_send.h"
#include "session_peer_send.h"
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
	m_strAccountName[0] = '\0';
	m_iGateServerId = 0;
	m_bFinalizing = false;
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
	iRet = m_pMainLoop->GateHoldReq();
	if (iRet < 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x No more gate session to hold"), strAccountName, iSessionId);
		return;
	}

	m_iGateServerId = (uint16)iRet;
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Hold a gate session on gate server id=%d"), strAccountName, iSessionId, iRet);

	iRet = GatePeerSend::GateHoldReq(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, (uint16)wcslen(strAccountName)+1, strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Send gate hold request failed"), strAccountName, iSessionId);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEHOLDREQ) != PLAYER_STATE_GATEHOLDREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void MasterPlayerContext::GateHoldAck(uint16 iGateServerId, uint32 iGateSessionId)
{
	int32 iRet = 0;
	GateConfigItem* pConfigItem = NULL;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Hold gate success"), m_strAccountName, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEHOLDACK) != PLAYER_STATE_GATEHOLDACK)
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
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEHOLDNTF) != PLAYER_STATE_GATEHOLDNTF)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// todo: login session id delete, add gate session id
	m_pMainLoop->LoginSession2GateSession(this, m_iSessionId, iGateSessionId);
}

void MasterPlayerContext::OnSessionDisconnect()
{
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect"), m_strAccountName, m_iSessionId);

	m_pMainLoop->ShutdownPlayer(this);
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((MasterPlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}

