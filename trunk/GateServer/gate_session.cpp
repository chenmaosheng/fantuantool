#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"
#include "master_peer_send.h"
#include "gate_server_config.h"
#include "session_peer_send.h"

GateServerLoop* GateSession::m_pMainLoop = NULL;

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
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDISCONNECT) != SESSION_STATE_ONDISCONNECT)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Set new state failed"), m_iSessionId);
		return;
	}

	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	switch (m_StateMachine.GetCurrState())
	{
	case SESSION_STATE_ONDISCONNECT:
		iRet = SessionPeerSend::OnSessionDisconnect(g_pServer->m_pMasterServer, m_iSessionId);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnSessionDisconnect failed"), m_strAccountName, m_iSessionId);
			return;
		}

		m_pMainLoop->CloseSession(this);
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		break;
	}
}

void GateSession::Disconnect()
{
	LOG_DBG(LOG_SERVER, _T("sid=%08x force to disconnect"), m_iSessionId);
	super::Disconnect();
}

void GateSession::OnGateAllocReq(uint32 iLoginSessionId, const TCHAR *strAccountName)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONGATEALLOCREQ) != SESSION_STATE_ONGATEALLOCREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	((SessionId*)&m_iSessionId)->sValue_.sequence_++;
	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);
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
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%d sid=%08x send gate allocate ack to master server"), strAccountName, m_iSessionId);
}

void GateSession::OnGateReleaseReq()
{
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x connection is disconnected"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(SESSION_EVENT_GATERELEASEREQ) != SESSION_STATE_GATERELEASEREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		
		m_pMainLoop->CloseSession(this);
		return;
	}

	m_pMainLoop->CloseSession(this);
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

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN, false) == -1)
	{
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
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN, false) != SESSION_STATE_TRANSFERED)
	{
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
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}
}

void GateSession::OnMasterDisconnect()
{
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect"), m_strAccountName, m_iSessionId);

	Connection::Close(m_pConnection);
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((GateSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}
