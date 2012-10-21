#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"
#include "master_peer_send.h"
#include "gate_server_config.h"
#include "session_peer_send.h"

GateServerLoop* GateSession::m_pMainLoop = NULL;
uint16 GateSession::m_iDelayTypeId = 0;
uint16 GateSession::m_iDelayLen = 0;
char GateSession::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

GateSession::GateSession()
{
	Clear();

	// initialize state machine
	InitStateMachine();
}

GateSession::~GateSession()
{
	Clear();
}

void GateSession::Clear()
{
	super::Clear();
	m_strAccountName[0] = _T('\0');
	m_bTempSession = true;
	m_bFinalizing = false;
	m_pRegionServer = NULL;
}

int32 GateSession::OnConnection(ConnID connId)
{
	super::OnConnection(connId);
	m_bTempSession = true;

	LOG_DBG(LOG_SERVER, _T("sid=%08x Receive a connection"), m_iSessionId);
	return 0;
}

void GateSession::OnDisconnect()
{
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x connection is disconnected"), m_strAccountName, m_iSessionId);
	// check and set state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDISCONNECT) < 0)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	switch (m_StateMachine.GetCurrState())
	{
	case SESSION_STATE_HOLDCONNECTION:
		{
			if (m_pMainLoop->IsStartShutdown())
			{
				m_pMainLoop->CloseSession(this);
			}
			else
			{
				iRet = SessionPeerSend::OnSessionDisconnect(g_pServer->m_pMasterServer, m_iSessionId);
				if (iRet != 0)
				{
					LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnSessionDisconnect failed"), m_strAccountName, m_iSessionId);
					return;
				}

				if (m_StateMachine.StateTransition(SESSION_EVENT_SESSIONDISCONNECTREQ) != SESSION_STATE_ONDISCONNECT)
				{
					_ASSERT(false && _T("state error"));
					LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
				}

				m_pMainLoop->CloseSession(this);
			}
		}
		break;

	case SESSION_STATE_ONDISCONNECT:
		{
			iRet = SessionPeerSend::OnSessionDisconnect(g_pServer->m_pMasterServer, m_iSessionId);
			if (iRet != 0)
			{
				LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnSessionDisconnect failed"), m_strAccountName, m_iSessionId);
				return;
			}

			m_pMainLoop->CloseSession(this);
		}
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		break;
	}
}

void GateSession::Disconnect()
{
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x force to disconnect"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(SESSION_EVENT_DISCONNECT) < 0)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	super::Disconnect();
}

void GateSession::OnGateAllocReq(uint32 iLoginSessionId, const TCHAR *strAccountName)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONGATEALLOCREQ) != SESSION_STATE_ONGATEALLOCREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	((SessionId*)&m_iSessionId)->sValue_.sequence_++;
	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);
	m_iLoginSessionId = iLoginSessionId;
	m_bTempSession = false;

	iRet = MasterPeerSend::GateAllocAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, iLoginSessionId, (uint16)wcslen(strAccountName)+1, strAccountName, m_iSessionId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x gate allocate ack failed to send"), strAccountName, m_iSessionId);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_GATEALLOCACK) != SESSION_STATE_GATEALLOCACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%d sid=%08x send gate allocate ack to master server"), strAccountName, m_iSessionId);
}

void GateSession::OnGateReleaseReq()
{
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x connection is disconnected"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(SESSION_EVENT_GATERELEASEREQ) != SESSION_STATE_GATERELEASEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		
		m_pMainLoop->CloseSession(this, true);
		return;
	}

	m_pMainLoop->CloseSession(this, true);
}

void GateSession::OnRegionBindReq(uint8 iRegionServerId)
{
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnRegionBindReq"), m_strAccountName, m_iSessionId);

	m_pRegionServer = g_pServer->GetPeerServer(iRegionServerId);
}

void GateSession::Clone(GateSession* pSession)
{
	// source must not be temp session and dest must be temp session
	if (m_bTempSession || !pSession->m_bTempSession)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x session error"), m_iSessionId);
		return;
	}

	m_dwConnectionTime = pSession->m_dwConnectionTime;
	m_dwLoggedInTime = pSession->m_dwLoggedInTime;
	m_pConnection = pSession->m_pConnection;
	wcscpy_s(m_strAccountName, _countof(m_strAccountName), pSession->m_strAccountName);

	m_pConnection->SetClient(this);
}

int32 GateSession::CheckLoginToken(uint16 iLen, char* pBuf)
{
	int32 iRet = 0;
	// todo: pBuf's format is (Account;Password)
	char strAccountName[ACCOUNTNAME_MAX*3 + 1] = {0};
	char strPassword[PASSWORD_MAX+1] = {0};
	char* nextToken = NULL;

	if (m_bFinalizing)
	{
		return -1;
	}

	// get account and password from token
	char* pToken = strtok_s(pBuf, ";", &nextToken);
	if (!pToken || strlen(pToken) > ACCOUNTNAME_MAX*3 + 1)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token is invalid"), m_iSessionId);
		return -1;
	}

	strcpy_s(strAccountName, sizeof(strAccountName), pToken);

	pToken = strtok_s(NULL, ";", &nextToken);
	if (!pToken || strlen(pToken) > PASSWORD_MAX + 1)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token is invalid"), m_iSessionId);
		return -1;
	}

	strcpy_s(strPassword, sizeof(strPassword), pToken);

	iRet = Char2WChar(strAccountName, m_strAccountName, _countof(m_strAccountName));
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Char2WChar error"), m_iSessionId);
		return -1;
	}

	// check if accountName is empty
	if (wcslen(m_strAccountName) == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x m_strAccountName is empty"), m_iSessionId);
		return -1;
	}

	// change accountName to lowercase
	LowerCase(m_strAccountName);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Token is valid"), m_strAccountName, m_iSessionId);

	return 0;
}

int32 GateSession::LoggedInNtf()
{
	int32 iRet = 0;
	GateSession* pSession = NULL;

	if (m_bFinalizing)
	{
		return -1;
	}

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN, false) == -1)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}

	iRet = m_pMainLoop->TransferSession(m_iSessionId, m_strAccountName, pSession);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x TransferSession error"), m_strAccountName, m_iSessionId);
		Disconnect();
		return -1;
	}

	// after transfer session, check state again
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN) != SESSION_STATE_TRANSFERED)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x TempSession finishes itse task"), m_strAccountName, m_iSessionId);

	m_pMainLoop->CloseSession(this);

	return 0;
}

void GateSession::OnSessionTransfered()
{
	int32 iRet = 0;
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x gate login start"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(SESSION_EVENT_TRANSFERED) != SESSION_STATE_LOGGEDIN)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	iRet = MasterPeerSend::OnGateLoginReq(g_pServer->m_pMasterServer, m_iSessionId, wcslen(m_strAccountName)+1, m_strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnGateLoginReq failed"), m_strAccountName, m_iSessionId);
		Disconnect();
		return;
	}

	if (m_StateMachine.StateTransition(SESSION_EVENT_GATELOGINREQ) != SESSION_STATE_GATELOGINREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}
}

void GateSession::OnMasterDisconnect()
{
	if (m_bFinalizing)
	{
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(SESSION_EVENT_ONMASTERDISCONNECT) != SESSION_STATE_ONMASTERDISCONNECT)
	{
		LOG_WAR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	switch(m_StateMachine.GetCurrState())
	{
	case SESSION_STATE_ONMASTERDISCONNECT:
		m_pConnection->AsyncDisconnect();
		break;

	case SESSION_STATE_ONDISCONNECT:
		m_pMainLoop->CloseSession(this, true);
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		break;
	}
}

int32 GateSession::HandlePacket(ServerPacket* pPacket)
{
	DelaySendData(pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return super::HandlePacket(pPacket);
}

int32 GateSession::DelaySendData(uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((GateSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}
